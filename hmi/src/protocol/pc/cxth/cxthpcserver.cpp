#include "protocol/pc/cxth/cxthpcserver.h"
#include "app/cmdinvoke.h"
#include "app/cmdsource.h"
#include "app/pumpcommand.h"
#include "domain/pumpsession.h"
#include "protocol/pc/cxth/cxthpccodec.h"
#include "protocol/pc/cxth/cxthpcids.h"
#include "protocol/pc/pcflow.h"
#include "utils/eventlog.h"

CxthPcServer::CxthPcServer(QObject *parent)
    : PcServer(parent)
{
}

void CxthPcServer::sendPressure(double mpa)
{
    const quint32 v = quint32(qAbs(mpa) * 100.0 + 0.5);
    sendBytes(CxthPcCodec::encode(CxthPc::PFC_READ_PRESS, v));
}

void CxthPcServer::handle(const QByteArray &chunk)
{
    m_rx.append(chunk);
    while (m_rx.size() >= 4)
    {
        const int n = m_rx.size() >= 5 ? 5 : 4;
        const QByteArray frame = m_rx.left(n);
        m_rx.remove(0, n);
        handleFrame(frame);
    }
}

void CxthPcServer::handleFrame(const QByteArray &frame)
{
    quint8 cmd = 0, add = 0;
    quint32 arg = 0;
    if (!CxthPcCodec::decode(frame, &cmd, &arg, &add) || !m_cmd)
        return;

    if (cmd != CxthPc::PFC_READ_PRESS
        && cmd != CxthPc::PFC_TIME_SYNC) {
        EventLog_key(QStringLiteral("PC-RX"),
                    QStringLiteral("CXTH %1 arg=%2 add=%3")
                        .arg(EventLog::pcCxthCmdName(cmd))
                        .arg(arg)
                        .arg(add));
    }

    const PumpSession::Snap snap = m_session ? m_session->copy() : PumpSession::Snap();
    const int remote = CmdSource::Remote;
    switch (cmd)
    {
    case CxthPc::PFC_SET_FLOW1:
        CmdInvoke::callBool(m_cmd, "pcApplyFlowCmd",
                            PcFlow::fromArg(snap.pumpType, arg, false),
                            PcFlow::cxthPercent(add), remote);
        break;
    case CxthPc::PFC_SET_MAXPRESS:
        CmdInvoke::callBool(m_cmd, "setPressLimitsCmd", snap.pressMin, arg / 100.0, remote);
        break;
    case CxthPc::PFC_SET_MINPRESS:
        CmdInvoke::callBool(m_cmd, "setPressLimitsCmd", arg / 100.0, snap.pressMax, remote);
        break;
    case CxthPc::PFC_START:
        CmdInvoke::callBool(m_cmd, "pcPumpStartCmd", remote);
        break;
    case CxthPc::PFC_STOP:
        CmdInvoke::callBool(m_cmd, "stopCmd", remote);
        break;
    case CxthPc::PFC_PURGE:
        CmdInvoke::callBool(m_cmd, "purgeCmd", remote);
        break;
    case CxthPc::PFC_HOLD:
        CmdInvoke::callBool(m_cmd, "pauseCmd", remote);
        break;
    case CxthPc::PFC_READ_PRESS:
        CmdInvoke::callVoid(m_cmd, "replyPressureToPc");
        break;
    case CxthPc::PFC_TIME_SYNC:
        CmdInvoke::callBoolU8(m_cmd, "pcTimeSyncCmd", int(arg), remote);
        break;
    default:
        break;
    }
}
