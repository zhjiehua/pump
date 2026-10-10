#include "protocol/pc/clarity/claritypcserver.h"
#include "app/cmdinvoke.h"
#include "app/cmdsource.h"
#include "app/pumpcommand.h"
#include "domain/pumpsession.h"
#include "protocol/pc/clarity/claritycodec.h"
#include "protocol/pc/clarity/clarityids.h"
#include "protocol/pc/pcflow.h"
#include "utils/eventlog.h"

ClarityPcServer::ClarityPcServer(QObject *parent)
    : PcServer(parent)
{
}

void ClarityPcServer::sendPressure(double mpa)
{
    const quint32 v = quint32(qAbs(mpa) * 100.0 + 0.5);
    sendBytes(ClarityCodec::encode(m_machineCode, 0, ClarityPc::PFC_SEND_PRESS, v));
}

void ClarityPcServer::handle(const QByteArray &chunk)
{
    m_rx.append(chunk);
    while (true)
    {
        const int s = m_rx.indexOf(char(ClarityPc::STX));
        if (s < 0)
        {
            m_rx.clear();
            return;
        }
        if (s > 0)
            m_rx.remove(0, s);
        if (m_rx.size() < 16)
            return;
        const QByteArray frame = m_rx.left(16);
        m_rx.remove(0, 16);
        handleFrame(frame);
    }
}

void ClarityPcServer::handleFrame(const QByteArray &frame)
{
    quint8 id = 0, ai = 0, pfc = 0;
    quint32 val = 0;
    if (!ClarityCodec::decode(frame, &id, &ai, &pfc, &val) || !m_cmd)
    {
        sendBytes(ClarityCodec::encodeAck(false));
        return;
    }
    if (pfc != ClarityPc::PFC_READ_ID && id != m_machineCode)
        return;

    const PumpSession::Snap snap = m_session ? m_session->copy() : PumpSession::Snap();
    const int remote = CmdSource::Remote;

    if (pfc != ClarityPc::PFC_READ_PRESS
        && pfc != ClarityPc::PFC_SYNCTIME) {
        EventLog_key(QStringLiteral("PC-RX"),
                    QStringLiteral("Clarity %1 id=%2 ai=%3 val=%4")
                        .arg(EventLog::pcClarityPfcName(pfc))
                        .arg(id)
                        .arg(ai)
                        .arg(val));
    }

    switch (pfc)
    {
    case ClarityPc::PFC_READ_ID:
        sendBytes(ClarityCodec::encode(m_machineCode, 0, ClarityPc::PFC_READ_ID, m_machineCode));
        break;
    case ClarityPc::PFC_STATUS:
    {
        quint32 st = quint32(snap.flow * 1000.0 + 0.5) & 0xFFFFF;
        if (snap.stat != 0)
            st |= (1u << 20);
        sendBytes(ClarityCodec::encode(m_machineCode, 0, ClarityPc::PFC_STATUS, st));
        break;
    }
    case ClarityPc::PFC_SET_FLOW:
        CmdInvoke::callBool(m_cmd, "pcApplyFlowCmd",
                            PcFlow::fromArg(snap.pumpType, val, true), snap.percent, remote);
        sendBytes(ClarityCodec::encodeAck(true));
        break;
    case ClarityPc::PFC_SET_PERCENT:
        CmdInvoke::callBool(m_cmd, "setPercentCmd", val / 10.0, remote);
        sendBytes(ClarityCodec::encodeAck(true));
        break;
    case ClarityPc::PFC_MAX_PRESS:
        CmdInvoke::callBool(m_cmd, "setPressLimitsCmd", snap.pressMin, val / 100.0, remote);
        sendBytes(ClarityCodec::encodeAck(true));
        break;
    case ClarityPc::PFC_MIN_PRESS:
        CmdInvoke::callBool(m_cmd, "setPressLimitsCmd", val / 100.0, snap.pressMax, remote);
        sendBytes(ClarityCodec::encodeAck(true));
        break;
    case ClarityPc::PFC_START:
        CmdInvoke::callBool(m_cmd, "pcPumpStartCmd", remote);
        sendBytes(ClarityCodec::encodeAck(true));
        break;
    case ClarityPc::PFC_STOP:
        CmdInvoke::callBool(m_cmd, "stopCmd", remote);
        sendBytes(ClarityCodec::encodeAck(true));
        break;
    case ClarityPc::PFC_PRESSCLEAR:
        CmdInvoke::callBool(m_cmd, "pressZeroCmd", remote);
        sendBytes(ClarityCodec::encodeAck(true));
        break;
    case ClarityPc::PFC_READ_PRESS:
        CmdInvoke::callVoid(m_cmd, "replyPressureToPc");
        sendBytes(ClarityCodec::encodeAck(true));
        break;
    case ClarityPc::PFC_SYNCTIME:
        CmdInvoke::callBoolU8(m_cmd, "pcTimeSyncCmd", int(val), remote);
        sendBytes(ClarityCodec::encodeAck(true));
        break;
    case ClarityPc::PFC_PURGE:
        CmdInvoke::callBool(m_cmd, "purgeCmd", remote);
        sendBytes(ClarityCodec::encodeAck(true));
        break;
    case ClarityPc::PFC_HOLD:
        CmdInvoke::callBool(m_cmd, "pauseCmd", remote);
        sendBytes(ClarityCodec::encodeAck(true));
        break;
    case ClarityPc::PFC_PRESS_COMPEN:
        if (val <= 1)
            CmdInvoke::callBoolU8(m_cmd, "setPressCompenCmd", int(val), remote);
        sendBytes(ClarityCodec::encode(m_machineCode, 0, ClarityPc::PFC_PRESS_COMPEN,
                                       m_session ? m_session->copy().pressCompen : 0));
        break;
    default:
        sendBytes(ClarityCodec::encodeAck(true));
        break;
    }
}
