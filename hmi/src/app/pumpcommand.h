#ifndef APP_PUMPCOMMAND_H
#define APP_PUMPCOMMAND_H

#include <QObject>
#include "app/cmdsource.h"

/**
 * Narrow command facade. UI uses Local (or the no-arg wrappers);
 * CDS servers always pass Remote via the *Cmd slots.
 */
class PumpCommand : public QObject
{
    Q_OBJECT
public:
    explicit PumpCommand(QObject *parent = nullptr);

    bool start() { return startCmd(CmdSource::Local); }
    bool stop() { return stopCmd(CmdSource::Local); }
    bool pause() { return pauseCmd(CmdSource::Local); }
    bool purge() { return purgeCmd(CmdSource::Local); }
    bool pump() { return pumpCmd(CmdSource::Local); }
    void setFlow(double mlMin) { setFlowCmd(mlMin, CmdSource::Local); }
    void enterPcControl() { enterPcControlCmd(CmdSource::Local); }

public slots:
    virtual bool startCmd(int source) = 0;
    virtual bool stopCmd(int source) = 0;
    virtual bool pauseCmd(int source) = 0;
    virtual bool purgeCmd(int source) = 0;
    virtual bool pumpCmd(int source) = 0;
    virtual bool setFlowCmd(double mlMin, int source) = 0;
    virtual bool setPercentCmd(double percent, int source) = 0;
    virtual bool setPressLimitsCmd(double pmin, double pmax, int source) = 0;
    virtual bool pressZeroCmd(int source) = 0;
    virtual bool enterPcControlCmd(int source) = 0;
    virtual bool setPressCompenCmd(int on, int source) = 0;

signals:
    void commandRejected(const QString &reason);
};

#endif
