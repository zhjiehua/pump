#include "protocol/mcu/cxth/cxthmcuclient.h"
#include "protocol/mcu/cxth/cxthmcucodec.h"
#include "utils/eventlog.h"

CxthMcuClient::CxthMcuClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

bool CxthMcuClient::open(const QString &port, int baud)
{
    close();
    if (!m_port.open(port, baud))
    {
        EventLog::key(QStringLiteral("MCU"),
                      QStringLiteral("open failed %1: %2").arg(port, m_port.errorString()));
        emit errorText(m_port.errorString());
        emit connectedChanged(false);
        return false;
    }
    EventLog::key(QStringLiteral("MCU"), QStringLiteral("opened %1 @ %2").arg(port).arg(baud));
    emit connectedChanged(true);
    return true;
}

void CxthMcuClient::close()
{
    if (m_port.isOpen())
    {
        EventLog::key(QStringLiteral("MCU"), QStringLiteral("closed"));
        m_port.close();
    }
    m_rx.clear();
    emit connectedChanged(false);
}

bool CxthMcuClient::isOpen() const
{
    return m_port.isOpen();
}

void CxthMcuClient::send(const QByteArray &frame)
{
    if (!m_port.isOpen())
        return;
    m_port.write(frame);
}

void CxthMcuClient::sendCmd(quint8 cmd, quint32 arg)
{
    if (cmd != CxthMcu::CMD_READ_AU_VAL && cmd != CxthMcu::CMD_READ_AU_VALB
        && cmd != CxthMcu::CMD_WAVEADD_MOTOR && cmd != CxthMcu::CMD_WAVEDEC_MOTOR)
        EventLog::key(QStringLiteral("MCU-TX"),
                      QStringLiteral("%1 arg=%2").arg(EventLog::mcuCxthCmdName(cmd)).arg(arg));
    send(CxthMcu::encodeCmd(cmd, arg));
}

void CxthMcuClient::setFlowWord(quint32 word)
{
    sendCmd(CxthMcu::CMD_WAVEADD_MOTOR, word);
}

void CxthMcuClient::stopMotor()
{
    setFlowWord(0);
}

void CxthMcuClient::pollPressure()
{
    sendCmd(CxthMcu::CMD_READ_AU_VAL, 0);
}

void CxthMcuClient::onReadyRead()
{
    const auto frames = CxthMcu::feed(&m_rx, m_port.readAll());
    for (const auto &f : frames)
    {
        emit cmdEcho(f.cmd, f.arg);
        if (f.cmd == CxthMcu::CMD_READ_AU_VAL || f.cmd == CxthMcu::CMD_READ_AU_VALB)
            emit pressureRaw(f.arg);
        else
            EventLog::key(QStringLiteral("MCU-RX"),
                          QStringLiteral("%1 arg=%2").arg(EventLog::mcuCxthCmdName(f.cmd)).arg(f.arg));
    }
}
