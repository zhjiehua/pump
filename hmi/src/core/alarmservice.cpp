#include "core/alarmservice.h"
#include "utils/eventlog.h"

AlarmService::AlarmService(QObject *parent)
    : QObject(parent)
{
}

void AlarmService::setAlarm(Kind kind, bool active)
{
    const int id = int(kind);
    if (active)
    {
        if (m_active.contains(id))
            return;
        m_active.insert(id);
        EventLog_key(QStringLiteral("ALARM"),
                      QStringLiteral("raised %1").arg(EventLog::alarmName(id)));
    }
    else
    {
        if (!m_active.remove(id))
            return;
        if (kind == OverpressErr)
            m_pressWarn = 0;
        EventLog_key(QStringLiteral("ALARM"),
                      QStringLiteral("cleared %1").arg(EventLog::alarmName(id)));
    }
    emit alarmChanged();
}

void AlarmService::clearAll()
{
    if (m_active.isEmpty() && m_pressWarn == 0)
        return;
    m_active.clear();
    m_pressWarn = 0;
    m_overPress = 0;
    EventLog_key(QStringLiteral("ALARM"), QStringLiteral("cleared all"));
    emit alarmChanged();
}

AlarmService::Kind AlarmService::primaryAlarm() const
{
    if (m_active.isEmpty())
        return NoWarn;
    if (m_active.contains(int(CommunicationErr)))
        return CommunicationErr;
    if (m_active.contains(int(OverpressErr)))
        return OverpressErr;
    if (m_active.contains(int(Weeping)))
        return Weeping;
    return Kind(*m_active.constBegin());
}

int AlarmService::pressWarnLevel() const
{
    return m_pressWarn;
}

void AlarmService::checkPressure(double mpa, quint32 runSec, double pmin, double pmax)
{
    int warn = 0;
    if (runSec > 60)
    {
        if (mpa > pmax)
            warn = 2;
        else if (mpa < pmin)
            warn = 1;
    }
    else if (mpa > pmax)
    {
        warn = 2;
    }

    if (warn != 0)
    {
        m_overPress = mpa;
        m_pressWarn = warn;
        setAlarm(OverpressErr, true);
    }
}
