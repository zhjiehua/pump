#include "core/usagetracker.h"
#include "core/appsettings.h"

UsageTracker::UsageTracker(AppSettings *settings, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
{
}

void UsageTracker::tick(bool pumpRunning)
{
    if (!m_settings)
        return;
    ++m_settings->sysUsedSec;
    if (pumpRunning)
        ++m_settings->pumpUsedSec;
    if (++m_saveCounter >= 60)
    {
        m_saveCounter = 0;
        m_settings->save();
        emit usageChanged();
    }
}

void UsageTracker::clearSystemTime()
{
    if (!m_settings)
        return;
    m_settings->sysUsedSec = 0;
    m_settings->save();
    emit usageChanged();
}

void UsageTracker::clearPumpTime()
{
    if (!m_settings)
        return;
    m_settings->pumpUsedSec = 0;
    m_settings->save();
    emit usageChanged();
}
