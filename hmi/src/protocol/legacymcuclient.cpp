#include "protocol/legacymcuclient.h"
#include "protocol/legacymcucodec.h"
#include "utils/eventlog.h"

LegacyMcuClient::LegacyMcuClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

bool LegacyMcuClient::open(const QString &port, int baud)
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

void LegacyMcuClient::close()
{
    if (m_port.isOpen())
    {
        EventLog::key(QStringLiteral("MCU"), QStringLiteral("closed"));
        m_port.close();
    }
    m_rx.clear();
    emit connectedChanged(false);
}

bool LegacyMcuClient::isOpen() const
{
    return m_port.isOpen();
}

void LegacyMcuClient::send(const QByteArray &frame)
{
    if (!m_port.isOpen())
        return;
    m_port.write(frame);
}

void LegacyMcuClient::sendCmd(quint8 cmd, quint32 arg)
{
    if (cmd != LegacyMcu::CMD_READ_AU_VAL && cmd != LegacyMcu::CMD_READ_AU_VALB
        && cmd != LegacyMcu::CMD_WAVEADD_MOTOR && cmd != LegacyMcu::CMD_WAVEDEC_MOTOR)
        EventLog::key(QStringLiteral("MCU-TX"),
                      QStringLiteral("%1 arg=%2").arg(EventLog::mcuLegacyCmdName(cmd)).arg(arg));
    send(LegacyMcu::encodeCmd(cmd, arg));
}

void LegacyMcuClient::setFlowWord(quint32 word)
{
    sendCmd(LegacyMcu::CMD_WAVEADD_MOTOR, word);
}

void LegacyMcuClient::stopMotor()
{
    setFlowWord(0);
}

void LegacyMcuClient::pollPressure()
{
    sendCmd(LegacyMcu::CMD_READ_AU_VAL, 0);
}

void LegacyMcuClient::onReadyRead()
{
    const auto frames = LegacyMcu::feed(&m_rx, m_port.readAll());
    for (const auto &f : frames)
    {
        emit cmdEcho(f.cmd, f.arg);
        if (f.cmd == LegacyMcu::CMD_READ_AU_VAL || f.cmd == LegacyMcu::CMD_READ_AU_VALB)
            emit pressureRaw(f.arg);
        else
            EventLog::key(QStringLiteral("MCU-RX"),
                          QStringLiteral("%1 arg=%2").arg(EventLog::mcuLegacyCmdName(f.cmd)).arg(f.arg));
    }
}
