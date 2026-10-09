#ifndef ADAPTER_QINFINEBACKEND_H
#define ADAPTER_QINFINEBACKEND_H

#include "domain/ipumpbackend.h"

class McuPortAgent;

/** Thin terminal: physical flow/press/tables live on HPLC_PUMP_Mini. */
class QinFinePumpBackend : public IPumpBackend
{
public:
    explicit QinFinePumpBackend(McuPortAgent *agent, QObject *parent = nullptr);

    Kind kind() const override { return QinFine; }
    bool ownsLocalGradient() const override { return false; }
    bool ownsLocalPressureScale() const override { return false; }

    bool open(AppSettings *s) override;
    void close() override;

    void applyFlow(double mlMin, bool flowCalibActive) override;
    void applyStop() override;
    void applyPause(bool on) override;
    void applyPurge(double purgeFlow) override;
    void applyRun(double flow, double percent) override;
    void applyPercent(double percent) override;
    void applyPressLimits(double pmin, double pmax) override;
    void applyPressZero() override;
    void applyPressCompen(quint8 on) override;
    void applyLoadParams(double rate, double real, double press) override;
    void applyWorkMode(quint8 mode, quint8 flag) override;
    void applyFlowCalib(bool on) override;
    void applyPressCalib(bool on) override;
    void writeFlowTable(const QVector<RatePoint> &t) override;
    void writePressTable(const QVector<PressPoint> &t) override;
    void writePulseTable(const QVector<PulsePoint> &t, bool save) override;
    void clearPulseTable() override;
    void requestFlowTable() override;
    void requestPressTable() override;
    void requestPulseTable() override;
    void requestLoadFloats() override;
    void setTableCmd(quint8 sub, quint8 cmd) override;
    void setTablePoint(quint8 sub, float a, float b) override;
    void setLoadFloat(quint8 sub, float v) override;

private:
    McuPortAgent *m_agent = nullptr;
};

#endif
