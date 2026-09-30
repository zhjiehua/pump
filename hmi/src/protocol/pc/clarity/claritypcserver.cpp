#include "protocol/pc/clarity/claritypcserver.h"
#include "core/machinecontroller.h"
#include "protocol/pc/clarity/claritycodec.h"
#include "protocol/pc/clarity/clarityids.h"
#include "utils/eventlog.h"

ClarityPcServer::ClarityPcServer(QObject *parent)
    : PcServer(parent)
{
}

void ClarityPcServer::sendPressure(double mpa)
{
    if (!m_settings || !m_ctrl)
        return;
    const quint32 v = quint32(qAbs(mpa) * 100.0 + 0.5);
    sendBytes(ClarityCodec::encode(m_settings->machineCode, 0, ClarityPc::PFC_SEND_PRESS, v));
}

void ClarityPcServer::handle(const QByteArray &chunk)
{
    m_rx.append(chunk);
    if (!m_settings)
        return;
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
    if (!ClarityCodec::decode(frame, &id, &ai, &pfc, &val) || !m_ctrl)
    {
        sendBytes(ClarityCodec::encodeAck(false));
        return;
    }
    if (pfc != ClarityPc::PFC_READ_ID && id != m_settings->machineCode)
        return;

    auto ack = [this]() { sendBytes(ClarityCodec::encodeAck(true)); };

    EventLog::key(QStringLiteral("PC-RX"),
                  QStringLiteral("Clarity %1 id=%2 ai=%3 val=%4")
                      .arg(EventLog::pcClarityPfcName(pfc))
                      .arg(id)
                      .arg(ai)
                      .arg(val));

    switch (pfc)
    {
    case ClarityPc::PFC_READ_ID:
        sendBytes(ClarityCodec::encode(m_settings->machineCode, 0, ClarityPc::PFC_READ_ID, m_settings->machineCode));
        break;
    case ClarityPc::PFC_STATUS:
    {
        quint32 st = quint32(m_ctrl->flow() * 1000.0 + 0.5) & 0xFFFFF;
        if (m_ctrl->stat() != MachineController::Stat::Stop)
            st |= (1u << 20);
        sendBytes(ClarityCodec::encode(m_settings->machineCode, 0, ClarityPc::PFC_STATUS, st));
        break;
    }
    case ClarityPc::PFC_SET_FLOW:
        m_ctrl->enterPcControl();
        m_ctrl->setFlow(val / 1000.0);
        ack();
        break;
    case ClarityPc::PFC_SET_PERCENT:
        m_ctrl->setPercent(val / 10.0);
        ack();
        break;
    case ClarityPc::PFC_MAX_PRESS:
        m_ctrl->setPressLimits(m_ctrl->settings()->pressMin, val / 100.0);
        ack();
        break;
    case ClarityPc::PFC_MIN_PRESS:
        m_ctrl->setPressLimits(val / 100.0, m_ctrl->settings()->pressMax);
        ack();
        break;
    case ClarityPc::PFC_START:
        m_ctrl->start();
        ack();
        break;
    case ClarityPc::PFC_STOP:
        m_ctrl->stop();
        ack();
        break;
    case ClarityPc::PFC_PRESSCLEAR:
        m_ctrl->pressZero();
        ack();
        break;
    case ClarityPc::PFC_READ_PRESS:
        m_ctrl->replyPressureToPc();
        ack();
        break;
    case ClarityPc::PFC_SYNCTIME:
        if (m_ctrl->stat() == MachineController::Stat::Stop)
            m_ctrl->start();
        ack();
        break;
    case ClarityPc::PFC_PURGE:
        m_ctrl->purge();
        ack();
        break;
    case ClarityPc::PFC_HOLD:
        m_ctrl->pause();
        ack();
        break;
    case ClarityPc::PFC_PRESS_COMPEN:
        if (val <= 1)
            m_ctrl->setPressCompen(quint8(val));
        sendBytes(ClarityCodec::encode(m_settings->machineCode, 0, ClarityPc::PFC_PRESS_COMPEN, m_ctrl->pressCompen()));
        break;
    default:
        ack();
        break;
    }
}
