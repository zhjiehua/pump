#include "app/controlauthority.h"

ControlAuthority::ControlAuthority(QObject *parent)
    : QObject(parent)
{
}

bool ControlAuthority::allows(int source) const
{
    if (m_remote)
        return source == CmdSource::Remote;
    return true;
}

void ControlAuthority::noteRemoteCommand()
{
    if (m_remote)
        return;
    m_remote = true;
    emit changed();
}

void ControlAuthority::leaveRemote()
{
    if (!m_remote)
        return;
    m_remote = false;
    emit changed();
}
