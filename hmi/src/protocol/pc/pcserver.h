#ifndef PROTOCOL_PC_SERVER_H
#define PROTOCOL_PC_SERVER_H

#include <QObject>
#include "platform/hmiserialport.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QVariantMap>

class PumpCommand;
class PumpSession;

class PcServer : public QObject
{
    Q_OBJECT
public:
    explicit PcServer(QObject *parent = nullptr);
    void setFacade(PumpCommand *cmd, PumpSession *session);

    bool start(const QVariantMap &cfg);
    void stop();
    void sendBytes(const QByteArray &ba);
    virtual void sendPressure(double mpa) = 0;

    quint8 machineCode() const { return m_machineCode; }
    quint8 mcuAddress() const { return m_mcuAddress; }

protected slots:
    void onSerial();
    void onUdp();
    void onNewConnection();
    void onTcpReadyRead();
    void onTcpDisconnected();

protected:
    virtual void handle(const QByteArray &chunk) = 0;

    PumpCommand *m_cmd = nullptr;
    PumpSession *m_session = nullptr;
    QByteArray m_rx;
    quint8 m_machineCode = 0x12;
    quint8 m_mcuAddress = 0x01;

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
