#ifndef APP_CONTROLAUTHORITY_H
#define APP_CONTROLAUTHORITY_H

#include <QObject>
#include "app/cmdsource.h"

/**
 * weiduodianzi bSyncFlag: panel keys lock only during CDS time-sync.
 * CXTH drops the lock ~2s after the last TIME_SYNC; Clarity holds until disconnect.
 */
class ControlAuthority : public QObject
{
    Q_OBJECT
public:
    explicit ControlAuthority(QObject *parent = nullptr);

    bool isRemote() const { return m_sync; }
    bool isSyncLocked() const { return m_sync; }
    bool allows(int source) const;
    bool allowsPanelKeys() const { return !m_sync; }

    void noteTimeSync();
    void leaveRemote();
    void tickSecond(bool holdUntilDisconnect);

signals:
    void changed();
    void timedOut();

private:
    bool m_sync = false;
    int m_missed = 0;
};

#endif
