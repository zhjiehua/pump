#include "protocol/pc/pcserver.h"
#include "app/pumpcommand.h"
#include "domain/pumpsession.h"
#include "utils/eventlog.h"

PcServer::PcServer(QObject *parent)
    : QObject(parent)
{
    connect(&m_serial, SIGNAL(readyRead()), this, SLOT(onSerial()));
    connect(&m_udp, SIGNAL(readyRead()), this, SLOT(onUdp()));
    connect(&m_tcp, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

void PcServer::setFacade(PumpCommand *cmd, PumpSession *session)
{
    m_cmd = cmd;
    m_session = session;
}

bool PcServer::start(const QVariantMap &cfg)
{
    stop();
    m_machineCode = quint8(cfg.value(QStringLiteral("machineCode")).toInt());
    m_mcuAddress = quint8(cfg.value(QStringLiteral("mcuAddress"), 1).toInt());
    const int portKind = cfg.value(QStringLiteral("pcPort")).toInt();
    QString how;
    switch (portKind)
    {
    case 0: // Serial
    {
        const QString port = cfg.value(QStringLiteral("pcSerialPort")).toString();
        const int baud = cfg.value(QStringLiteral("pcSerialBaud"), 9600).toInt();
        if (!m_serial.open(port, baud))
            return false;
        how = QStringLiteral("started serial %1 @ %2").arg(port).arg(baud);
        break;
    }
    case 2: // TcpServer
    {
        const quint16 localPort = quint16(cfg.value(QStringLiteral("localPort")).toUInt());
        if (!m_tcp.listen(QHostAddress::Any, localPort))
            return false;
        how = QStringLiteral("started TCP server local:%1").arg(localPort);
        break;
    }
    case 1: // Udp
    default:
    {
        const quint16 localPort = quint16(cfg.value(QStringLiteral("localPort")).toUInt());
        if (!m_udp.bind(QHostAddress::Any, localPort))
            return false;
        m_peer = QHostAddress(cfg.value(QStringLiteral("remoteIp")).toString());
        m_peerPort = quint16(cfg.value(QStringLiteral("remotePort")).toUInt());
        how = QStringLiteral("started UDP local:%1 remote:%2:%3")
                  .arg(localPort)
                  .arg(cfg.value(QStringLiteral("remoteIp")).toString())
                  .arg(m_peerPort);
        break;
    }
    }
    m_running = true;
    EventLog_key(QStringLiteral("PC"), how);
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
        EventLog_key(QStringLiteral("PC"), QStringLiteral("stopped"));
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
