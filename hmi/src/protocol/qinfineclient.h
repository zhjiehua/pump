#ifndef QINFINECLIENT_H
#define QINFINECLIENT_H

#include <QObject>
#include "platform/hmiserialport.h"
#include "protocol/qinfinecodec.h"

class QinFineClient : public QObject
{
    Q_OBJECT
public:
    explicit QinFineClient(QObject *parent = nullptr);

    bool open(const QString &port, int baud, quint8 addr);
    void close();
    bool isOpen() const;

    quint8 address() const { return m_addr; }

    void setFlow(float mlMin);
    void setPercent(quint8 percent);
    void setPressMin(float mpa);
    void setPressMax(float mpa);
    void setStartStop(bool run);
    void setPause(bool pause);
    void setPurge(bool on);
    void pressZero();
    void setPressCompen(quint8 on);
    void getPressure();
    void setWorkMode(quint8 mode, quint8 flag);
    void setTableCmd(quint8 subCmd, quint8 cmd);
    void setTablePoint(quint8 subData, float a, float b);
    void getTable(quint8 subData);
    void setLoadFloat(quint8 sub, float v);
    void getLoadFloat(quint8 sub);
    void tick();

signals:
    void connectedChanged(bool on);
    void pressureUpdated(float mpa);
    void ackReceived(bool ok);
    void extPoint(quint8 sub, float a, float b);
    void extU8(quint8 sub, quint8 v);
    void extFloat(quint8 sub, float v);
    void errorText(const QString &text);

private slots:
    void onReadyRead();

private:
    void send(const QByteArray &frame);
    void handleFrame(const QinFine::Frame &f);

    HmiSerialPort m_port;
    quint8 m_addr = 0x01;
    QByteArray m_rx;
};

#endif
