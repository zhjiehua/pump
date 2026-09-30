#ifndef PROTOCOL_PC_SERVER_H
#define PROTOCOL_PC_SERVER_H

#include <QObject>
#include "platform/hmiserialport.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include "core/appsettings.h"

class MachineController;

class PcServer : public QObject
{
    Q_OBJECT
public:
    explicit PcServer(QObject *parent = nullptr);
    void setController(MachineController *c) { m_ctrl = c; }

    bool start(AppSettings *s);
    void stop();
    void sendBytes(const QByteArray &ba);
    virtual void sendPressure(double mpa) = 0;

protected slots:
    void onSerial();
    void onUdp();
    void onNewConnection();
    void onTcpReadyRead();
    void onTcpDisconnected();

protected:
    virtual void handle(const QByteArray &chunk) = 0;

    MachineController *m_ctrl = nullptr;
    AppSettings *m_settings = nullptr;
    QByteArray m_rx;

private:
    void closeTcpClient();

    HmiSerialPort m_serial;
    QUdpSocket m_udp;
    QTcpServer m_tcp;
    QTcpSocket *m_tcpClient = nullptr;
    QHostAddress m_peer;
    quint16 m_peerPort = 0;
    bool m_running = false;
};

#endif
