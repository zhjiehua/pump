#include "core/machinecontroller.h"
#include "core/calibinterp.h"
#include "adapter/iocall.h"
#include "adapter/mcuportagent.h"
#include "adapter/pcportagent.h"
#include "adapter/qinfinebackend.h"
#include "adapter/cxthbackend.h"
#include "protocol/mcu/qinfine/qinfinecodec.h"
#include "utils/eventlog.h"

#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QVariantMap>

MachineController::MachineController(QObject *parent)
    : PumpCommand(parent)
    , m_auth(&m_settings, this)
    , m_gradient(&m_settings, this)
    , m_usage(&m_settings, this)
    , m_i18n(&m_settings, this)
    , m_session(this)
    , m_authority(this)
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
    m_records.bindToEventLog();
    EventLog_key(QStringLiteral("BOOT"), QStringLiteral("HMI ready"));

    m_ioThread = new QThread(this);
    m_ioThread->setObjectName(QStringLiteral("hmi_io"));
    m_mcuAgent = new McuPortAgent;
    m_pcAgent = new PcPortAgent;
    m_mcuAgent->moveToThread(m_ioThread);
    m_pcAgent->moveToThread(m_ioThread);
    m_ioThread->start();
    IoCall::blockingVoid(m_mcuAgent, "init");
    IoCall::blockingVoid(m_pcAgent, "init");
    QMetaObject::invokeMethod(m_pcAgent, "setFacade", IoCall::blocking(m_pcAgent),
                              Q_ARG(QObject*, this), Q_ARG(QObject*, static_cast<QObject*>(&m_session)));

    connect(m_mcuAgent, SIGNAL(connectedChanged(bool)), this, SLOT(onMcuConnectedChanged(bool)),
            Qt::QueuedConnection);
    connect(m_mcuAgent, SIGNAL(qfPressure(float)), this, SLOT(onQinFinePressure(float)),
            Qt::QueuedConnection);
    connect(m_mcuAgent, SIGNAL(cxthPressureRaw(uint)), this, SLOT(onCxthPressureRaw(uint)),
            Qt::QueuedConnection);
    connect(m_mcuAgent, SIGNAL(extPoint(int,float,float)), this, SLOT(onExtPoint(int,float,float)),
            Qt::QueuedConnection);
    connect(m_mcuAgent, SIGNAL(extFloat(int,float)), this, SLOT(onExtFloat(int,float)),
            Qt::QueuedConnection);
    connect(m_mcuAgent, SIGNAL(extU8(int,int)), this, SLOT(onExtU8(int,int)),
            Qt::QueuedConnection);
    connect(m_mcuAgent, SIGNAL(errorText(QString)), this, SIGNAL(logLine(QString)),
            Qt::QueuedConnection);
    connect(m_mcuAgent, SIGNAL(pollTick()), this, SLOT(onPollTick()), Qt::QueuedConnection);
    connect(this, SIGNAL(logLine(QString)), this, SLOT(onLogLineEcho(QString)));
    connect(&m_runState, SIGNAL(statChanged(Stat)), this, SIGNAL(statusChanged()));
    connect(&m_runState, SIGNAL(runTimeChanged(quint32)), this, SIGNAL(statusChanged()));
    connect(&m_alarms, SIGNAL(alarmChanged()), this, SIGNAL(alarmChanged()));
    connect(&m_authority, SIGNAL(changed()), this, SIGNAL(statusChanged()));
    connect(&m_authority, SIGNAL(timedOut()), this, SLOT(onPcSyncTimedOut()));
    connect(this, SIGNAL(statusChanged()), this, SLOT(syncSession()));
    connect(this, SIGNAL(pressureChanged()), this, SLOT(syncSession()));
    connect(this, SIGNAL(tablesChanged()), this, SLOT(syncSession()));
    connect(this, SIGNAL(alarmChanged()), this, SLOT(syncSession()));

    recreateBackend();

    m_secondTimer = new QTimer(this);
    connect(m_secondTimer, SIGNAL(timeout()), this, SLOT(onSecondTick()));
    m_secondTimer->start(1000);

    m_dumpTimer = new QTimer(this);
    m_dumpTimer->setSingleShot(true);
    connect(m_dumpTimer, SIGNAL(timeout()), this, SLOT(onDumpTimeout()));

    syncSession();
    if (m_settings.autoConnect)
        QTimer::singleShot(0, this, SLOT(autoConnectStartup()));
}

