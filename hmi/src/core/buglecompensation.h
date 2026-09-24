#ifndef BUGLECOMPENSATION_H
#define BUGLECOMPENSATION_H

#include <QObject>

/** Cam pulse compensation stub (full PID logic in legacy BugleCompensationWithPID). */
class BugleCompensation : public QObject
{
    Q_OBJECT
public:
    static BugleCompensation *instance();

    void bugleSignal();
    void pauseOutput();
    void adjustOutputByInput(quint32 rawPress);

signals:
    void outputChanged(double factor);

private:
    explicit BugleCompensation(QObject *parent = nullptr);
    double m_factor = 1.0;
};

#endif
