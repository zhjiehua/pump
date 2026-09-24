#include "protocol/cxthpcserver.h"
#include "core/machinecontroller.h"
#include "protocol/cxthpccodec.h"
#include "protocol/cxthpcids.h"
#include "utils/eventlog.h"

CxthPcServer::CxthPcServer(QObject *parent)
    : QObject(parent)
{
    connect(&m_serial, SIGNAL(readyRead()), this, SLOT(onSerial()));
    connect(&m_udp, SIGNAL(readyRead()), this, SLOT(onUdp()));
}

bool CxthPcServer::start(AppSettings *s)
{
    stop();
    m_settings = s;
    if (s->pcPort == AppSettings::Serial)
    {
        if (!m_serial.open(s->pcSerialPort, s->pcSerialBaud))
            return false;
    }
    else
    {
        if (!m_udp.bind(QHostAddress::Any, s->localUdpPort))
            return false;
        m_peer = QHostAddress(s->remoteIp);
        m_peerPort = s->remotePort;
    }
    m_running = true;
    EventLog::key(QStringLiteral("PC"),
                  s->pcPort == AppSettings::Serial
                      ? QStringLiteral("started serial %1 @ %2").arg(s->pcSerialPort).arg(s->pcSerialBaud)
                      : QStringLiteral("started UDP local:%1 remote:%2:%3")
                            .arg(s->localUdpPort)
                            .arg(s->remoteIp)
                            .arg(s->remotePort));
    return true;
}

void CxthPcServer::stop()
{
    if (m_running)
        EventLog::key(QStringLiteral("PC"), QStringLiteral("stopped"));
    if (m_serial.isOpen())
        m_serial.close();
    m_udp.close();
    m_rx.clear();
    m_running = false;
}

void CxthPcServer::sendBytes(const QByteArray &ba)
{
    if (!m_running)
        return;
    if (m_serial.isOpen())
        m_serial.write(ba);
    else if (m_peerPort)
        m_udp.writeDatagram(ba, m_peer, m_peerPort);
}

void CxthPcServer::sendPressure(double mpa)
{
    if (!m_settings || !m_ctrl)
        return;
    const quint32 v = quint32(qAbs(mpa) * 100.0 + 0.5);
    if (m_settings->pcProtocol == AppSettings::Clarity)
        sendBytes(CxthPcCodec::encodeClarity(m_settings->machineCode, 0, CxthPc::PFCC_SEND_PRESS, v));
    else
        sendBytes(CxthPcCodec::encodeLegacy(CxthPc::PFC_READ_PRESS, v));
}

void CxthPcServer::sendCalib(quint8 ai, quint32 value)
{
    if (!m_settings)
        return;
    if (m_settings->pcProtocol == AppSettings::Clarity)
        sendBytes(CxthPcCodec::encodeClarity(m_settings->machineCode, ai, CxthPc::PFCC_EXT, value));
    else
        sendBytes(CxthPcCodec::encodeLegacy(CxthPc::PFC_CALIB, value, ai));
}

void CxthPcServer::dumpFlowTable()
{
    if (!m_ctrl)
        return;
    sendCalib(CxthPc::AI_FLOW_CMD, CxthPc::CALIB_BEGIN);
    for (const auto &p : m_ctrl->flowTable())
    {
        sendCalib(CxthPc::AI_FLOW_A, CxthPc::packCount(p.rpm));
        sendCalib(CxthPc::AI_FLOW_B, CxthPc::packMilli(p.rate));
    }
    sendCalib(CxthPc::AI_LOAD_FLOW, CxthPc::packMilli(m_ctrl->loadRate()));
    sendCalib(CxthPc::AI_LOAD_REAL, CxthPc::packMilli(m_ctrl->loadReal()));
    sendCalib(CxthPc::AI_LOAD_PRESS, CxthPc::packMilli(m_ctrl->loadPress()));
    sendCalib(CxthPc::AI_FLOW_CMD, CxthPc::CALIB_END);
}

void CxthPcServer::dumpPressTable()
{
    if (!m_ctrl)
        return;
    sendCalib(CxthPc::AI_PRESS_CMD, CxthPc::CALIB_BEGIN);
    for (const auto &p : m_ctrl->pressTable())
    {
        sendCalib(CxthPc::AI_PRESS_A, CxthPc::packCount(p.adc));
        sendCalib(CxthPc::AI_PRESS_B, CxthPc::packMilli(p.pressure));
    }
    sendCalib(CxthPc::AI_PRESS_CMD, CxthPc::CALIB_END);
}

void CxthPcServer::dumpPulseTable()
{
    if (!m_ctrl)
        return;
    sendCalib(CxthPc::AI_PULSE_CMD, CxthPc::CALIB_BEGIN);
    for (const auto &p : m_ctrl->pulseTable())
    {
        sendCalib(CxthPc::AI_PULSE_A, CxthPc::packCount(p.position));
        sendCalib(CxthPc::AI_PULSE_B, CxthPc::packMilli(p.factor));
    }
    sendCalib(CxthPc::AI_PULSE_CMD, CxthPc::CALIB_END);
}

