#ifndef RUNSTATEMACHINE_H
#define RUNSTATEMACHINE_H

#include <QObject>

/** Tracks run elapsed time and PC-control flag. */
class RunStateMachine : public QObject
{
    Q_OBJECT
public:
    enum Stat { Stop = 0, Pause, Running, Pump, Purge, PcCtrl };

    explicit RunStateMachine(QObject *parent = nullptr);

    Stat stat() const { return m_stat; }
    void setStat(Stat s);
    quint32 runSeconds() const { return m_runSec; }
    void tickSecond();
    bool isPumpRunning() const;

signals:
    void statChanged(Stat s);
    void runTimeChanged(quint32 sec);

private:
    Stat m_stat = Stop;
    quint32 m_runSec = 0;
};

#endif
