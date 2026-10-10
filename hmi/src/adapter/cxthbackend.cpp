#include "adapter/cxthbackend.h"
#include "adapter/iocall.h"
#include "adapter/mcuportagent.h"
#include "core/calibinterp.h"
#include "utils/eventlog.h"

CxthPumpBackend::CxthPumpBackend(McuPortAgent *agent, AppSettings *settings, QObject *parent)
    : IPumpBackend(parent)
    , m_agent(agent)
    , m_settings(settings)
{
    if (settings)
        m_flowTable = settings->flowTable;
}

bool CxthPumpBackend::open(AppSettings *s)
{
    if (!s || !m_agent)
        return false;
    m_settings = s;
    m_flowTable = s->flowTable;
    const int baud = s->mcuBaud > 0 ? s->mcuBaud : 9600;
    return IoCall::blockingBool(m_agent, "openCxth", s->mcuPort, baud);
}

void CxthPumpBackend::close()
{
    IoCall::blockingVoid(m_agent, "closeMcu");
}

void CxthPumpBackend::sendFlowWord(double mlMin, bool calibActive)
{
    if (!m_settings)
        return;
    double out = mlMin;
    if (!calibActive)
        out = CalibInterp::commandFlowFromTable(m_flowTable, mlMin);
    EventLog_key(QStringLiteral("MCU-TX"),
                  QStringLiteral("set flow %1 mL/min (cmd %2)")
                      .arg(mlMin, 0, 'f', 3)
                      .arg(out, 0, 'f', 3));
    const uint word = uint(qMax(0.0, out * m_settings->mcuWordFactor + 0.5));
    IoCall::queued(m_agent, "cxthSetFlowWord", word);
}

void CxthPumpBackend::applyFlow(double mlMin, bool flowCalibActive)
{
    sendFlowWord(mlMin, flowCalibActive);
}

void CxthPumpBackend::applyStop()
{
    EventLog_key(QStringLiteral("MCU-TX"), QStringLiteral("stop motor"));
    IoCall::queued(m_agent, "cxthStopMotor");
}

void CxthPumpBackend::applyPause(bool)
{
}

void CxthPumpBackend::applyPurge(double purgeFlow)
{
    sendFlowWord(purgeFlow, false);
}

void CxthPumpBackend::applyRun(double flow, double)
{
    sendFlowWord(flow, false);
}

void CxthPumpBackend::applyPercent(double)
{
}

void CxthPumpBackend::applyPressLimits(double, double)
{
}

void CxthPumpBackend::applyPressZero()
{
}

void CxthPumpBackend::applyPressCompen(quint8)
{
}

void CxthPumpBackend::applyLoadParams(double, double, double)
{
}

void CxthPumpBackend::applyWorkMode(quint8, quint8)
{
}

void CxthPumpBackend::applyFlowCalib(bool)
{
}

void CxthPumpBackend::applyPressCalib(bool)
{
}

void CxthPumpBackend::writeFlowTable(const QVector<RatePoint> &t)
{
    m_flowTable = t;
}

void CxthPumpBackend::writePressTable(const QVector<PressPoint> &)
{
}

void CxthPumpBackend::writePulseTable(const QVector<PulsePoint> &, bool)
{
}

void CxthPumpBackend::clearPulseTable()
{
}

void CxthPumpBackend::requestFlowTable()
{
}

void CxthPumpBackend::requestPressTable()
{
}

void CxthPumpBackend::requestPulseTable()
{
}

void CxthPumpBackend::requestLoadFloats()
{
}

void CxthPumpBackend::setTableCmd(quint8, quint8)
{
}

void CxthPumpBackend::setTablePoint(quint8, float, float)
{
}

void CxthPumpBackend::setLoadFloat(quint8, float)
{
}
