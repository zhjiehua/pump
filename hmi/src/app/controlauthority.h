#ifndef APP_CONTROLAUTHORITY_H
#define APP_CONTROLAUTHORITY_H

#include <QObject>
#include "app/cmdsource.h"

/** Local panel vs CDS remote gate (aligned with MCU App_AllowsLocal/RemoteControl). */
class ControlAuthority : public QObject
{
    Q_OBJECT
public:
    explicit ControlAuthority(QObject *parent = nullptr);

    bool isRemote() const { return m_remote; }
    bool allows(int source) const;
    /** First Remote command while local takes over. */
    void noteRemoteCommand();
    void leaveRemote();

signals:
    void changed();

private:
    bool m_remote = false;
};

#endif
