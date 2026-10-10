#include "app/controlauthority.h"

ControlAuthority::ControlAuthority(QObject *parent)
    : QObject(parent)
{
}

bool ControlAuthority::allows(int source) const
{
    if (source == CmdSource::Local && m_sync)
        return false;
    return true;
}

void ControlAuthority::noteTimeSync()
{
    m_missed = 0;
    if (m_sync)
        return;
    m_sync = true;
    emit changed();
}

void ControlAuthority::leaveRemote()
{
    m_missed = 0;
    if (!m_sync)
        return;
    m_sync = false;
    emit changed();
}

void ControlAuthority::tickSecond(bool holdUntilDisconnect)
{
    if (!m_sync || holdUntilDisconnect)
        return;
    ++m_missed;
    if (m_missed < 2)
        return;
    m_sync = false;
    m_missed = 0;
    emit changed();
    emit timedOut();
}
