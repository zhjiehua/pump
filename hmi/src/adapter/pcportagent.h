#ifndef ADAPTER_PCPORTAGENT_H
#define ADAPTER_PCPORTAGENT_H

#include <QByteArray>
#include <QObject>
#include <QVariantMap>

class PcServer;
class QinFinePcServer;
class PumpCommand;
class PumpSession;

/** Lives on the IO thread. Owns CDS transports; commands hop to GUI PumpCommand. */
class PcPortAgent : public QObject
{
    Q_OBJECT
public:
    explicit PcPortAgent(QObject *parent = nullptr);

public slots:
    void init();
    void shutdown();
    void setFacade(QObject *cmd, QObject *session);
    bool startLink(const QVariantMap &cfg);
    void stopLink();
    void sendPressure(double mpa);
    void sendBytes(const QByteArray &ba);
    void dumpFlowTable();
    void dumpPressTable();
    void dumpPulseTable();

private:
    PcServer *active() const;

    PcServer *m_cxth = nullptr;
    PcServer *m_clarity = nullptr;
    QinFinePcServer *m_qinFine = nullptr;
    int m_protocol = 0;
};

#endif
