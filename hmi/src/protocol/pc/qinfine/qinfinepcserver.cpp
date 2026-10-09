#include "protocol/pc/qinfine/qinfinepcserver.h"
#include "app/cmdinvoke.h"
#include "app/cmdsource.h"
#include "app/pumpcommand.h"
#include "domain/pumpsession.h"
#include "utils/eventlog.h"

QinFinePcServer::QinFinePcServer(QObject *parent)
    : PcServer(parent)
{
}

quint8 QinFinePcServer::addr() const
{
    return m_mcuAddress;
}

void QinFinePcServer::ack(bool ok)
{
    sendBytes(QByteArray(1, char(ok ? QinFine::kAck : QinFine::kNack)));
}

void QinFinePcServer::sendFloat(quint8 pfc, float v)
{
    sendBytes(QinFine::encode(addr(), pfc, QinFine::floatBytes(v)));
}

void QinFinePcServer::sendU8(quint8 pfc, quint8 v)
{
    sendBytes(QinFine::encode(addr(), pfc, QByteArray(1, char(v))));
}

void QinFinePcServer::sendExtFloat(quint8 sub, float v)
{
    QByteArray p;
    p.append(char(sub));
    p.append(QinFine::floatBytes(v));
    sendBytes(QinFine::encode(addr(), QinFine::PFC_EXT_SYSTEM, p));
}

void QinFinePcServer::sendExtPoint(quint8 sub, float a, float b)
{
    QByteArray p;
    p.append(char(sub));
    p.append(QinFine::floatBytes(a));
    p.append(QinFine::floatBytes(b));
    sendBytes(QinFine::encode(addr(), QinFine::PFC_EXT_SYSTEM, p));
}

void QinFinePcServer::sendPressure(double mpa)
{
    sendFloat(QinFine::PFC_PRESS, float(mpa));
}

void QinFinePcServer::sendLoadFloat(quint8 sub, float v)
{
    sendExtFloat(sub, v);
}

void QinFinePcServer::dumpFlowTable()
{
    const PumpSession::Snap snap = m_session ? m_session->copy() : PumpSession::Snap();
    for (const auto &p : snap.flowTable)
        sendExtPoint(QinFine::PES_FLOW_DATA, float(p.rpm), float(p.rate));
    sendExtFloat(QinFine::PES_LOAD_FLOW, float(snap.loadRate));
    sendExtFloat(QinFine::PES_LOAD_REAL, float(snap.loadReal));
    sendExtFloat(QinFine::PES_LOAD_PRESS, float(snap.loadPress));
}

void QinFinePcServer::dumpPressTable()
{
    const PumpSession::Snap snap = m_session ? m_session->copy() : PumpSession::Snap();
    for (const auto &p : snap.pressTable)
        sendExtPoint(QinFine::PES_PRESS_DATA, float(p.adc), float(p.pressure));
}

void QinFinePcServer::dumpPulseTable()
{
    const PumpSession::Snap snap = m_session ? m_session->copy() : PumpSession::Snap();
    for (const auto &p : snap.pulseTable)
        sendExtPoint(QinFine::PES_PULSE_DATA, float(p.position), float(p.factor));
}

