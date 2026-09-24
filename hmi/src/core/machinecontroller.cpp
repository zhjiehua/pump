#include "core/machinecontroller.h"
#include "protocol/qinfineclient.h"
#include "protocol/qinfinecodec.h"
#include "protocol/legacymcuclient.h"
#include "protocol/cxthpcserver.h"
#include "protocol/cxthpcids.h"
#include "utils/eventlog.h"

#include <QDebug>
#include <QTimer>

MachineController::MachineController(QObject *parent)
    : QObject(parent)
    , m_auth(&m_settings, this)
    , m_gradient(&m_settings, this)
    , m_usage(&m_settings, this)
    , m_i18n(&m_settings, this)
{
    m_settings.load();
    qInfo() << "settings loaded from"
            << m_settings.deviceInfoPath() << m_settings.configPath() << m_settings.dataPath();
    m_flow = m_settings.flowSet;
    m_percent = m_settings.percent;
    m_flowTable = m_settings.flowTable;
    m_pressTable = m_settings.pressTable;
    m_pulseTable = m_settings.pulseTable;
    m_pressCompen = m_settings.pressCompen;
    m_loadRate = m_settings.loadRate;
    m_loadReal = m_settings.loadReal;
    m_loadPress = m_settings.loadPress;
    m_auth.ensureSerial();
    m_gradient.reload(m_settings.gradientIndex);
    m_i18n.applyFromSettings();

    m_qinFine = new QinFineClient(this);
    m_legacy = new LegacyMcuClient(this);
    m_pc = new CxthPcServer(this);
    m_pc->setController(this);

    connect(m_qinFine, SIGNAL(connectedChanged(bool)), this, SLOT(onMcuConnectedChanged(bool)));
    connect(m_legacy, SIGNAL(connectedChanged(bool)), this, SLOT(onMcuConnectedChanged(bool)));
    connect(m_qinFine, SIGNAL(pressureUpdated(float)), this, SLOT(onQinFinePressure(float)));
    connect(m_legacy, SIGNAL(pressureRaw(quint32)), this, SLOT(onLegacyPressureRaw(quint32)));
    connect(m_qinFine, SIGNAL(extPoint(quint8,float,float)), this, SLOT(onExtPoint(quint8,float,float)));
    connect(m_qinFine, SIGNAL(extFloat(quint8,float)), this, SLOT(onExtFloat(quint8,float)));
    connect(m_qinFine, SIGNAL(extU8(quint8,quint8)), this, SLOT(onExtU8(quint8,quint8)));
    connect(m_qinFine, SIGNAL(errorText(QString)), this, SIGNAL(logLine(QString)));
    connect(m_legacy, SIGNAL(errorText(QString)), this, SIGNAL(logLine(QString)));
    connect(this, SIGNAL(logLine(QString)), this, SLOT(onLogLineEcho(QString)));
    connect(&m_commWorker, SIGNAL(pollTick()), this, SLOT(onPollTick()));
    connect(&m_runState, SIGNAL(statChanged()), this, SIGNAL(statusChanged()));
    connect(&m_runState, SIGNAL(runTimeChanged()), this, SIGNAL(statusChanged()));
    connect(&m_alarms, SIGNAL(alarmChanged()), this, SIGNAL(alarmChanged()));

    m_commWorker.startPolling(500);

    m_secondTimer = new QTimer(this);
    connect(m_secondTimer, SIGNAL(timeout()), this, SLOT(onSecondTick()));
    m_secondTimer->start(1000);

    m_dumpTimer = new QTimer(this);
    m_dumpTimer->setSingleShot(true);
    connect(m_dumpTimer, SIGNAL(timeout()), this, SLOT(onDumpTimeout()));

    if (m_settings.autoConnect)
        QTimer::singleShot(0, this, SLOT(autoConnectStartup()));
}

void MachineController::onMcuConnectedChanged(bool on)
{
    m_mcuOpen = on;
    m_alarms.setAlarm(AlarmService::CommunicationErr, !on && m_settings.autoConnect);
    EventLog::key(QStringLiteral("MCU"),
                  on ? QStringLiteral("connected") : QStringLiteral("disconnected"));
    emit statusChanged();
    emit alarmChanged();
}

