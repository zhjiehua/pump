#ifndef DOMAIN_PUMPSESSION_H
#define DOMAIN_PUMPSESSION_H

#include <QMutex>
#include <QObject>
#include <QVector>
#include "core/appsettings.h"

/**
 * Thread-safe snapshot of pump runtime. GUI writes; IO/CDS may copy().
 * Signals are emitted only from the GUI thread.
 */
class PumpSession : public QObject
{
    Q_OBJECT
public:
    struct Snap {
        double flow = 0;
        double percent = 100;
        double pressure = 0;
        int stat = 0;
        bool linkOk = false;
        bool mcuOpen = false;
        bool pcOpen = false;
        bool remote = false;
        quint32 runSeconds = 0;
        QVector<RatePoint> flowTable;
        QVector<PressPoint> pressTable;
        QVector<PulsePoint> pulseTable;
        double loadRate = 0;
        double loadReal = 0;
        double loadPress = 0;
        double pressMin = 0;
        double pressMax = 0;
        quint8 pressCompen = 0;
        quint8 machineCode = 0;
        quint8 mcuAddress = 1;
        int pumpType = 0;
        int pressWarnLevel = 0;
    };

    explicit PumpSession(QObject *parent = nullptr);

    Snap copy() const;
    void update(const Snap &s);

signals:
    void snapshotChanged();
    void pressureChanged();
    void alarmChanged();
    void tablesChanged();

private:
    mutable QMutex m_mu;
    Snap m_snap;
};

#endif
