#ifndef ADAPTER_CXTHBACKEND_H
#define ADAPTER_CXTHBACKEND_H

#include "domain/ipumpbackend.h"

class McuPortAgent;

/** Legacy brain: word factor, HMI pressure scale, local gradient. */
class CxthPumpBackend : public IPumpBackend
{
public:
    explicit CxthPumpBackend(McuPortAgent *agent, AppSettings *settings, QObject *parent = nullptr);

    Kind kind() const override { return Cxth; }
    bool ownsLocalGradient() const override { return true; }
    bool ownsLocalPressureScale() const override { return true; }

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

    void setFlowTable(const QVector<RatePoint> &t) { m_flowTable = t; }

private:
    void sendFlowWord(double mlMin, bool calibActive);

    McuPortAgent *m_agent = nullptr;
    AppSettings *m_settings = nullptr;
    QVector<RatePoint> m_flowTable;
};

#endif
