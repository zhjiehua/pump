#include "core/machinecontroller.h"
#include "core/calibinterp.h"
#include "protocol/mcu/qinfine/qinfineclient.h"
#include "protocol/mcu/qinfine/qinfinecodec.h"
#include "protocol/mcu/cxth/cxthmcuclient.h"
#include "protocol/pc/pcserver.h"
#include "protocol/pc/cxth/cxthpcserver.h"
#include "protocol/pc/clarity/claritypcserver.h"
#include "protocol/pc/qinfine/qinfinepcserver.h"
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
    m_gradient.reload();
    m_i18n.applyFromSettings();

    m_qinFine = new QinFineClient(this);
    m_cxthMcu = new CxthMcuClient(this);
    m_cxthPc = new CxthPcServer(this);
    m_clarityPc = new ClarityPcServer(this);
    m_qinFinePc = new QinFinePcServer(this);
    m_cxthPc->setController(this);
    m_clarityPc->setController(this);
    m_qinFinePc->setController(this);

    connect(m_qinFine, SIGNAL(connectedChanged(bool)), this, SLOT(onMcuConnectedChanged(bool)));
    connect(m_cxthMcu, SIGNAL(connectedChanged(bool)), this, SLOT(onMcuConnectedChanged(bool)));
    connect(m_qinFine, SIGNAL(pressureUpdated(float)), this, SLOT(onQinFinePressure(float)));
    connect(m_cxthMcu, SIGNAL(pressureRaw(quint32)), this, SLOT(onCxthPressureRaw(quint32)));
    connect(m_qinFine, SIGNAL(extPoint(quint8,float,float)), this, SLOT(onExtPoint(quint8,float,float)));
    connect(m_qinFine, SIGNAL(extFloat(quint8,float)), this, SLOT(onExtFloat(quint8,float)));
    connect(m_qinFine, SIGNAL(extU8(quint8,quint8)), this, SLOT(onExtU8(quint8,quint8)));
    connect(m_qinFine, SIGNAL(errorText(QString)), this, SIGNAL(logLine(QString)));
    connect(m_cxthMcu, SIGNAL(errorText(QString)), this, SIGNAL(logLine(QString)));
    connect(this, SIGNAL(logLine(QString)), this, SLOT(onLogLineEcho(QString)));
    connect(&m_commWorker, SIGNAL(pollTick()), this, SLOT(onPollTick()));
    connect(&m_runState, SIGNAL(statChanged(Stat)), this, SIGNAL(statusChanged()));
    connect(&m_runState, SIGNAL(runTimeChanged(quint32)), this, SIGNAL(statusChanged()));
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
        m_cxthMcu->pollPressure();
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
    m_gradient.reload();
    m_i18n.applyFromSettings();
    emit statusChanged();
    emit logLine(tr("JSON imported from %1").arg(path));
    return true;
}

bool MachineController::qinFineReady() const
{
    return usingQinFine() && m_mcuOpen && m_qinFine && m_qinFine->isOpen();
}

PcServer *MachineController::activePc() const
{
    if (m_settings.pcProtocol == AppSettings::Clarity)
        return m_clarityPc;
    if (m_settings.pcProtocol == AppSettings::QinFinePc)
        return m_qinFinePc;
    return m_cxthPc;
}

void MachineController::sendFlowToMcu(double mlMin)
{
    if (!m_mcuOpen)
        return;
    double out = mlMin;
    if (!usingQinFine() && !m_flowCalibActive)
        out = CalibInterp::commandFlowFromTable(m_flowTable, mlMin);
    EventLog::key(QStringLiteral("MCU-TX"),
                  QStringLiteral("set flow %1 mL/min (cmd %2)")
                      .arg(mlMin, 0, 'f', 3)
                      .arg(out, 0, 'f', 3));
    if (usingQinFine())
        m_qinFine->setFlow(float(mlMin));
    else
    {
        const quint32 word = quint32(qMax(0.0, out * m_settings.mcuWordFactor + 0.5));
        m_cxthMcu->setFlowWord(word);
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
        m_cxthMcu->stopMotor();
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
        ok = m_cxthMcu->open(m_settings.mcuPort, baud);
    }
    m_mcuOpen = ok;
    m_alarms.setAlarm(AlarmService::CommunicationErr, !ok && m_settings.autoConnect);
    emit statusChanged();
    emit alarmChanged();
    if (ok)
        emit logLine(usingQinFine() ? tr("MCU QinFine connected") : tr("MCU CXTH connected"));
    else
        EventLog::key(QStringLiteral("MCU"), QStringLiteral("connect failed %1").arg(m_settings.mcuPort));
    return ok;
}