void MachineController::onLogLineEcho(const QString &line)
{
    qInfo() << line;
}

void MachineController::onDumpTimeout()
{
    const int kind = m_dumpKind;
    m_dumpKind = 0;
    if (kind == 1)
        dumpFlowToPc();
    else if (kind == 2)
        dumpPressToPc();
    else if (kind == 3)
        dumpPulseToPc();
}

bool MachineController::linkOk() const
{
    if (m_alarms.primaryAlarm() == AlarmService::CommunicationErr)
        return false;
    return m_mcuOpen || !m_settings.autoConnect;
}

void MachineController::autoConnectStartup()
{
    connectMcu();
    connectPc();
}

void MachineController::onPollTick()
{
    if (!m_mcuOpen)
        return;
    if (usingQinFine())
    {
        m_qinFine->tick();
        m_qinFine->getPressure();
    }
    else
    {
        m_legacy->pollPressure();
    }
    if (stat() == Stat::Running && m_settings.gradientIndex < 10)
    {
        double stepFlow = 0;
        if (m_gradient.flowChangedAt(m_runState.runSeconds(), &stepFlow))
            setFlow(stepFlow, true);
    }
}

void MachineController::onSecondTick()
{
    m_runState.tickSecond();
    m_usage.tick(m_runState.isPumpRunning());
    if (m_auth.checkProbationExpired())
        emit probationExpired();
}

bool MachineController::importJsonConfig(const QString &path)
{
    if (path.isEmpty())
    {
        emit logLine(tr("JSON import: empty path"));
        return false;
    }
    if (!m_settings.importFromJson(path))
    {
        emit logLine(tr("JSON import failed: %1").arg(path));
        return false;
    }
    m_flow = m_settings.flowSet;
    m_percent = m_settings.percent;
    m_gradient.reload(m_settings.gradientIndex);
    m_i18n.applyFromSettings();
    emit statusChanged();
    emit logLine(tr("JSON imported from %1").arg(path));
    return true;
}

bool MachineController::qinFineReady() const
{
    return usingQinFine() && m_mcuOpen && m_qinFine && m_qinFine->isOpen();
}

void MachineController::sendFlowToMcu(double mlMin)
{
    if (!m_mcuOpen)
        return;
    EventLog::key(QStringLiteral("MCU-TX"),
                  QStringLiteral("set flow %.3f mL/min").arg(mlMin, 0, 'f', 3));
    if (usingQinFine())
        m_qinFine->setFlow(float(mlMin));
    else
    {
        const quint32 word = quint32(qMax(0.0, mlMin * m_settings.mcuWordFactor + 0.5));
        m_legacy->setFlowWord(word);
    }
}

void MachineController::sendStopToMcu()
{
    if (!m_mcuOpen)
        return;
    EventLog::key(QStringLiteral("MCU-TX"), QStringLiteral("stop motor"));
    if (usingQinFine())
    {
        m_qinFine->setStartStop(false);
        m_qinFine->setPurge(false);
    }
    else
        m_legacy->stopMotor();
}

void MachineController::armDump(int kind)
{
    m_dumpKind = kind;
    if (m_dumpTimer)
        m_dumpTimer->start(400);
}

bool MachineController::connectMcu()
{
    disconnectMcu();
    bool ok = false;
    if (usingQinFine())
    {
        const int baud = m_settings.mcuBaud > 0 ? m_settings.mcuBaud : 115200;
        ok = m_qinFine->open(m_settings.mcuPort, baud, m_settings.mcuAddress);
    }
    else
    {
        const int baud = m_settings.mcuBaud > 0 ? m_settings.mcuBaud : 9600;
        ok = m_legacy->open(m_settings.mcuPort, baud);
    }
    m_mcuOpen = ok;
    m_alarms.setAlarm(AlarmService::CommunicationErr, !ok && m_settings.autoConnect);
    emit statusChanged();
    emit alarmChanged();
    if (ok)
        emit logLine(usingQinFine() ? tr("MCU QinFine connected") : tr("MCU Legacy connected"));
    else
        EventLog::key(QStringLiteral("MCU"), QStringLiteral("connect failed %1").arg(m_settings.mcuPort));
    return ok;
}

