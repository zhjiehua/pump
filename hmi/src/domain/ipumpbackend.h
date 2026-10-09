#ifndef DOMAIN_IPUMPBACKEND_H
#define DOMAIN_IPUMPBACKEND_H

#include <QObject>
#include <QVector>
#include "core/appsettings.h"

class AppSettings;

/**
 * Protocol-dependent pump domain. QinFine is a thin MCU mirror;
 * Cxth computes word factor / pressure scale / local gradient on the HMI.
 */
class IPumpBackend : public QObject
{
    Q_OBJECT
public:
    enum Kind { Cxth = 0, QinFine = 1 };

    explicit IPumpBackend(QObject *parent = nullptr) : QObject(parent) {}

    virtual Kind kind() const = 0;
    virtual bool ownsLocalGradient() const = 0;
    virtual bool ownsLocalPressureScale() const = 0;

    virtual bool open(AppSettings *s) = 0;
    virtual void close() = 0;

    virtual void applyFlow(double mlMin, bool flowCalibActive) = 0;
    virtual void applyStop() = 0;
    virtual void applyPause(bool on) = 0;
    virtual void applyPurge(double purgeFlow) = 0;
    virtual void applyRun(double flow, double percent) = 0;
    virtual void applyPercent(double percent) = 0;
    virtual void applyPressLimits(double pmin, double pmax) = 0;
    virtual void applyPressZero() = 0;
    virtual void applyPressCompen(quint8 on) = 0;
    virtual void applyLoadParams(double rate, double real, double press) = 0;
    virtual void applyWorkMode(quint8 mode, quint8 flag) = 0;
    virtual void applyFlowCalib(bool on) = 0;
    virtual void applyPressCalib(bool on) = 0;
    virtual void writeFlowTable(const QVector<RatePoint> &t) = 0;
    virtual void writePressTable(const QVector<PressPoint> &t) = 0;
    virtual void writePulseTable(const QVector<PulsePoint> &t, bool save) = 0;
    virtual void clearPulseTable() = 0;
    virtual void requestFlowTable() = 0;
    virtual void requestPressTable() = 0;
    virtual void requestPulseTable() = 0;
    virtual void requestLoadFloats() = 0;
    virtual void setTableCmd(quint8 sub, quint8 cmd) = 0;
    virtual void setTablePoint(quint8 sub, float a, float b) = 0;
    virtual void setLoadFloat(quint8 sub, float v) = 0;
};

#endif