void MachineController::disconnectMcu()
{
    EventLog::key(QStringLiteral("MCU"), QStringLiteral("disconnect requested"));
    m_qinFine->close();
    m_cxthMcu->close();
    m_mcuOpen = false;
    emit statusChanged();
}

bool MachineController::connectPc()
{
    disconnectPc();
    m_pcStarted = activePc()->start(&m_settings);
    if (!m_pcStarted)
        EventLog::key(QStringLiteral("PC"), QStringLiteral("connect failed"));
    emit statusChanged();
    return m_pcStarted;
}

void MachineController::disconnectPc()
{
    EventLog::key(QStringLiteral("PC"), QStringLiteral("disconnect requested"));
    m_cxthPc->stop();
    m_clarityPc->stop();
    m_qinFinePc->stop();
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
        m_gradient.reload();
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
        emit logLine(tr("CXTH press zero (raw V0=%1)").arg(m_settings.pressRawV0));
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

void MachineController::setFlowCalibActive(bool on)
{
    if (m_flowCalibActive == on)
        return;
    m_flowCalibActive = on;
    if (qinFineReady())
        m_qinFine->setWorkMode(QinFine::WORK_FLOWCALIB, on ? 1 : 0);
    if (!usingQinFine() && m_mcuOpen && stat() != Stat::Stop && stat() != Stat::Pause)
        sendFlowToMcu(m_flow);
}

void MachineController::setPressCalibActive(bool on)
{
    if (m_pressCalibActive == on)
        return;
    m_pressCalibActive = on;
    if (qinFineReady())
        m_qinFine->setWorkMode(QinFine::WORK_PRESSCALIB, on ? 1 : 0);
}

void MachineController::writeFlowTable(const QVector<RatePoint> &t)
{
    const QVector<RatePoint> table = CalibInterp::sanitizeFlowTable(t);
    m_flowDumpOpen = 0;
    m_flowDumpStarted = false;
    m_flowTable = table;
    m_settings.flowTable = table;
    if (qinFineReady())
    {
        m_qinFine->setTableCmd(QinFine::PES_FLOW_CMD, QinFine::TBL_BEGIN);
        for (int i = 0; i < table.size(); ++i)
            m_qinFine->setTablePoint(QinFine::PES_FLOW_DATA, float(table[i].rpm), float(table[i].rate));
        m_qinFine->setTableCmd(QinFine::PES_FLOW_CMD, QinFine::TBL_END);
    }
    m_settings.save();
    emit tablesChanged();
    if (!usingQinFine() && m_mcuOpen && !m_flowCalibActive
        && stat() != Stat::Stop && stat() != Stat::Pause)
        sendFlowToMcu(m_flow);
}

void MachineController::writePressTable(const QVector<PressPoint> &t)
{
    const QVector<PressPoint> table = CalibInterp::sanitizePressTable(t);
    m_pressDumpOpen = 0;
    m_pressDumpStarted = false;
    m_pressTable = table;
    m_settings.pressTable = table;
    if (qinFineReady())
    {
        m_qinFine->setTableCmd(QinFine::PES_PRESS_CMD, QinFine::TBL_BEGIN);
        for (int i = 0; i < table.size(); ++i)
            m_qinFine->setTablePoint(QinFine::PES_PRESS_DATA, float(table[i].adc),
                                     float(table[i].pressure));
        m_qinFine->setTableCmd(QinFine::PES_PRESS_CMD, QinFine::TBL_END);
    }
    m_settings.save();
    emit tablesChanged();
}

void MachineController::writePulseTable(const QVector<PulsePoint> &t, bool save)
{
    m_pulseDumpOpen = 0;
    m_pulseDumpStarted = false;
    m_pulseTable = t;
    m_settings.pulseTable = t;
    if (!qinFineReady())
    {
        m_settings.save();
        emit tablesChanged();
        return;
    }
    m_qinFine->setTableCmd(QinFine::PES_PULSE_CMD, QinFine::TBL_BEGIN);
    for (const auto &p : t)
        m_qinFine->setTablePoint(QinFine::PES_PULSE_DATA, float(p.position), float(p.factor));
    m_qinFine->setTableCmd(QinFine::PES_PULSE_CMD, QinFine::TBL_END);
    if (save)
        m_qinFine->setTableCmd(QinFine::PES_PULSE_CMD, QinFine::TBL_SAVE);
    m_settings.save();
    emit tablesChanged();
}

void MachineController::clearPulseTable()
{
    m_pulseDumpOpen = 0;
    m_pulseDumpStarted = false;
    m_pulseTable.clear();
    m_settings.pulseTable.clear();
    if (qinFineReady())
        m_qinFine->setTableCmd(QinFine::PES_PULSE_CMD, QinFine::TBL_CLEAR);
    m_settings.save();
    emit tablesChanged();
}

void MachineController::requestFlowTable()
{
    m_flowDumpOpen = 0;
    m_flowDumpStarted = false;
    if (!qinFineReady())
    {
        m_flowTable = m_settings.flowTable;
        emit tablesChanged();
        return;
    }
    m_flowDumpOpen = ++m_flowDumpId;
    m_qinFine->getTable(QinFine::PES_FLOW_DATA);
}

void MachineController::requestPressTable()
{
    m_pressDumpOpen = 0;
    m_pressDumpStarted = false;
    if (!qinFineReady())
    {
        m_pressTable = m_settings.pressTable;
        emit tablesChanged();
        return;
    }
    m_pressDumpOpen = ++m_pressDumpId;
    m_qinFine->getTable(QinFine::PES_PRESS_DATA);
}

void MachineController::requestPulseTable()
{
    if (!qinFineReady())
    {
        m_pulseTable = m_settings.pulseTable;
        emit tablesChanged();
        return;
    }
    m_pulseDumpOpen = ++m_pulseDumpId;
    m_pulseDumpStarted = false;
    m_qinFine->getTable(QinFine::PES_PULSE_DATA);
}

void MachineController::applyQinFineExtSet(quint8 sub, const QByteArray &payload)
{
    EventLog::key(QStringLiteral("CALIB"),
                  QStringLiteral("QinFine sub=0x%1 len=%2").arg(sub, 2, 16, QLatin1Char('0')).arg(payload.size()));
    switch (sub)
    {
    case QinFine::PES_WORKMODE:
        if (payload.size() >= 2)
            setWorkMode(quint8(payload.at(0)), quint8(payload.at(1)));
        break;
    case QinFine::PES_FLOW_CMD:
    case QinFine::PES_PRESS_CMD:
    case QinFine::PES_PULSE_CMD:
    {
        const quint8 cmd = QinFine::beU8(payload);
        if (cmd == 4)
        {
            if (sub == QinFine::PES_FLOW_CMD)
                requestQinFineDump(1);
            else if (sub == QinFine::PES_PRESS_CMD)
                requestQinFineDump(2);
            else
                requestQinFineDump(3);
            break;
        }
        if (qinFineReady())
            m_qinFine->setTableCmd(sub, cmd);
        if (cmd == QinFine::TBL_BEGIN)
        {
            if (sub == QinFine::PES_FLOW_CMD)
                m_flowTable.clear();
            else if (sub == QinFine::PES_PRESS_CMD)
                m_pressTable.clear();
            else
                m_pulseTable.clear();
        }
        else if (sub == QinFine::PES_PULSE_CMD && cmd == QinFine::TBL_CLEAR)
            m_pulseTable.clear();
        if (cmd == QinFine::TBL_END || cmd == QinFine::TBL_SAVE)
        {
            if (sub == QinFine::PES_FLOW_CMD)
                m_settings.flowTable = m_flowTable;
            else if (sub == QinFine::PES_PRESS_CMD)
                m_settings.pressTable = m_pressTable;
            else
                m_settings.pulseTable = m_pulseTable;
            m_settings.save();
        }
        emit tablesChanged();
        break;
    }
    case QinFine::PES_FLOW_DATA:
        if (payload.size() >= 8)
        {
            const RatePoint p(double(QinFine::beFloat(payload, 0)), double(QinFine::beFloat(payload, 4)));
            m_flowTable.append(p);
            if (qinFineReady())
                m_qinFine->setTablePoint(sub, float(p.rpm), float(p.rate));
            emit tablesChanged();
        }
        break;
    case QinFine::PES_PRESS_DATA:
        if (payload.size() >= 8)
        {
            const PressPoint p(double(QinFine::beFloat(payload, 0)), double(QinFine::beFloat(payload, 4)));
            m_pressTable.append(p);
            if (qinFineReady())
                m_qinFine->setTablePoint(sub, float(p.adc), float(p.pressure));
            emit tablesChanged();
        }
        break;
    case QinFine::PES_PULSE_DATA:
        if (payload.size() >= 8)
        {
            const PulsePoint p(double(QinFine::beFloat(payload, 0)), double(QinFine::beFloat(payload, 4)));
            m_pulseTable.append(p);
            if (qinFineReady())
                m_qinFine->setTablePoint(sub, float(p.position), float(p.factor));
            emit tablesChanged();
        }
        break;
    case QinFine::PES_LOAD_FLOW:
        m_loadRate = double(QinFine::beFloat(payload));
        m_settings.loadRate = m_loadRate;
        if (qinFineReady())
            m_qinFine->setLoadFloat(sub, float(m_loadRate));
        m_settings.save();
        emit tablesChanged();
        break;
    case QinFine::PES_LOAD_REAL:
        m_loadReal = double(QinFine::beFloat(payload));
        m_settings.loadReal = m_loadReal;
        if (qinFineReady())
            m_qinFine->setLoadFloat(sub, float(m_loadReal));
        m_settings.save();
        emit tablesChanged();
        break;
    case QinFine::PES_LOAD_PRESS:
        m_loadPress = double(QinFine::beFloat(payload));
        m_settings.loadPress = m_loadPress;
        if (qinFineReady())
            m_qinFine->setLoadFloat(sub, float(m_loadPress));
        m_settings.save();
        emit tablesChanged();
        break;
    default:
        break;
    }
}

void MachineController::requestQinFineDump(int kind)
{
    if (kind == 1)
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
    }
    else if (kind == 2)
    {
        if (qinFineReady())
        {
            requestPressTable();
            armDump(2);
        }
        else
            dumpPressToPc();
    }
    else if (kind == 3)
    {
        if (qinFineReady())
        {
            requestPulseTable();
            armDump(3);
        }
        else
            dumpPulseToPc();
    }
}