void MachineController::disconnectMcu()
{
    EventLog::key(QStringLiteral("MCU"), QStringLiteral("disconnect requested"));
    m_qinFine->close();
    m_legacy->close();
    m_mcuOpen = false;
    emit statusChanged();
}

bool MachineController::connectPc()
{
    m_pcStarted = m_pc->start(&m_settings);
    if (!m_pcStarted)
        EventLog::key(QStringLiteral("PC"), QStringLiteral("connect failed"));
    emit statusChanged();
    return m_pcStarted;
}

void MachineController::disconnectPc()
{
    EventLog::key(QStringLiteral("PC"), QStringLiteral("disconnect requested"));
    m_pc->stop();
    m_pcStarted = false;
    emit statusChanged();
}

void MachineController::setFlow(double mlMin, bool sendMcu)
{
    m_flow = mlMin;
    m_settings.flowSet = mlMin;
    EventLog::key(QStringLiteral("FLOW"),
                  QStringLiteral("set %.3f mL/min sendMcu=%1").arg(mlMin, 0, 'f', 3).arg(sendMcu));
    if (sendMcu && m_mcuOpen && stat() != Stat::Stop && stat() != Stat::Pause)
        sendFlowToMcu(mlMin);
    emit statusChanged();
}

void MachineController::setPercent(double percent, bool sendMcu)
{
    m_percent = percent;
    m_settings.percent = percent;
    EventLog::key(QStringLiteral("FLOW"),
                  QStringLiteral("percent %.1f%% sendMcu=%1").arg(percent, 0, 'f', 1).arg(sendMcu));
    if (sendMcu && qinFineReady())
        m_qinFine->setPercent(quint8(qBound(0.0, percent, 100.0)));
    emit statusChanged();
}

void MachineController::setPressLimits(double pmin, double pmax, bool sendMcu)
{
    m_settings.pressMin = pmin;
    m_settings.pressMax = pmax;
    EventLog::key(QStringLiteral("PRESS"),
                  QStringLiteral("limits min=%.2f max=%.2f sendMcu=%1")
                      .arg(pmin, 0, 'f', 2)
                      .arg(pmax, 0, 'f', 2)
                      .arg(sendMcu));
    if (sendMcu && qinFineReady())
    {
        m_qinFine->setPressMin(float(pmin));
        m_qinFine->setPressMax(float(pmax));
    }
    emit statusChanged();
}

void MachineController::applyStatToMcu(Stat s)
{
    if (!m_mcuOpen)
        return;
    if (s == Stat::Stop)
    {
        sendStopToMcu();
    }
    else if (s == Stat::Pause)
    {
        if (usingQinFine())
            m_qinFine->setPause(true);
    }
    else if (s == Stat::Purge)
    {
        if (usingQinFine())
        {
            m_qinFine->setFlow(float(m_settings.purgeFlow));
            m_qinFine->setPurge(true);
            m_qinFine->setStartStop(true);
        }
        else
            sendFlowToMcu(m_settings.purgeFlow);
    }
    else if (s == Stat::Pump)
    {
        const double flow = m_gradient.flowAtElapsed(0);
        if (usingQinFine())
        {
            m_qinFine->setPause(false);
            m_qinFine->setPurge(false);
            m_qinFine->setFlow(float(flow > 0 ? flow : m_flow));
            m_qinFine->setStartStop(true);
        }
        else
            sendFlowToMcu(flow > 0 ? flow : m_flow);
    }
    else
    {
        if (usingQinFine())
        {
            m_qinFine->setPause(false);
            m_qinFine->setPurge(false);
            m_qinFine->setFlow(float(m_flow));
            m_qinFine->setPercent(quint8(qBound(0.0, m_percent, 100.0)));
            m_qinFine->setStartStop(true);
        }
        else
            sendFlowToMcu(m_flow);
    }
    if (s == Stat::Running)
    {
        m_gradient.reload(m_settings.gradientIndex);
        m_alarms.setAlarm(AlarmService::OverpressErr, false);
    }
}

void MachineController::setStat(Stat s)
{
    const Stat prev = stat();
    if (prev != s)
        EventLog::key(QStringLiteral("STATE"),
                      QStringLiteral("%1 -> %2").arg(EventLog::runStatName(prev), EventLog::runStatName(s)));
    m_runState.setStat(s);
    applyStatToMcu(s);
    emit statusChanged();
}