void CxthPcServer::onSerial()
{
    handle(m_serial.readAll());
}

void CxthPcServer::onUdp()
{
    while (m_udp.hasPendingDatagrams())
    {
        QByteArray d;
        d.resize(int(m_udp.pendingDatagramSize()));
        m_udp.readDatagram(d.data(), d.size(), &m_peer, &m_peerPort);
        handle(d);
    }
}

void CxthPcServer::handle(const QByteArray &chunk)
{
    m_rx.append(chunk);
    if (!m_settings)
        return;
    if (m_settings->pcProtocol == AppSettings::Clarity)
    {
        while (true)
        {
            const int s = m_rx.indexOf(char(CxthPc::STX));
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
            handleClarity(frame);
        }
    }
    else
    {
        while (m_rx.size() >= 4)
        {
            const int n = m_rx.size() >= 5 ? 5 : 4;
            const QByteArray frame = m_rx.left(n);
            m_rx.remove(0, n);
            handleLegacy(frame);
        }
    }
}

void CxthPcServer::handleClarity(const QByteArray &frame)
{
    quint8 id = 0, ai = 0, pfc = 0;
    quint32 val = 0;
    if (!CxthPcCodec::decodeClarity(frame, &id, &ai, &pfc, &val) || !m_ctrl)
    {
        sendBytes(CxthPcCodec::encodeAck(false));
        return;
    }
    if (pfc != CxthPc::PFCC_READ_ID && id != m_settings->machineCode)
        return;

    auto ack = [this]() { sendBytes(CxthPcCodec::encodeAck(true)); };

    EventLog::key(QStringLiteral("PC-RX"),
                  QStringLiteral("Clarity %1 id=%2 ai=%3 val=%4")
                      .arg(EventLog::pcClarityPfcName(pfc))
                      .arg(id)
                      .arg(ai)
                      .arg(val));

    switch (pfc)
    {
    case CxthPc::PFCC_READ_ID:
        sendBytes(CxthPcCodec::encodeClarity(m_settings->machineCode, 0, CxthPc::PFCC_READ_ID, m_settings->machineCode));
        break;
    case CxthPc::PFCC_STATUS:
    {
        quint32 st = quint32(m_ctrl->flow() * 1000.0 + 0.5) & 0xFFFFF;
        if (m_ctrl->stat() != MachineController::Stat::Stop)
            st |= (1u << 20);
        sendBytes(CxthPcCodec::encodeClarity(m_settings->machineCode, 0, CxthPc::PFCC_STATUS, st));
        break;
    }
    case CxthPc::PFCC_SET_FLOW:
        m_ctrl->enterPcControl();
        m_ctrl->setFlow(val / 1000.0);
        ack();
        break;
    case CxthPc::PFCC_SET_PERCENT:
        m_ctrl->setPercent(val / 10.0);
        ack();
        break;
    case CxthPc::PFCC_MAX_PRESS:
        m_ctrl->setPressLimits(m_ctrl->settings()->pressMin, val / 100.0);
        ack();
        break;
    case CxthPc::PFCC_MIN_PRESS:
        m_ctrl->setPressLimits(val / 100.0, m_ctrl->settings()->pressMax);
        ack();
        break;
    case CxthPc::PFCC_START:
        m_ctrl->start();
        ack();
        break;
    case CxthPc::PFCC_STOP:
        m_ctrl->stop();
        ack();
        break;
    case CxthPc::PFCC_PRESSCLEAR:
        m_ctrl->pressZero();
        ack();
        break;
    case CxthPc::PFCC_READ_PRESS:
        m_ctrl->replyPressureToPc();
        ack();
        break;
    case CxthPc::PFCC_SYNCTIME:
        if (m_ctrl->stat() == MachineController::Stat::Stop)
            m_ctrl->start();
        ack();
        break;
    case CxthPc::PFCC_PURGE:
        m_ctrl->purge();
        ack();
        break;
    case CxthPc::PFCC_HOLD:
        m_ctrl->pause();
        ack();
        break;
    case CxthPc::PFCC_PRESS_COMPEN:
        if (val <= 1)
            m_ctrl->setPressCompen(quint8(val));
        sendBytes(CxthPcCodec::encodeClarity(m_settings->machineCode, 0, CxthPc::PFCC_PRESS_COMPEN, m_ctrl->pressCompen()));
        break;
    case CxthPc::PFCC_EXT:
        m_ctrl->applyCalibCmd(ai, val);
        ack();
        break;
    default:
        ack();
        break;
    }
}

void CxthPcServer::handleLegacy(const QByteArray &frame)
{
    quint8 cmd = 0, add = 0;
    quint32 arg = 0;
    if (!CxthPcCodec::decodeLegacy(frame, &cmd, &arg, &add) || !m_ctrl)
        return;
    EventLog::key(QStringLiteral("PC-RX"),
                  QStringLiteral("Legacy %1 arg=%2 add=%3")
                      .arg(EventLog::pcLegacyCmdName(cmd))
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
    case CxthPc::PFC_CALIB:
        m_ctrl->applyCalibCmd(add, arg);
        break;
    default:
        break;
    }
}
