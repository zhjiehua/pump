#include "protocol/qinfineclient.h"
#include "utils/eventlog.h"

QinFineClient::QinFineClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

bool QinFineClient::open(const QString &port, int baud, quint8 addr)
{
    close();
    m_addr = addr;
    if (!m_port.open(port, baud))
    {
        EventLog::key(QStringLiteral("MCU"),
                      QStringLiteral("QinFine open failed %1: %2").arg(port, m_port.errorString()));
        emit errorText(m_port.errorString());
        emit connectedChanged(false);
        return false;
    }
    EventLog::key(QStringLiteral("MCU"),
                  QStringLiteral("QinFine opened %1 @ %2 addr=%3").arg(port).arg(baud).arg(addr));
    emit connectedChanged(true);
    return true;
}

void QinFineClient::close()
{
    if (m_port.isOpen())
    {
        EventLog::key(QStringLiteral("MCU"), QStringLiteral("QinFine closed"));
        m_port.close();
    }
    m_rx.clear();
    emit connectedChanged(false);
}

bool QinFineClient::isOpen() const
{
    return m_port.isOpen();
}

void QinFineClient::send(const QByteArray &frame)
{
    if (!m_port.isOpen())
        return;
    m_port.write(frame);
}

void QinFineClient::setFlow(float mlMin)
{
    send(QinFine::encodeFloat(m_addr, QinFine::PFC_FLOW, mlMin));
}

void QinFineClient::setPercent(quint8 percent)
{
    send(QinFine::encodeU8(m_addr, QinFine::PFC_PERCENT, percent));
}

void QinFineClient::setPressMin(float mpa)
{
    send(QinFine::encodeFloat(m_addr, QinFine::PFC_PMIN, mpa));
}

void QinFineClient::setPressMax(float mpa)
{
    send(QinFine::encodeFloat(m_addr, QinFine::PFC_PMAX, mpa));
}

void QinFineClient::setStartStop(bool run)
{
    send(QinFine::encodeU8(m_addr, QinFine::PFC_START_STOP, run ? 1 : 0));
}

void QinFineClient::setPause(bool pause)
{
    send(QinFine::encodeU8(m_addr, QinFine::PFC_PAUSE, pause ? 1 : 0));
}

void QinFineClient::setPurge(bool on)
{
    send(QinFine::encodeU8(m_addr, QinFine::PFC_PURGE, on ? 1 : 0));
}

void QinFineClient::pressZero()
{
    send(QinFine::encodeU8(m_addr, QinFine::PFC_PRESS_ZERO, 1));
}

void QinFineClient::setPressCompen(quint8 on)
{
    send(QinFine::encodeU8(m_addr, QinFine::PFC_PRESS_COMPEN, on));
}

void QinFineClient::getPressure()
{
    send(QinFine::encodeGet(m_addr, QinFine::PFC_PRESS));
}

void QinFineClient::setWorkMode(quint8 mode, quint8 flag)
{
    send(QinFine::encodeExtU8U8(m_addr, QinFine::PES_WORKMODE, mode, flag));
}

void QinFineClient::setTableCmd(quint8 subCmd, quint8 cmd)
{
    send(QinFine::encodeExtU8(m_addr, subCmd, cmd));
}

void QinFineClient::setTablePoint(quint8 subData, float a, float b)
{
    send(QinFine::encodeExt2Float(m_addr, subData, a, b));
}

void QinFineClient::getTable(quint8 subData)
{
    send(QinFine::encodeExtGet(m_addr, subData));
}

void QinFineClient::setLoadFloat(quint8 sub, float v)
{
    send(QinFine::encodeExtFloat(m_addr, sub, v));
}

void QinFineClient::getLoadFloat(quint8 sub)
{
    send(QinFine::encodeExtGet(m_addr, sub));
}

void QinFineClient::tick()
{
    send(QinFine::encodeU8(m_addr, QinFine::PFC_TICK, 1));
}

void QinFineClient::onReadyRead()
{
    m_rx.append(m_port.readAll());
    while (!m_rx.isEmpty())
    {
        if (m_rx.at(0) == QinFine::kAck)
        {
            emit ackReceived(true);
            m_rx.remove(0, 1);
            continue;
        }
        if (m_rx.at(0) == QinFine::kNack)
        {
            EventLog::key(QStringLiteral("MCU-RX"), QStringLiteral("QinFine NACK"));
            emit ackReceived(false);
            m_rx.remove(0, 1);
            continue;
        }
        const int head = m_rx.indexOf(QinFine::kHead);
        if (head < 0)
        {
            m_rx.clear();
            return;
        }
        if (head > 0)
            m_rx.remove(0, head);
        const int tail = m_rx.indexOf(QinFine::kTail);
        if (tail < 0)
            return;
        const QByteArray inner = m_rx.mid(1, tail - 1);
        m_rx.remove(0, tail + 1);
        handleFrame(QinFine::decode(inner));
    }
}

void QinFineClient::handleFrame(const QinFine::Frame &f)
{
    if (!f.ok)
        return;
    const quint8 pfc = f.pfc & 0x7F;
    if (pfc == QinFine::PFC_PRESS && f.data.size() >= 4)
    {
        emit pressureUpdated(QinFine::beFloat(f.data));
        return;
    }
    if (pfc == QinFine::PFC_EXT_SYSTEM && !f.data.isEmpty())
    {
        const quint8 sub = quint8(f.data.at(0));
        if (f.data.size() >= 9)
            emit extPoint(sub, QinFine::beFloat(f.data, 1), QinFine::beFloat(f.data, 5));
        else if (f.data.size() >= 5)
            emit extFloat(sub, QinFine::beFloat(f.data, 1));
        else if (f.data.size() >= 2)
            emit extU8(sub, quint8(f.data.at(1)));
    }
}