void MachineController::enterPcControl()
{
    EventLog::key(QStringLiteral("STATE"), QStringLiteral("enter PC control"));
    m_settings.currentGradient = 10;
    setStat(Stat::PcCtrl);
}

void MachineController::start()
{
    setStat(Stat::Running);
}

void MachineController::stop()
{
    setStat(Stat::Stop);
}

void MachineController::pause()
{
    setStat(stat() == Stat::Pause ? Stat::Running : Stat::Pause);
}

void MachineController::purge()
{
    setStat(Stat::Purge);
}

void MachineController::pump()
{
    setStat(Stat::Pump);
}

void MachineController::pressZero()
{
    if (usingQinFine())
    {
        if (m_mcuOpen)
            m_qinFine->pressZero();
    }
    else
    {
        m_settings.pressRawV0 = m_lastPressRaw;
        m_pressure = 0;
        m_settings.save();
        emit pressureChanged();
        emit logLine(tr("Legacy press zero (raw V0=%1)").arg(m_settings.pressRawV0));
    }
}

void MachineController::setPressCompen(quint8 on)
{
    m_pressCompen = on;
    m_settings.pressCompen = on;
    if (qinFineReady())
        m_qinFine->setPressCompen(on);
    m_settings.save();
    emit tablesChanged();
}

void MachineController::setLoadParams(double rate, double real, double press)
{
    m_loadRate = rate;
    m_loadReal = real;
    m_loadPress = press;
    m_settings.loadRate = rate;
    m_settings.loadReal = real;
    m_settings.loadPress = press;
    if (qinFineReady())
    {
        m_qinFine->setLoadFloat(QinFine::PES_LOAD_FLOW, float(rate));
        m_qinFine->setLoadFloat(QinFine::PES_LOAD_REAL, float(real));
        m_qinFine->setLoadFloat(QinFine::PES_LOAD_PRESS, float(press));
    }
    m_settings.save();
    emit tablesChanged();
}

void MachineController::setWorkMode(quint8 mode, quint8 flag)
{
    if (qinFineReady())
        m_qinFine->setWorkMode(mode, flag);
}

void MachineController::writeFlowTable(const QVector<RatePoint> &t)
{
    m_flowTable = t;
    m_settings.flowTable = t;
    if (!qinFineReady())
    {
        m_settings.save();
        emit tablesChanged();
        return;
    }
    m_qinFine->setTableCmd(QinFine::PES_FLOW_CMD, 1);
    for (const auto &p : t)
        m_qinFine->setTablePoint(QinFine::PES_FLOW_DATA, float(p.rpm), float(p.rate));
    m_qinFine->setTableCmd(QinFine::PES_FLOW_CMD, 0);
    m_settings.save();
    emit tablesChanged();
}

void MachineController::writePressTable(const QVector<PressPoint> &t)
{
    m_pressTable = t;
    m_settings.pressTable = t;
    if (!qinFineReady())
    {
        m_settings.save();
        emit tablesChanged();
        return;
    }
    m_qinFine->setTableCmd(QinFine::PES_PRESS_CMD, 1);
    for (const auto &p : t)
        m_qinFine->setTablePoint(QinFine::PES_PRESS_DATA, float(p.adc), float(p.pressure));
    m_qinFine->setTableCmd(QinFine::PES_PRESS_CMD, 0);
    m_settings.save();
    emit tablesChanged();
}

void MachineController::writePulseTable(const QVector<PulsePoint> &t, bool save)
{
    m_pulseTable = t;
    m_settings.pulseTable = t;
    if (!qinFineReady())
    {
        m_settings.save();
        emit tablesChanged();
        return;
    }
    m_qinFine->setTableCmd(QinFine::PES_PULSE_CMD, 1);
    for (const auto &p : t)
        m_qinFine->setTablePoint(QinFine::PES_PULSE_DATA, float(p.position), float(p.factor));
    m_qinFine->setTableCmd(QinFine::PES_PULSE_CMD, 0);
    if (save)
        m_qinFine->setTableCmd(QinFine::PES_PULSE_CMD, 2);
    m_settings.save();
    emit tablesChanged();
}