void MachineController::replyPressureToPc()
{
    activePc()->sendPressure(m_pressure);
}

void MachineController::dumpFlowToPc()
{
    if (m_qinFinePc)
        m_qinFinePc->dumpFlowTable();
}

void MachineController::dumpPressToPc()
{
    if (m_qinFinePc)
        m_qinFinePc->dumpPressTable();
}

void MachineController::dumpPulseToPc()
{
    if (m_qinFinePc)
        m_qinFinePc->dumpPulseTable();
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

void MachineController::onCxthPressureRaw(quint32 raw)
{
    m_lastPressRaw = raw;
    double val = (double(raw) - double(m_settings.pressRawV0)) * m_settings.pressRawScale;
    if (val < 0)
        val = 0;
    if (!m_pressCalibActive)
        val = CalibInterp::displayPressFromTable(m_pressTable, val);
    m_pressure = val;
    emit pressureChanged();
    updatePressureAlarms();
}

void MachineController::onExtPoint(quint8 sub, float a, float b)
{
    if (sub == QinFine::PES_FLOW_DATA)
    {
        if (m_flowDumpOpen == 0 || m_flowDumpOpen != m_flowDumpId)
            return;
        if (!m_flowDumpStarted)
        {
            m_flowTable.clear();
            m_flowDumpStarted = true;
        }
        m_flowTable.append(RatePoint(a, b));
    }
    else if (sub == QinFine::PES_PRESS_DATA)
    {
        if (m_pressDumpOpen == 0 || m_pressDumpOpen != m_pressDumpId)
            return;
        if (!m_pressDumpStarted)
        {
            m_pressTable.clear();
            m_pressDumpStarted = true;
        }
        m_pressTable.append(PressPoint(a, b));
    }
    else if (sub == QinFine::PES_PULSE_DATA)
    {
        if (m_pulseDumpOpen == 0 || m_pulseDumpOpen != m_pulseDumpId)
            return;
        if (!m_pulseDumpStarted)
        {
            m_pulseTable.clear();
            m_pulseDumpStarted = true;
        }
        m_pulseTable.append(PulsePoint(a, b));
    }
    else
        return;
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
    const bool flowCmd = (sub == QinFine::PES_FLOW_CMD);
    const bool pressCmd = (sub == QinFine::PES_PRESS_CMD);
    if (!flowCmd && !pressCmd)
        return;

    if (flowCmd)
    {
        if (m_flowDumpOpen == 0 || m_flowDumpOpen != m_flowDumpId)
            return;
        if (v == QinFine::TBL_BEGIN)
        {
            m_flowTable.clear();
            m_flowDumpStarted = true;
        }
        else if (v == QinFine::TBL_END)
        {
            m_settings.flowTable = m_flowTable;
            m_settings.save();
        }
    }
    else
    {
        if (m_pressDumpOpen == 0 || m_pressDumpOpen != m_pressDumpId)
            return;
        if (v == QinFine::TBL_BEGIN)
        {
            m_pressTable.clear();
            m_pressDumpStarted = true;
        }
        else if (v == QinFine::TBL_END)
        {
            m_settings.pressTable = m_pressTable;
            m_settings.save();
        }
    }
    if (m_dumpKind && m_dumpTimer)
        m_dumpTimer->start(400);
    emit tablesChanged();
}
