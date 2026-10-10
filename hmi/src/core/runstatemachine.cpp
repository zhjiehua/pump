#include "core/runstatemachine.h"

RunStateMachine::RunStateMachine(QObject *parent)
    : QObject(parent)
{
}

void RunStateMachine::setStat(Stat s)
{
    if (m_stat == s)
        return;
    m_stat = s;
    if (s == Stop)
        m_runSec = 0;
    emit statChanged(m_stat);
    emit runTimeChanged(m_runSec);
}

void RunStateMachine::setRunSeconds(quint32 sec)
{
    if (m_runSec == sec)
        return;
    m_runSec = sec;
    emit runTimeChanged(m_runSec);
}

void RunStateMachine::tickSecond()
{
    if (m_stat == Stop || m_stat == Pause || m_stat == PcCtrl)
        return;
    ++m_runSec;
    emit runTimeChanged(m_runSec);
}

bool RunStateMachine::isPumpRunning() const
{
    return m_stat == Running || m_stat == Pump || m_stat == Purge || m_stat == PcCtrl;
}