void QinFinePcServer::handle(const QByteArray &chunk)
{
    m_rx.append(chunk);
    while (!m_rx.isEmpty())
    {
        if (m_rx.at(0) == QinFine::kAck || m_rx.at(0) == QinFine::kNack)
        {
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

void QinFinePcServer::handleFrame(const QinFine::Frame &f)
{
    if (!f.ok || !m_cmd)
    {
        ack(false);
        return;
    }
    if (f.addr != addr())
        return;
    const bool isSet = (f.pfc & QinFine::kSetBit) != 0;
    handlePfc(quint8(f.pfc & 0x7F), isSet, f.data);
}

void QinFinePcServer::handlePfc(quint8 pfc, bool isSet, const QByteArray &data)
{
    EventLog::key(QStringLiteral("PC-RX"),
                  QStringLiteral("QinFine pfc=0x%1 set=%2 len=%3")
                      .arg(pfc, 2, 16, QLatin1Char('0'))
                      .arg(isSet)
                      .arg(data.size()));

    const PumpSession::Snap snap = m_session ? m_session->copy() : PumpSession::Snap();
    const int remote = CmdSource::Remote;

    switch (pfc)
    {
    case QinFine::PFC_FLOW:
        if (isSet)
        {
            CmdInvoke::callBool(m_cmd, "enterPcControlCmd", remote);
            CmdInvoke::callBool(m_cmd, "setFlowCmd", double(QinFine::beFloat(data)), remote);
            ack(true);
        }
        else
            sendFloat(QinFine::PFC_FLOW, float(snap.flow));
        break;
    case QinFine::PFC_PERCENT:
        if (isSet)
        {
            CmdInvoke::callBool(m_cmd, "setPercentCmd", double(QinFine::beU8(data)), remote);
            ack(true);
        }
        else
            sendU8(QinFine::PFC_PERCENT, quint8(snap.percent));
        break;
    case QinFine::PFC_PMIN:
        if (isSet)
        {
            CmdInvoke::callBool(m_cmd, "setPressLimitsCmd", double(QinFine::beFloat(data)),
                                snap.pressMax, remote);
            ack(true);
        }
        else
            sendFloat(QinFine::PFC_PMIN, float(snap.pressMin));
        break;
    case QinFine::PFC_PMAX:
        if (isSet)
        {
            CmdInvoke::callBool(m_cmd, "setPressLimitsCmd", snap.pressMin,
                                double(QinFine::beFloat(data)), remote);
            ack(true);
        }
        else
            sendFloat(QinFine::PFC_PMAX, float(snap.pressMax));
        break;
    case QinFine::PFC_START_STOP:
        if (isSet)
        {
            if (QinFine::beU8(data))
                CmdInvoke::callBool(m_cmd, "startCmd", remote);
            else
                CmdInvoke::callBool(m_cmd, "stopCmd", remote);
            ack(true);
        }
        else
            sendU8(QinFine::PFC_START_STOP, snap.stat == 0 ? 0 : 1);
        break;
    case QinFine::PFC_PAUSE:
        if (isSet)
        {
            CmdInvoke::callBool(m_cmd, "pauseCmd", remote);
            ack(true);
        }
        break;
    case QinFine::PFC_PURGE:
        if (isSet)
        {
            CmdInvoke::callBool(m_cmd, "purgeCmd", remote);
            ack(true);
        }
        break;
    case QinFine::PFC_PRESS_ZERO:
        if (isSet)
        {
            CmdInvoke::callBool(m_cmd, "pressZeroCmd", remote);
            ack(true);
        }
        break;
    case QinFine::PFC_PRESS_COMPEN:
        if (isSet)
        {
            CmdInvoke::callBoolU8(m_cmd, "setPressCompenCmd", int(QinFine::beU8(data)), remote);
            ack(true);
        }
        else
            sendU8(QinFine::PFC_PRESS_COMPEN, snap.pressCompen);
        break;
    case QinFine::PFC_PRESS:
        if (isSet)
            ack(true);
        else
            sendPressure(snap.pressure);
        break;
    case QinFine::PFC_TICK:
        ack(true);
        break;
    case QinFine::PFC_EXT_SYSTEM:
        handleExt(isSet, data);
        break;
    default:
        if (isSet)
            ack(true);
        break;
    }
}

void QinFinePcServer::handleExt(bool isSet, const QByteArray &data)
{
    if (data.isEmpty())
    {
        ack(false);
        return;
    }
    const quint8 sub = quint8(data.at(0));
    const QByteArray payload = data.mid(1);
    if (isSet)
    {
        CmdInvoke::callVoid(m_cmd, "applyQinFineExtSet", int(sub), payload);
        ack(true);
        return;
    }
    const PumpSession::Snap snap = m_session ? m_session->copy() : PumpSession::Snap();
    switch (sub)
    {
    case QinFine::PES_FLOW_CMD:
    case QinFine::PES_FLOW_DATA:
        CmdInvoke::callVoid(m_cmd, "requestQinFineDump", 1);
        break;
    case QinFine::PES_PRESS_CMD:
    case QinFine::PES_PRESS_DATA:
        CmdInvoke::callVoid(m_cmd, "requestQinFineDump", 2);
        break;
    case QinFine::PES_PULSE_CMD:
    case QinFine::PES_PULSE_DATA:
        CmdInvoke::callVoid(m_cmd, "requestQinFineDump", 3);
        break;
    case QinFine::PES_LOAD_FLOW:
        sendLoadFloat(sub, float(snap.loadRate));
        break;
    case QinFine::PES_LOAD_REAL:
        sendLoadFloat(sub, float(snap.loadReal));
        break;
    case QinFine::PES_LOAD_PRESS:
        sendLoadFloat(sub, float(snap.loadPress));
        break;
    case QinFine::PES_WORKMODE:
        break;
    default:
        break;
    }
}
