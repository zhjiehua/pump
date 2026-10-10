#ifndef MACHINECONTROLLER_H
#define MACHINECONTROLLER_H

#include <QObject>
#include <QVector>
#include <QPair>
#include <QByteArray>

#include "app/pumpcommand.h"
#include "app/controlauthority.h"
#include "core/appsettings.h"
#include "core/runstatemachine.h"
#include "core/alarmservice.h"
#include "core/authservice.h"
#include "core/gradientengine.h"
#include "core/usagetracker.h"
#include "core/i18nmanager.h"
#include "core/recordstore.h"
#include "domain/pumpsession.h"
#include "domain/uicapabilities.h"
#include "domain/ipumpbackend.h"

class QTimer;
class QThread;
class McuPortAgent;
class PcPortAgent;

class MachineController : public PumpCommand
{
    Q_OBJECT
public:
    using Stat = RunStateMachine::Stat;

    explicit MachineController(QObject *parent = nullptr);
    ~MachineController() override;

    AppSettings *settings() { return &m_settings; }
    AlarmService *alarms() { return &m_alarms; }
    AuthService *auth() { return &m_auth; }
    GradientEngine *gradient() { return &m_gradient; }
    UsageTracker *usage() { return &m_usage; }
    I18nManager *i18n() { return &m_i18n; }
    RecordStore *records() { return &m_records; }
    PumpSession *session() { return &m_session; }
    ControlAuthority *authority() { return &m_authority; }
    UiCapabilities capabilities() const;

    Stat stat() const { return m_runState.stat(); }
    double flow() const { return m_flow; }
    double percent() const { return m_percent; }
    double pressure() const { return m_pressure; }
    bool mcuOpen() const { return m_mcuOpen; }
    bool pcOpen() const { return m_pcStarted; }
    bool linkOk() const;
    quint32 runSeconds() const { return m_runState.runSeconds(); }

    QVector<RatePoint> flowTable() const { return m_flowTable; }
    QVector<PressPoint> pressTable() const { return m_pressTable; }
    QVector<PulsePoint> pulseTable() const { return m_pulseTable; }
    quint8 pressCompen() const { return m_pressCompen; }
    double loadRate() const { return m_loadRate; }
    double loadReal() const { return m_loadReal; }
    double loadPress() const { return m_loadPress; }

    bool connectMcu();
    void disconnectMcu();
    bool connectPc();
    void disconnectPc();

    using PumpCommand::setFlow;
    void setFlow(double mlMin, bool sendMcu);
    void setPercent(double percent, bool sendMcu = true);
    void setPressLimits(double pmin, double pmax, bool sendMcu = true);
    void setStat(Stat s);
    void pressZero();
    void setPressCompen(quint8 on);
    void setLoadParams(double rate, double real, double press);

    void setWorkMode(quint8 mode, quint8 flag);
    void setFlowCalibActive(bool on);
    void setPressCalibActive(bool on);
    void writeFlowTable(const QVector<RatePoint> &t);
    void writePressTable(const QVector<PressPoint> &t);
    void writePulseTable(const QVector<PulsePoint> &t, bool save);
    void clearPulseTable();
    void requestFlowTable();
    void requestPressTable();
    void requestPulseTable();

    void postLog(const QString &line) { emit logLine(line); }
    bool importJsonConfig(const QString &path);
    void reloadFromSettings();

public slots:
    bool startCmd(int source) override;
    bool stopCmd(int source) override;
    bool pauseCmd(int source) override;
    bool purgeCmd(int source) override;
    bool pumpCmd(int source) override;
    bool setFlowCmd(double mlMin, int source) override;
    bool setPercentCmd(double percent, int source) override;
    bool setPressLimitsCmd(double pmin, double pmax, int source) override;
    bool pressZeroCmd(int source) override;
    bool enterPcControlCmd(int source) override;
    bool setPressCompenCmd(int on, int source) override;
    bool pcApplyFlowCmd(double mlMin, double percent, int source) override;
    bool pcPumpStartCmd(int source) override;
    bool pcTimeSyncCmd(int ticks, int source) override;

    void applyQinFineExtSet(int sub, const QByteArray &payload);
    void requestQinFineDump(int kind);
    void replyPressureToPc();

signals:
    void statusChanged();
    void pressureChanged();
    void tablesChanged();
    void logLine(const QString &line);
    void alarmChanged();
    void probationExpired();
    void capabilitiesChanged();

private slots:
    void onMcuConnectedChanged(bool on);
    void onLogLineEcho(const QString &line);
    void onDumpTimeout();
    void onPollTick();
    void onSecondTick();
    void onQinFinePressure(float mpa);
    void onCxthPressureRaw(uint raw);
    void onExtPoint(int sub, float a, float b);
    void onExtFloat(int sub, float v);
    void onExtU8(int sub, int v);
    void autoConnectStartup();
    void syncSession();
    void onPcSyncTimedOut();

private:
    bool gate(int source);
    void recreateBackend();
    bool backendQinFine() const;
    void applyStatToMcu(Stat s);
    void sendFlowToMcu(double mlMin);
    void sendStopToMcu();
    void armDump(int kind);
    void dumpFlowToPc();
    void dumpPressToPc();
    void dumpPulseToPc();
    void updatePressureAlarms();
    QVariantMap pcLinkConfig() const;

    AppSettings m_settings;
    RunStateMachine m_runState;
    AlarmService m_alarms;
    AuthService m_auth;
    GradientEngine m_gradient;
    UsageTracker m_usage;
    I18nManager m_i18n;
    RecordStore m_records;
    PumpSession m_session;
    ControlAuthority m_authority;

    QThread *m_ioThread = nullptr;
    McuPortAgent *m_mcuAgent = nullptr;
    PcPortAgent *m_pcAgent = nullptr;
    IPumpBackend *m_backend = nullptr;

    double m_flow = 1.0;
    double m_percent = 100.0;
    double m_pcFlow = 0;
    bool m_pcPumpOn = false;
    double m_pressure = 0;
    quint32 m_lastPressRaw = 0;
    bool m_mcuOpen = false;
    bool m_pcStarted = false;
    QVector<RatePoint> m_flowTable;
    QVector<PressPoint> m_pressTable;
    QVector<PulsePoint> m_pulseTable;
    quint8 m_pressCompen = 0;
    double m_loadRate = 0;
    double m_loadReal = 0;
    double m_loadPress = 0;
    int m_dumpKind = 0;
    QTimer *m_dumpTimer = nullptr;
    QTimer *m_secondTimer = nullptr;
    int m_flowDumpId = 0;
    int m_flowDumpOpen = 0;
    bool m_flowDumpStarted = false;
    int m_pressDumpId = 0;
    int m_pressDumpOpen = 0;
    bool m_pressDumpStarted = false;
    int m_pulseDumpId = 0;
    int m_pulseDumpOpen = 0;
    bool m_pulseDumpStarted = false;
    bool m_flowCalibActive = false;
    bool m_pressCalibActive = false;
};

#endif