MachineController::~MachineController()
{
    if (m_mcuAgent)
        disconnect(m_mcuAgent, nullptr, this, nullptr);
    if (m_pcAgent)
        disconnect(m_pcAgent, nullptr, this, nullptr);
    if (m_mcuAgent)
        IoCall::blockingVoid(m_mcuAgent, "shutdown");
    if (m_pcAgent)
        IoCall::blockingVoid(m_pcAgent, "shutdown");
    if (m_ioThread)
    {
        m_ioThread->quit();
        m_ioThread->wait(2000);
    }
    delete m_mcuAgent;
    m_mcuAgent = nullptr;
    delete m_pcAgent;
    m_pcAgent = nullptr;
}

void MachineController::recreateBackend()
{
    delete m_backend;
    m_backend = nullptr;
    if (m_settings.mcuProtocol == AppSettings::QinFine)
        m_backend = new QinFinePumpBackend(m_mcuAgent, this);
    else
        m_backend = new CxthPumpBackend(m_mcuAgent, &m_settings, this);
    emit capabilitiesChanged();
}

bool MachineController::backendQinFine() const
{
    return m_backend && m_backend->kind() == IPumpBackend::QinFine;
}

UiCapabilities MachineController::capabilities() const
{
    return UiCapabilities::fromMcuProtocol(int(m_settings.mcuProtocol));
}

bool MachineController::gate(int source)
{
    // weiduodianzi: local shortcuts silently ignore while bSyncFlag.
    return m_authority.allows(source);
}

