#include "domain/pumpsession.h"

PumpSession::PumpSession(QObject *parent)
    : QObject(parent)
{
}

PumpSession::Snap PumpSession::copy() const
{
    QMutexLocker lock(&m_mu);
    return m_snap;
}

void PumpSession::update(const Snap &s)
{
    bool press = false;
    bool alarm = false;
    bool tables = false;
    {
        QMutexLocker lock(&m_mu);
        press = s.pressure != m_snap.pressure;
        alarm = s.pressWarnLevel != m_snap.pressWarnLevel || s.linkOk != m_snap.linkOk;
        tables = s.flowTable.size() != m_snap.flowTable.size()
            || s.pressTable.size() != m_snap.pressTable.size()
            || s.pulseTable.size() != m_snap.pulseTable.size()
            || s.pressCompen != m_snap.pressCompen;
        m_snap = s;
    }
    emit snapshotChanged();
    if (press)
        emit pressureChanged();
    if (alarm)
        emit alarmChanged();
    if (tables)
        emit tablesChanged();
}