void MachineController::clearPulseTable()
{
    m_pulseTable.clear();
    m_settings.pulseTable.clear();
    if (qinFineReady())
        m_qinFine->setTableCmd(QinFine::PES_PULSE_CMD, 3);
    m_settings.save();
    emit tablesChanged();
}

void MachineController::requestFlowTable()
{
    m_flowTable.clear();
    if (qinFineReady())
        m_qinFine->getTable(QinFine::PES_FLOW_DATA);
    emit tablesChanged();
}

void MachineController::requestPressTable()
{
    m_pressTable.clear();
    if (qinFineReady())
        m_qinFine->getTable(QinFine::PES_PRESS_DATA);
    emit tablesChanged();
}

void MachineController::requestPulseTable()
{
    m_pulseTable.clear();
    if (qinFineReady())
        m_qinFine->getTable(QinFine::PES_PULSE_DATA);
    emit tablesChanged();
}

void MachineController::applyCalibCmd(quint8 ai, quint32 value)
{
    using namespace CxthPc;
    EventLog::key(QStringLiteral("CALIB"),
                  QStringLiteral("ai=0x%1 value=%2").arg(ai, 2, 16, QLatin1Char('0')).arg(value));
    if (!usingQinFine())
    {
        emit logLine(tr("Calib tables require QinFine MCU protocol"));
        return;
    }
    switch (ai)
    {
    case AI_WORKMODE:
        setWorkMode(quint8(value / 1000), quint8(value % 1000));
        break;
    case AI_FLOW_CMD:
        if (value == CALIB_DUMP)
        {
            if (qinFineReady())
            {
                requestFlowTable();
                m_qinFine->getLoadFloat(QinFine::PES_LOAD_FLOW);
                m_qinFine->getLoadFloat(QinFine::PES_LOAD_REAL);
                m_qinFine->getLoadFloat(QinFine::PES_LOAD_PRESS);
                armDump(1);
            }
            else
                dumpFlowToPc();
            break;
        }
        if (qinFineReady())
            m_qinFine->setTableCmd(QinFine::PES_FLOW_CMD, quint8(value));
        if (value == CALIB_BEGIN)
            m_flowTable.clear();
        break;
    case AI_FLOW_A:
        m_pendingKind = 1;
        m_pendingA = unpackCount(value);
        m_hasPendingA = true;
        break;
    case AI_FLOW_B:
        if (m_hasPendingA && m_pendingKind == 1)
        {
            const RatePoint p(m_pendingA, unpackMilli(value));
            m_flowTable.append(p);
            if (qinFineReady())
                m_qinFine->setTablePoint(QinFine::PES_FLOW_DATA, float(p.rpm), float(p.rate));
            m_hasPendingA = false;
            emit tablesChanged();
        }
        break;
    case AI_PRESS_CMD:
        if (value == CALIB_DUMP)
        {
            if (qinFineReady())
            {
                requestPressTable();
                armDump(2);
            }
            else
                dumpPressToPc();
            break;
        }
        if (qinFineReady())
            m_qinFine->setTableCmd(QinFine::PES_PRESS_CMD, quint8(value));
        if (value == CALIB_BEGIN)
            m_pressTable.clear();
        break;
    case AI_PRESS_A:
        m_pendingKind = 2;
        m_pendingA = unpackCount(value);
        m_hasPendingA = true;
        break;
    case AI_PRESS_B:
        if (m_hasPendingA && m_pendingKind == 2)
        {
            const PressPoint p(m_pendingA, unpackMilli(value));
            m_pressTable.append(p);
            if (qinFineReady())
                m_qinFine->setTablePoint(QinFine::PES_PRESS_DATA, float(p.adc), float(p.pressure));
            m_hasPendingA = false;
            emit tablesChanged();
        }
        break;
    case AI_PULSE_CMD:
        if (value == CALIB_DUMP)
        {
            if (qinFineReady())
            {
                requestPulseTable();
                armDump(3);
            }
            else
                dumpPulseToPc();
            break;
        }
        if (qinFineReady())
            m_qinFine->setTableCmd(QinFine::PES_PULSE_CMD, quint8(value));
        if (value == CALIB_BEGIN || value == CALIB_CLEAR)
            m_pulseTable.clear();
        emit tablesChanged();
        break;
    case AI_PULSE_A:
        m_pendingKind = 3;
        m_pendingA = unpackCount(value);
        m_hasPendingA = true;
        break;
    case AI_PULSE_B:
        if (m_hasPendingA && m_pendingKind == 3)
        {
            const PulsePoint p(m_pendingA, unpackMilli(value));
            m_pulseTable.append(p);
            if (qinFineReady())
                m_qinFine->setTablePoint(QinFine::PES_PULSE_DATA, float(p.position), float(p.factor));
            m_hasPendingA = false;
            emit tablesChanged();
        }
        break;
    case AI_PRESS_COMPEN:
        if (value <= 1)
            setPressCompen(quint8(value));
        m_pc->sendCalib(AI_PRESS_COMPEN, m_pressCompen);
        break;
    case AI_LOAD_FLOW:
        m_loadRate = unpackMilli(value);
        if (qinFineReady())
            m_qinFine->setLoadFloat(QinFine::PES_LOAD_FLOW, float(m_loadRate));
        emit tablesChanged();
        break;
    case AI_LOAD_REAL:
        m_loadReal = unpackMilli(value);
        if (qinFineReady())
            m_qinFine->setLoadFloat(QinFine::PES_LOAD_REAL, float(m_loadReal));
        emit tablesChanged();
        break;
    case AI_LOAD_PRESS:
        m_loadPress = unpackMilli(value);
        if (qinFineReady())
            m_qinFine->setLoadFloat(QinFine::PES_LOAD_PRESS, float(m_loadPress));
        emit tablesChanged();
        break;
    default:
        break;
    }
}