void MachineController::onMcuConnectedChanged(bool on)
{
    m_mcuOpen = on;
    m_alarms.setAlarm(AlarmService::CommunicationErr, !on && m_settings.autoConnect);
    EventLog_key(QStringLiteral("MCU"),
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
    if (!m_mcuOpen || !m_backend || !m_backend->ownsLocalGradient())
        return;
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
    m_authority.tickSecond(m_settings.pcProtocol == AppSettings::Clarity);
    m_usage.tick(m_runState.isPumpRunning());
    if (m_auth.checkProbationExpired())
        emit probationExpired();
}

void MachineController::onPcSyncTimedOut()
{
    EventLog_key(QStringLiteral("STATE"), QStringLiteral("PC time-sync timeout -> Pause"));
    // CDS ended while still in PcCtrl: keep last flow (weiduodianzi Pause).
    // Explicit STOP already parked the pump; do not revive it as Pause.
    if (stat() != Stat::PcCtrl)
        return;
    setStat(Stat::Pause);
}

void MachineController::syncSession()
{
    PumpSession::Snap s;
    s.flow = m_flow;
    s.percent = m_percent;
    s.pressure = m_pressure;
    s.stat = int(stat());
    s.linkOk = linkOk();
    s.mcuOpen = m_mcuOpen;
    s.pcOpen = m_pcStarted;
    s.remote = m_authority.isRemote();
    s.runSeconds = m_runState.runSeconds();
    s.flowTable = m_flowTable;
    s.pressTable = m_pressTable;
    s.pulseTable = m_pulseTable;
    s.loadRate = m_loadRate;
    s.loadReal = m_loadReal;
    s.loadPress = m_loadPress;
    s.pressMin = m_settings.pressMin;
    s.pressMax = m_settings.pressMax;
    s.pressCompen = m_pressCompen;
    s.machineCode = m_settings.machineCode;
    s.mcuAddress = m_settings.mcuAddress;
    s.pumpType = m_settings.pumpType;
    s.pressWarnLevel = m_alarms.pressWarnLevel();
    m_session.update(s);
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
    reloadFromSettings();
    emit logLine(tr("JSON imported from %1").arg(path));
    return true;
}

void MachineController::reloadFromSettings()
{
    m_flow = m_settings.flowSet;
    m_percent = m_settings.percent;
    m_gradient.reload();
    m_i18n.applyFromSettings();
    recreateBackend();
    emit statusChanged();
    emit tablesChanged();
}

void MachineController::sendFlowToMcu(double mlMin)
{
    if (!m_mcuOpen || !m_backend)
        return;
    m_backend->applyFlow(mlMin, m_flowCalibActive);
}

void MachineController::sendStopToMcu()
{
    if (!m_mcuOpen || !m_backend)
        return;
    m_backend->applyStop();
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
    recreateBackend();
    const bool ok = m_backend && m_backend->open(&m_settings);
    m_mcuOpen = ok;
    m_alarms.setAlarm(AlarmService::CommunicationErr, !ok && m_settings.autoConnect);
    emit statusChanged();
    emit alarmChanged();
    if (ok)
        emit logLine(backendQinFine() ? tr("MCU QinFine connected") : tr("MCU CXTH connected"));
    else
        EventLog_key(QStringLiteral("MCU"), QStringLiteral("connect failed %1").arg(m_settings.mcuPort));
    return ok;
}

void MachineController::disconnectMcu()
{
    EventLog_key(QStringLiteral("MCU"), QStringLiteral("disconnect requested"));
    if (m_backend)
        m_backend->close();
    m_mcuOpen = false;
    emit statusChanged();
}

QVariantMap MachineController::pcLinkConfig() const
{
    QVariantMap cfg;
    cfg.insert(QStringLiteral("pcProtocol"), int(m_settings.pcProtocol));
    cfg.insert(QStringLiteral("pcPort"), int(m_settings.pcPort));
    cfg.insert(QStringLiteral("pcSerialPort"), m_settings.pcSerialPort);
    cfg.insert(QStringLiteral("pcSerialBaud"), m_settings.pcSerialBaud);
    cfg.insert(QStringLiteral("localPort"), int(m_settings.localPort));
    cfg.insert(QStringLiteral("remoteIp"), m_settings.remoteIp);
    cfg.insert(QStringLiteral("remotePort"), int(m_settings.remotePort));
    cfg.insert(QStringLiteral("machineCode"), int(m_settings.machineCode));
    cfg.insert(QStringLiteral("mcuAddress"), int(m_settings.mcuAddress));
    return cfg;
}

bool MachineController::connectPc()
{
    disconnectPc();
    m_pcStarted = IoCall::blockingBool(m_pcAgent, "startLink", pcLinkConfig());
    if (!m_pcStarted)
        EventLog_key(QStringLiteral("PC"), QStringLiteral("connect failed"));
    emit statusChanged();
    return m_pcStarted;
}

void MachineController::disconnectPc()
{
    EventLog_key(QStringLiteral("PC"), QStringLiteral("disconnect requested"));
    IoCall::blockingVoid(m_pcAgent, "stopLink");
    m_pcStarted = false;
    m_authority.leaveRemote();
    if (stat() == Stat::PcCtrl)
        m_runState.setStat(Stat::Stop);
    emit statusChanged();
}

void MachineController::setFlow(double mlMin, bool sendMcu)
{
    m_flow = mlMin;
    m_settings.flowSet = mlMin;
    EventLog_key(QStringLiteral("FLOW"),
                  QStringLiteral("set %1 mL/min sendMcu=%2")
                      .arg(mlMin, 0, 'f', 3)
                      .arg(int(sendMcu)));
    if (sendMcu && m_mcuOpen && stat() != Stat::Stop && stat() != Stat::Pause)
        sendFlowToMcu(mlMin);
    emit statusChanged();
}

void MachineController::setPercent(double percent, bool sendMcu)
{
    m_percent = percent;
    m_settings.percent = percent;
    EventLog_key(QStringLiteral("FLOW"),
                  QStringLiteral("percent %1%% sendMcu=%2")
                      .arg(percent, 0, 'f', 1)
                      .arg(int(sendMcu)));
    if (sendMcu && m_mcuOpen && m_backend)
        m_backend->applyPercent(percent);
    emit statusChanged();
}

void MachineController::setPressLimits(double pmin, double pmax, bool sendMcu)
{
    m_settings.pressMin = pmin;
    m_settings.pressMax = pmax;
    EventLog_key(QStringLiteral("PRESS"),
                  QStringLiteral("limits min=%1 max=%2 sendMcu=%3")
                      .arg(pmin, 0, 'f', 2)
                      .arg(pmax, 0, 'f', 2)
                      .arg(int(sendMcu)));
    if (sendMcu && m_mcuOpen && m_backend)
        m_backend->applyPressLimits(pmin, pmax);
    m_settings.save();
    emit statusChanged();
}

void MachineController::applyStatToMcu(Stat s)
{
    if (!m_mcuOpen || !m_backend)
        return;
    if (s == Stat::Stop)
        sendStopToMcu();
    else if (s == Stat::Pause)
        m_backend->applyPause(true);
    else if (s == Stat::Purge)
        m_backend->applyPurge(m_settings.purgeFlow);
    else if (s == Stat::Pump)
    {
        const double flow = m_gradient.flowAtElapsed(0);
        m_backend->applyRun(flow > 0 ? flow : m_flow, m_percent);
    }
    else if (s == Stat::PcCtrl)
    {
        // weiduodianzi pcCtrlMachine: switch UI/gradient, do not command motor.
        return;
    }
    else
        m_backend->applyRun(m_flow, m_percent);
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
        EventLog_key(QStringLiteral("STATE"),
                      QStringLiteral("%1 -> %2").arg(EventLog::runStatName(prev), EventLog::runStatName(s)));
    m_runState.setStat(s);
    applyStatToMcu(s);
    emit statusChanged();
}

bool MachineController::startCmd(int source)
{
    if (!gate(source))
        return false;
    setStat(Stat::Running);
    return true;
}

bool MachineController::stopCmd(int source)
{
    if (!gate(source))
        return false;
    if (source == CmdSource::Remote)
        m_pcPumpOn = false;
    m_authority.leaveRemote();
    setStat(Stat::Stop);
    return true;
}

bool MachineController::pauseCmd(int source)
{
    if (!gate(source))
        return false;
    setStat(stat() == Stat::Pause ? Stat::Running : Stat::Pause);
    return true;
}

bool MachineController::purgeCmd(int source)
{
    if (!gate(source))
        return false;
    setStat(Stat::Purge);
    return true;
}

bool MachineController::pumpCmd(int source)
{
    if (!gate(source))
        return false;
    setStat(Stat::Pump);
    return true;
}

bool MachineController::setFlowCmd(double mlMin, int source)
{
    if (!gate(source))
        return false;
    setFlow(mlMin, true);
    return true;
}

bool MachineController::setPercentCmd(double percent, int source)
{
    if (!gate(source))
        return false;
    setPercent(percent, true);
    return true;
}

bool MachineController::setPressLimitsCmd(double pmin, double pmax, int source)
{
    if (!gate(source))
        return false;
    setPressLimits(pmin, pmax, true);
    return true;
}

bool MachineController::pressZeroCmd(int source)
{
    if (!gate(source))
        return false;
    pressZero();
    return true;
}

bool MachineController::enterPcControlCmd(int source)
{
    if (!gate(source))
        return false;
    EventLog_key(QStringLiteral("STATE"), QStringLiteral("enter PC control"));
    m_settings.currentGradient = 10;
    setStat(Stat::PcCtrl);
    return true;
}

bool MachineController::pcApplyFlowCmd(double mlMin, double percent, int source)
{
    if (!gate(source))
        return false;
    m_pcFlow = mlMin;
    m_flow = mlMin;
    m_percent = percent;
    if (m_pcPumpOn)
        sendFlowToMcu(m_pcFlow);
    emit statusChanged();
    return true;
}

bool MachineController::pcPumpStartCmd(int source)
{
    if (!gate(source))
        return false;
    m_pcPumpOn = true;
    if (stat() != Stat::PcCtrl)
    {
        setStat(Stat::Purge);
        sendFlowToMcu(m_pcFlow);
    }
    return true;
}

bool MachineController::pcTimeSyncCmd(int ticks, int source)
{
    if (!gate(source))
        return false;
    m_authority.noteTimeSync();
    const bool overpress = m_alarms.primaryAlarm() == AlarmService::OverpressErr;
    if (stat() != Stat::PcCtrl && !overpress)
        enterPcControlCmd(source);
    m_runState.setRunSeconds(quint32(ticks * 0.6 + 0.5));
    emit statusChanged();
    return true;
}

bool MachineController::setPressCompenCmd(int on, int source)
{
    if (!gate(source))
        return false;
    setPressCompen(quint8(on));
    return true;
}

void MachineController::pressZero()
{
    if (backendQinFine())
    {
        if (m_mcuOpen && m_backend)
            m_backend->applyPressZero();
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
    if (m_mcuOpen && m_backend)
        m_backend->applyPressCompen(on);
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
    if (m_mcuOpen && m_backend)
        m_backend->applyLoadParams(rate, real, press);
    m_settings.save();
    emit tablesChanged();
}

void MachineController::setWorkMode(quint8 mode, quint8 flag)
{
    if (m_mcuOpen && m_backend)
        m_backend->applyWorkMode(mode, flag);
}

void MachineController::setFlowCalibActive(bool on)
{
    if (m_flowCalibActive == on)
        return;
    m_flowCalibActive = on;
    if (m_mcuOpen && m_backend)
        m_backend->applyFlowCalib(on);
    if (!backendQinFine() && m_mcuOpen && stat() != Stat::Stop && stat() != Stat::Pause)
        sendFlowToMcu(m_flow);
}

void MachineController::setPressCalibActive(bool on)
{
    if (m_pressCalibActive == on)
        return;
    m_pressCalibActive = on;
    if (m_mcuOpen && m_backend)
        m_backend->applyPressCalib(on);
}

void MachineController::writeFlowTable(const QVector<RatePoint> &t)
{
    const QVector<RatePoint> table = CalibInterp::sanitizeFlowTable(t);
    m_flowDumpOpen = 0;
    m_flowDumpStarted = false;
    m_flowTable = table;
    m_settings.flowTable = table;
    if (m_mcuOpen && m_backend)
        m_backend->writeFlowTable(table);
    m_settings.save();
    emit tablesChanged();
    if (!backendQinFine() && m_mcuOpen && !m_flowCalibActive
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
    if (m_mcuOpen && m_backend)
        m_backend->writePressTable(table);
    m_settings.save();
    emit tablesChanged();
}

void MachineController::writePulseTable(const QVector<PulsePoint> &t, bool save)
{
    m_pulseDumpOpen = 0;
    m_pulseDumpStarted = false;
    m_pulseTable = t;
    m_settings.pulseTable = t;
    if (m_mcuOpen && m_backend)
        m_backend->writePulseTable(t, save);
    m_settings.save();
    emit tablesChanged();
}

void MachineController::clearPulseTable()
{
    m_pulseDumpOpen = 0;
    m_pulseDumpStarted = false;
    m_pulseTable.clear();
    m_settings.pulseTable.clear();
    if (m_mcuOpen && m_backend)
        m_backend->clearPulseTable();
    m_settings.save();
    emit tablesChanged();
}

void MachineController::requestFlowTable()
{
    m_flowDumpOpen = 0;
    m_flowDumpStarted = false;
    if (!backendQinFine() || !m_mcuOpen)
    {
        m_flowTable = m_settings.flowTable;
        emit tablesChanged();
        return;
    }
    m_flowDumpOpen = ++m_flowDumpId;
    m_backend->requestFlowTable();
}

void MachineController::requestPressTable()
{
    m_pressDumpOpen = 0;
    m_pressDumpStarted = false;
    if (!backendQinFine() || !m_mcuOpen)
    {
        m_pressTable = m_settings.pressTable;
        emit tablesChanged();
        return;
    }
    m_pressDumpOpen = ++m_pressDumpId;
    m_backend->requestPressTable();
}

void MachineController::requestPulseTable()
{
    if (!backendQinFine() || !m_mcuOpen)
    {
        m_pulseTable = m_settings.pulseTable;
        emit tablesChanged();
        return;
    }
    m_pulseDumpOpen = ++m_pulseDumpId;
    m_pulseDumpStarted = false;
    m_backend->requestPulseTable();
}

void MachineController::applyQinFineExtSet(int subIn, const QByteArray &payload)
{
    const quint8 sub = quint8(subIn);
    EventLog_key(QStringLiteral("CALIB"),
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
        if (m_mcuOpen && m_backend)
            m_backend->setTableCmd(sub, cmd);
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
            if (m_mcuOpen && m_backend)
                m_backend->setTablePoint(sub, float(p.rpm), float(p.rate));
            emit tablesChanged();
        }
        break;
    case QinFine::PES_PRESS_DATA:
        if (payload.size() >= 8)
        {
            const PressPoint p(double(QinFine::beFloat(payload, 0)), double(QinFine::beFloat(payload, 4)));
            m_pressTable.append(p);
            if (m_mcuOpen && m_backend)
                m_backend->setTablePoint(sub, float(p.adc), float(p.pressure));
            emit tablesChanged();
        }
        break;
    case QinFine::PES_PULSE_DATA:
        if (payload.size() >= 8)
        {
            const PulsePoint p(double(QinFine::beFloat(payload, 0)), double(QinFine::beFloat(payload, 4)));
            m_pulseTable.append(p);
            if (m_mcuOpen && m_backend)
                m_backend->setTablePoint(sub, float(p.position), float(p.factor));
            emit tablesChanged();
        }
        break;
    case QinFine::PES_LOAD_FLOW:
        m_loadRate = double(QinFine::beFloat(payload));
        m_settings.loadRate = m_loadRate;
        if (m_mcuOpen && m_backend)
            m_backend->setLoadFloat(sub, float(m_loadRate));
        m_settings.save();
        emit tablesChanged();
        break;
    case QinFine::PES_LOAD_REAL:
        m_loadReal = double(QinFine::beFloat(payload));
        m_settings.loadReal = m_loadReal;
        if (m_mcuOpen && m_backend)
            m_backend->setLoadFloat(sub, float(m_loadReal));
        m_settings.save();
        emit tablesChanged();
        break;
    case QinFine::PES_LOAD_PRESS:
        m_loadPress = double(QinFine::beFloat(payload));
        m_settings.loadPress = m_loadPress;
        if (m_mcuOpen && m_backend)
            m_backend->setLoadFloat(sub, float(m_loadPress));
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
        if (backendQinFine() && m_mcuOpen && m_backend)
        {
            requestFlowTable();
            m_backend->requestLoadFloats();
            armDump(1);
        }
        else
            dumpFlowToPc();
    }
    else if (kind == 2)
    {
        if (backendQinFine() && m_mcuOpen)
        {
            requestPressTable();
            armDump(2);
        }
        else
            dumpPressToPc();
    }
    else if (kind == 3)
    {
        if (backendQinFine() && m_mcuOpen)
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
    IoCall::queued(m_pcAgent, "sendPressure", m_pressure);
}

void MachineController::dumpFlowToPc()
{
    IoCall::queued(m_pcAgent, "dumpFlowTable");
}

void MachineController::dumpPressToPc()
{
    IoCall::queued(m_pcAgent, "dumpPressTable");
}

void MachineController::dumpPulseToPc()
{
    IoCall::queued(m_pcAgent, "dumpPulseTable");
}

void MachineController::updatePressureAlarms()
{
    const bool hadOverpress = m_alarms.primaryAlarm() == AlarmService::OverpressErr;
    m_alarms.checkPressure(m_pressure, m_runState.runSeconds(), m_settings.pressMin,
                           m_settings.pressMax);
    if (m_alarms.primaryAlarm() == AlarmService::OverpressErr)
    {
        if (!hadOverpress)
            EventLog_key(QStringLiteral("PRESS"),
                          QStringLiteral("over-limit %1 MPa, auto stop").arg(m_pressure, 0, 'f', 3));
        stopCmd(m_authority.isSyncLocked() ? CmdSource::Remote : CmdSource::Local);
    }
    emit alarmChanged();
}

void MachineController::onQinFinePressure(float mpa)
{
    m_pressure = mpa;
    emit pressureChanged();
    updatePressureAlarms();
}

void MachineController::onCxthPressureRaw(uint raw)
{
    m_lastPressRaw = quint32(raw);
    double val = (double(raw) - double(m_settings.pressRawV0)) * m_settings.pressRawScale;
    if (val < 0)
        val = 0;
    if (!m_pressCalibActive)
        val = CalibInterp::displayPressFromTable(m_pressTable, val);
    m_pressure = val;
    emit pressureChanged();
    updatePressureAlarms();
}

void MachineController::onExtPoint(int sub, float a, float b)
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

void MachineController::onExtFloat(int sub, float v)
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

void MachineController::onExtU8(int sub, int v)
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
