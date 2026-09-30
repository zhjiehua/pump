#ifndef MACHINECONTROLLER_H
#define MACHINECONTROLLER_H

#include <QObject>
#include <QVector>
#include <QPair>
#include <QByteArray>

class QTimer;
#include "core/appsettings.h"
#include "core/runstatemachine.h"
#include "core/alarmservice.h"
#include "core/authservice.h"
#include "core/gradientengine.h"
#include "core/usagetracker.h"
#include "core/commworker.h"
#include "core/i18nmanager.h"

class QinFineClient;
class CxthMcuClient;
class PcServer;
class QinFinePcServer;

class MachineController : public QObject
{
    Q_OBJECT
public:
    using Stat = RunStateMachine::Stat;

    explicit MachineController(QObject *parent = nullptr);

    AppSettings *settings() { return &m_settings; }
    AlarmService *alarms() { return &m_alarms; }
    AuthService *auth() { return &m_auth; }
    GradientEngine *gradient() { return &m_gradient; }
    UsageTracker *usage() { return &m_usage; }
    I18nManager *i18n() { return &m_i18n; }
    QinFineClient *qinFine() { return m_qinFine; }
    CxthMcuClient *cxthMcu() { return m_cxthMcu; }
    bool usingQinFine() const { return m_settings.mcuProtocol == AppSettings::QinFine; }
    bool usingCxthMcu() const { return !usingQinFine(); }
    bool usingQinFinePc() const { return m_settings.pcProtocol == AppSettings::QinFinePc; }

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

    void setFlow(double mlMin, bool sendMcu = true);
    void setPercent(double percent, bool sendMcu = true);
    void setPressLimits(double pmin, double pmax, bool sendMcu = true);
    void setStat(Stat s);
    void enterPcControl();
    void start();
    void stop();
    void pause();
    void purge();
    void pump();
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

    void applyQinFineExtSet(quint8 sub, const QByteArray &payload);
    void requestQinFineDump(int kind);
    void armDump(int kind);
    void replyPressureToPc();
    void dumpFlowToPc();
    void dumpPressToPc();
    void dumpPulseToPc();
    void postLog(const QString &line) { emit logLine(line); }
    bool importJsonConfig(const QString &path);

signals:
    void statusChanged();
    void pressureChanged();
    void tablesChanged();
    void logLine(const QString &line);
    void alarmChanged();
    void probationExpired();

private slots:
    void onMcuConnectedChanged(bool on);
    void onLogLineEcho(const QString &line);
    void onDumpTimeout();
    void onPollTick();
    void onSecondTick();
    void onQinFinePressure(float mpa);
    void onCxthPressureRaw(quint32 raw);
    void onExtPoint(quint8 sub, float a, float b);
    void onExtFloat(quint8 sub, float v);
    void onExtU8(quint8 sub, quint8 v);
    void autoConnectStartup();

private:
    void applyStatToMcu(Stat s);
    void sendFlowToMcu(double mlMin);
    void sendStopToMcu();
    bool qinFineReady() const;
    void updatePressureAlarms();
    PcServer *activePc() const;

    AppSettings m_settings;
    RunStateMachine m_runState;
    AlarmService m_alarms;
    AuthService m_auth;
    GradientEngine m_gradient;
    UsageTracker m_usage;
    CommWorker m_commWorker;
    I18nManager m_i18n;
    QinFineClient *m_qinFine = nullptr;
    CxthMcuClient *m_cxthMcu = nullptr;
    PcServer *m_cxthPc = nullptr;
    PcServer *m_clarityPc = nullptr;
    QinFinePcServer *m_qinFinePc = nullptr;
    double m_flow = 1.0;
    double m_percent = 100.0;
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