void MachineController::replyPressureToPc()
{
    m_pc->sendPressure(m_pressure);
}

void MachineController::dumpFlowToPc()
{
    m_pc->dumpFlowTable();
}

void MachineController::dumpPressToPc()
{
    m_pc->dumpPressTable();
}

void MachineController::dumpPulseToPc()
{
    m_pc->dumpPulseTable();
}

void MachineController::updatePressureAlarms()
{
    const bool hadOverpress = m_alarms.primaryAlarm() == AlarmService::OverpressErr;
    m_alarms.checkPressure(m_pressure, m_runState.runSeconds(), m_settings.pressMin,
                           m_settings.pressMax);
    if (m_alarms.primaryAlarm() == AlarmService::OverpressErr)
    {
        if (!hadOverpress)
            EventLog::key(QStringLiteral("PRESS"),
                          QStringLiteral("over-limit %.3f MPa, auto stop").arg(m_pressure, 0, 'f', 3));
        stop();
    }
    emit alarmChanged();
}

void MachineController::onQinFinePressure(float mpa)
{
    m_pressure = mpa;
    emit pressureChanged();
    updatePressureAlarms();
}

void MachineController::onLegacyPressureRaw(quint32 raw)
{
    m_lastPressRaw = raw;
    double val = (double(raw) - double(m_settings.pressRawV0)) * m_settings.pressRawScale;
    if (val < 0)
        val = 0;
    m_pressure = val;
    emit pressureChanged();
    updatePressureAlarms();
}

void MachineController::onExtPoint(quint8 sub, float a, float b)
{
    if (sub == QinFine::PES_FLOW_DATA)
        m_flowTable.append(RatePoint(a, b));
    else if (sub == QinFine::PES_PRESS_DATA)
        m_pressTable.append(PressPoint(a, b));
    else if (sub == QinFine::PES_PULSE_DATA)
        m_pulseTable.append(PulsePoint(a, b));
    if (m_dumpKind && m_dumpTimer)
        m_dumpTimer->start(400);
    emit tablesChanged();
}

void MachineController::onExtFloat(quint8 sub, float v)
{
    if (sub == QinFine::PES_LOAD_FLOW)
        m_loadRate = v;
    else if (sub == QinFine::PES_LOAD_REAL)
        m_loadReal = v;
    else if (sub == QinFine::PES_LOAD_PRESS)
        m_loadPress = v;
    if (m_dumpKind && m_dumpTimer)
        m_dumpTimer->start(400);
    emit tablesChanged();
}

void MachineController::onExtU8(quint8 sub, quint8 v)
{
    Q_UNUSED(sub);
    Q_UNUSED(v);
}
