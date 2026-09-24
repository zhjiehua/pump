#ifndef CXTHPCSERVER_H
#define CXTHPCSERVER_H

#include <QObject>
#include "platform/hmiserialport.h"
#include <QUdpSocket>
#include <QHostAddress>
#include "core/appsettings.h"

class MachineController;

class CxthPcServer : public QObject
{
    Q_OBJECT
public:
    explicit CxthPcServer(QObject *parent = nullptr);
    void setController(MachineController *c) { m_ctrl = c; }

    bool start(AppSettings *s);
    void stop();
    void sendPressure(double mpa);
    void sendBytes(const QByteArray &ba);
    void sendCalib(quint8 ai, quint32 value);
    void dumpFlowTable();
    void dumpPressTable();
    void dumpPulseTable();

private slots:
    void onSerial();
    void onUdp();

private:
    void handle(const QByteArray &chunk);
    void handleClarity(const QByteArray &frame);
    void handleLegacy(const QByteArray &frame);

    MachineController *m_ctrl = nullptr;
    AppSettings *m_settings = nullptr;
    HmiSerialPort m_serial;
    QUdpSocket m_udp;
    QByteArray m_rx;
    QHostAddress m_peer;
    quint16 m_peerPort = 0;
    bool m_running = false;
};

#endif
