#include "adapter/mcuportagent.h"
#include "protocol/mcu/qinfine/qinfineclient.h"
#include "protocol/mcu/qinfine/qinfinecodec.h"
#include "protocol/mcu/cxth/cxthmcuclient.h"

#include <QTimer>

McuPortAgent::McuPortAgent(QObject *parent)
    : QObject(parent)
{
}

void McuPortAgent::init()
{
    if (m_poll)
        return;
    m_poll = new QTimer(this);
    connect(m_poll, SIGNAL(timeout()), this, SLOT(onPoll()));
    m_poll->start(500);
}

void McuPortAgent::shutdown()
{
    destroyClients();
    if (m_poll)
    {
        m_poll->stop();
        m_poll->deleteLater();
        m_poll = nullptr;
    }
}

void McuPortAgent::destroyClients()
{
    if (m_qf)
    {
        m_qf->close();
        m_qf->deleteLater();
        m_qf = nullptr;
        m_qfBound = false;
    }
    if (m_cxth)
    {
        m_cxth->close();
        m_cxth->deleteLater();
        m_cxth = nullptr;
        m_cxthBound = false;
    }
}

void McuPortAgent::bindQinFine()
{
    if (m_qfBound || !m_qf)
        return;
    connect(m_qf, SIGNAL(connectedChanged(bool)), this, SIGNAL(connectedChanged(bool)));
    connect(m_qf, SIGNAL(pressureUpdated(float)), this, SIGNAL(qfPressure(float)));
    connect(m_qf, SIGNAL(extPoint(quint8,float,float)), this, SLOT(onQfExtPoint(quint8,float,float)));
    connect(m_qf, SIGNAL(extFloat(quint8,float)), this, SLOT(onQfExtFloat(quint8,float)));
    connect(m_qf, SIGNAL(extU8(quint8,quint8)), this, SLOT(onQfExtU8(quint8,quint8)));
    connect(m_qf, SIGNAL(errorText(QString)), this, SIGNAL(errorText(QString)));
    m_qfBound = true;
}

void McuPortAgent::bindCxth()
{
    if (m_cxthBound || !m_cxth)
        return;
    connect(m_cxth, SIGNAL(connectedChanged(bool)), this, SIGNAL(connectedChanged(bool)));
    connect(m_cxth, SIGNAL(pressureRaw(quint32)), this, SLOT(onCxthRaw(quint32)));
    connect(m_cxth, SIGNAL(errorText(QString)), this, SIGNAL(errorText(QString)));
    m_cxthBound = true;
}

bool McuPortAgent::openQinFine(const QString &port, int baud, int addr)
{
    destroyClients();
    m_qf = new QinFineClient(this);
    bindQinFine();
    return m_qf->open(port, baud, quint8(addr));
}

bool McuPortAgent::openCxth(const QString &port, int baud)
{
    destroyClients();
    m_cxth = new CxthMcuClient(this);
    bindCxth();
    return m_cxth->open(port, baud);
}

void McuPortAgent::closeMcu()
{
    destroyClients();
}

void McuPortAgent::onPoll()
{
    if (m_qf && m_qf->isOpen())
    {
        m_qf->tick();
        m_qf->getPressure();
        emit pollTick();
    }
    else if (m_cxth && m_cxth->isOpen())
    {
        m_cxth->pollPressure();
        emit pollTick();
    }
}

void McuPortAgent::qfSetFlow(double mlMin)
{
    if (m_qf)
        m_qf->setFlow(float(mlMin));
}

void McuPortAgent::qfSetPercent(int percent)
{
    if (m_qf)
        m_qf->setPercent(quint8(percent));
}

void McuPortAgent::qfSetPressMin(double mpa)
{
    if (m_qf)
        m_qf->setPressMin(float(mpa));
}

void McuPortAgent::qfSetPressMax(double mpa)
{
    if (m_qf)
        m_qf->setPressMax(float(mpa));
}

void McuPortAgent::qfSetStartStop(bool run)
{
    if (m_qf)
        m_qf->setStartStop(run);
}

void McuPortAgent::qfSetPause(bool on)
{
    if (m_qf)
        m_qf->setPause(on);
}

void McuPortAgent::qfSetPurge(bool on)
{
    if (m_qf)
        m_qf->setPurge(on);
}

void McuPortAgent::qfPressZero()
{
    if (m_qf)
        m_qf->pressZero();
}

void McuPortAgent::qfSetPressCompen(int on)
{
    if (m_qf)
        m_qf->setPressCompen(quint8(on));
}

void McuPortAgent::qfSetWorkMode(int mode, int flag)
{
    if (m_qf)
        m_qf->setWorkMode(quint8(mode), quint8(flag));
}

void McuPortAgent::qfSetTableCmd(int sub, int cmd)
{
    if (m_qf)
        m_qf->setTableCmd(quint8(sub), quint8(cmd));
}

void McuPortAgent::qfSetTablePoint(int sub, double a, double b)
{
    if (m_qf)
        m_qf->setTablePoint(quint8(sub), float(a), float(b));
}

void McuPortAgent::qfGetTable(int sub)
{
    if (m_qf)
        m_qf->getTable(quint8(sub));
}

void McuPortAgent::qfSetLoadFloat(int sub, double v)
{
    if (m_qf)
        m_qf->setLoadFloat(quint8(sub), float(v));
}

void McuPortAgent::qfGetLoadFloat(int sub)
{
    if (m_qf)
        m_qf->getLoadFloat(quint8(sub));
}

void McuPortAgent::cxthSetFlowWord(uint word)
{
    if (m_cxth)
        m_cxth->setFlowWord(quint32(word));
}

void McuPortAgent::cxthStopMotor()
{
    if (m_cxth)
        m_cxth->stopMotor();
}

void McuPortAgent::onQfExtPoint(quint8 sub, float a, float b)
{
    emit extPoint(int(sub), a, b);
}

void McuPortAgent::onQfExtFloat(quint8 sub, float v)
{
    emit extFloat(int(sub), v);
}

void McuPortAgent::onQfExtU8(quint8 sub, quint8 v)
{
    emit extU8(int(sub), int(v));
}

void McuPortAgent::onCxthRaw(quint32 raw)
{
    emit cxthPressureRaw(uint(raw));
}
