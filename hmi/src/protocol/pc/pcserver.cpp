#include "protocol/pc/pcserver.h"
#include "utils/eventlog.h"

PcServer::PcServer(QObject *parent)
    : QObject(parent)
{
    connect(&m_serial, SIGNAL(readyRead()), this, SLOT(onSerial()));
    connect(&m_udp, SIGNAL(readyRead()), this, SLOT(onUdp()));
    connect(&m_tcp, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

bool PcServer::start(AppSettings *s)
{
    stop();
    m_settings = s;
    QString how;
    switch (s->pcPort)
    {
    case AppSettings::Serial:
        if (!m_serial.open(s->pcSerialPort, s->pcSerialBaud))
            return false;
        how = QStringLiteral("started serial %1 @ %2").arg(s->pcSerialPort).arg(s->pcSerialBaud);
        break;
    case AppSettings::TcpServer:
        if (!m_tcp.listen(QHostAddress::Any, s->localPort))
            return false;
        how = QStringLiteral("started TCP server local:%1").arg(s->localPort);
        break;
    case AppSettings::Udp:
    default:
        if (!m_udp.bind(QHostAddress::Any, s->localPort))
            return false;
        m_peer = QHostAddress(s->remoteIp);
        m_peerPort = s->remotePort;
        how = QStringLiteral("started UDP local:%1 remote:%2:%3")
                  .arg(s->localPort)
                  .arg(s->remoteIp)
                  .arg(s->remotePort);
        break;
    }
    m_running = true;
    EventLog::key(QStringLiteral("PC"), how);
    return true;
}

void PcServer::closeTcpClient()
{
    if (!m_tcpClient)
        return;
    m_tcpClient->disconnect(this);
    m_tcpClient->close();
    m_tcpClient->deleteLater();
    m_tcpClient = nullptr;
}

void PcServer::stop()
{
    if (m_running)
        EventLog::key(QStringLiteral("PC"), QStringLiteral("stopped"));
    if (m_serial.isOpen())
        m_serial.close();
    m_udp.close();
    m_tcp.close();
    closeTcpClient();
    m_rx.clear();
    m_running = false;
}

void PcServer::sendBytes(const QByteArray &ba)
{
    if (!m_running)
        return;
    if (m_serial.isOpen())
        m_serial.write(ba);
    else if (m_tcpClient && m_tcpClient->state() == QAbstractSocket::ConnectedState)
        m_tcpClient->write(ba);
    else if (m_peerPort)
        m_udp.writeDatagram(ba, m_peer, m_peerPort);
}

void PcServer::onSerial()
{
    handle(m_serial.readAll());
}

void PcServer::onUdp()
{
    while (m_udp.hasPendingDatagrams())
    {
        QByteArray d;
        d.resize(int(m_udp.pendingDatagramSize()));
        m_udp.readDatagram(d.data(), d.size(), &m_peer, &m_peerPort);
        handle(d);
    }
}

void PcServer::onNewConnection()
{
    QTcpSocket *sock = m_tcp.nextPendingConnection();
    if (!sock)
        return;
    closeTcpClient();
    m_tcpClient = sock;
    connect(m_tcpClient, SIGNAL(readyRead()), this, SLOT(onTcpReadyRead()));
    connect(m_tcpClient, SIGNAL(disconnected()), this, SLOT(onTcpDisconnected()));
}

void PcServer::onTcpReadyRead()
{
    if (m_tcpClient)
        handle(m_tcpClient->readAll());
}

void PcServer::onTcpDisconnected()
{
    closeTcpClient();
}
