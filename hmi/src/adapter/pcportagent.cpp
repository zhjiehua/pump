#include "adapter/pcportagent.h"
#include "app/pumpcommand.h"
#include "domain/pumpsession.h"
#include "protocol/pc/cxth/cxthpcserver.h"
#include "protocol/pc/clarity/claritypcserver.h"
#include "protocol/pc/qinfine/qinfinepcserver.h"

PcPortAgent::PcPortAgent(QObject *parent)
    : QObject(parent)
{
}

void PcPortAgent::init()
{
    if (m_cxth)
        return;
    m_cxth = new CxthPcServer(this);
    m_clarity = new ClarityPcServer(this);
    m_qinFine = new QinFinePcServer(this);
}

void PcPortAgent::shutdown()
{
    stopLink();
    if (m_cxth)
    {
        m_cxth->deleteLater();
        m_cxth = nullptr;
    }
    if (m_clarity)
    {
        m_clarity->deleteLater();
        m_clarity = nullptr;
    }
    if (m_qinFine)
    {
        m_qinFine->deleteLater();
        m_qinFine = nullptr;
    }
}

void PcPortAgent::setFacade(QObject *cmd, QObject *session)
{
    PumpCommand *c = qobject_cast<PumpCommand *>(cmd);
    PumpSession *s = qobject_cast<PumpSession *>(session);
    if (m_cxth)
        m_cxth->setFacade(c, s);
    if (m_clarity)
        m_clarity->setFacade(c, s);
    if (m_qinFine)
        m_qinFine->setFacade(c, s);
}

PcServer *PcPortAgent::active() const
{
    if (m_protocol == 1)
        return m_clarity;
    if (m_protocol == 2)
        return m_qinFine;
    return m_cxth;
}

bool PcPortAgent::startLink(const QVariantMap &cfg)
{
    stopLink();
    m_protocol = cfg.value(QStringLiteral("pcProtocol")).toInt();
    PcServer *srv = active();
    return srv ? srv->start(cfg) : false;
}

void PcPortAgent::stopLink()
{
    if (m_cxth)
        m_cxth->stop();
    if (m_clarity)
        m_clarity->stop();
    if (m_qinFine)
        m_qinFine->stop();
}

void PcPortAgent::sendPressure(double mpa)
{
    PcServer *srv = active();
    if (srv)
        srv->sendPressure(mpa);
}

void PcPortAgent::sendBytes(const QByteArray &ba)
{
    PcServer *srv = active();
    if (srv)
        srv->sendBytes(ba);
}

void PcPortAgent::dumpFlowTable()
{
    if (m_qinFine)
        m_qinFine->dumpFlowTable();
}

void PcPortAgent::dumpPressTable()
{
    if (m_qinFine)
        m_qinFine->dumpPressTable();
}

void PcPortAgent::dumpPulseTable()
{
    if (m_qinFine)
        m_qinFine->dumpPulseTable();
}
