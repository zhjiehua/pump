#include "protocol/pc/cxth/cxthpcserver.h"
#include "core/machinecontroller.h"
#include "protocol/pc/cxth/cxthpccodec.h"
#include "protocol/pc/cxth/cxthpcids.h"
#include "utils/eventlog.h"

CxthPcServer::CxthPcServer(QObject *parent)
    : PcServer(parent)
{
}

void CxthPcServer::sendPressure(double mpa)
{
    if (!m_settings || !m_ctrl)
        return;
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
    if (!CxthPcCodec::decode(frame, &cmd, &arg, &add) || !m_ctrl)
        return;
    EventLog::key(QStringLiteral("PC-RX"),
                  QStringLiteral("CXTH %1 arg=%2 add=%3")
                      .arg(EventLog::pcCxthCmdName(cmd))
                      .arg(arg)
                      .arg(add));
    switch (cmd)
    {
    case CxthPc::PFC_SET_FLOW1:
        m_ctrl->enterPcControl();
        m_ctrl->setPercent(add);
        m_ctrl->setFlow(arg / 1000.0);
        break;
    case CxthPc::PFC_SET_MAXPRESS:
        m_ctrl->setPressLimits(m_ctrl->settings()->pressMin, arg / 100.0);
        break;
    case CxthPc::PFC_SET_MINPRESS:
        m_ctrl->setPressLimits(arg / 100.0, m_ctrl->settings()->pressMax);
        break;
    case CxthPc::PFC_START:
        m_ctrl->start();
        break;
    case CxthPc::PFC_STOP:
        m_ctrl->stop();
        break;
    case CxthPc::PFC_PURGE:
        m_ctrl->purge();
        break;
    case CxthPc::PFC_HOLD:
        m_ctrl->pause();
        break;
    case CxthPc::PFC_READ_PRESS:
        m_ctrl->replyPressureToPc();
        break;
    case CxthPc::PFC_TIME_SYNC:
        if (m_ctrl->stat() == MachineController::Stat::Stop)
            m_ctrl->start();
        break;
    default:
        break;
    }
}
