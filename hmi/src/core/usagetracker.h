#ifndef USAGETRACKER_H
#define USAGETRACKER_H

#include <QObject>

class AppSettings;

/** GLP usage counters (sysUsedSec / pumpUsedSec). */
class UsageTracker : public QObject
{
    Q_OBJECT
public:
    explicit UsageTracker(AppSettings *settings, QObject *parent = nullptr);

    void tick(bool pumpRunning);
    void clearSystemTime();
    void clearPumpTime();

signals:
    void usageChanged();

private:
    AppSettings *m_settings = nullptr;
    int m_saveCounter = 0;
};

#endif
