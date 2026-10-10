#include "adapter/qinfinebackend.h"
#include "adapter/iocall.h"
#include "adapter/mcuportagent.h"
#include "protocol/mcu/qinfine/qinfinecodec.h"
#include "utils/eventlog.h"

#include <QtGlobal>

QinFinePumpBackend::QinFinePumpBackend(McuPortAgent *agent, QObject *parent)
    : IPumpBackend(parent)
    , m_agent(agent)
{
}

bool QinFinePumpBackend::open(AppSettings *s)
{
    if (!s || !m_agent)
        return false;
    const int baud = s->mcuBaud > 0 ? s->mcuBaud : 115200;
    return IoCall::blockingBool(m_agent, "openQinFine", s->mcuPort, baud, int(s->mcuAddress));
}

void QinFinePumpBackend::close()
{
    IoCall::blockingVoid(m_agent, "closeMcu");
}

void QinFinePumpBackend::applyFlow(double mlMin, bool)
{
    EventLog_key(QStringLiteral("MCU-TX"),
                  QStringLiteral("set flow %1 mL/min").arg(mlMin, 0, 'f', 3));
    IoCall::queued(m_agent, "qfSetFlow", mlMin);
}

void QinFinePumpBackend::applyStop()
{
    EventLog_key(QStringLiteral("MCU-TX"), QStringLiteral("stop motor"));
    IoCall::queued(m_agent, "qfSetStartStop", false);
    IoCall::queued(m_agent, "qfSetPurge", false);
}

void QinFinePumpBackend::applyPause(bool on)
{
    IoCall::queued(m_agent, "qfSetPause", on);
}

void QinFinePumpBackend::applyPurge(double purgeFlow)
{
    IoCall::queued(m_agent, "qfSetFlow", purgeFlow);
    IoCall::queued(m_agent, "qfSetPurge", true);
    IoCall::queued(m_agent, "qfSetStartStop", true);
}

void QinFinePumpBackend::applyRun(double flow, double percent)
{
    IoCall::queued(m_agent, "qfSetPause", false);
    IoCall::queued(m_agent, "qfSetPurge", false);
    IoCall::queued(m_agent, "qfSetFlow", flow);
    IoCall::queued(m_agent, "qfSetPercent", int(qBound(0.0, percent, 100.0)));
    IoCall::queued(m_agent, "qfSetStartStop", true);
}

void QinFinePumpBackend::applyPercent(double percent)
{
    IoCall::queued(m_agent, "qfSetPercent", int(qBound(0.0, percent, 100.0)));
}

void QinFinePumpBackend::applyPressLimits(double pmin, double pmax)
{
    IoCall::queued(m_agent, "qfSetPressMin", pmin);
    IoCall::queued(m_agent, "qfSetPressMax", pmax);
}

void QinFinePumpBackend::applyPressZero()
{
    IoCall::queued(m_agent, "qfPressZero");
}

void QinFinePumpBackend::applyPressCompen(quint8 on)
{
    IoCall::queued(m_agent, "qfSetPressCompen", int(on));
}

void QinFinePumpBackend::applyLoadParams(double rate, double real, double press)
{
    IoCall::queued(m_agent, "qfSetLoadFloat", int(QinFine::PES_LOAD_FLOW), rate);
    IoCall::queued(m_agent, "qfSetLoadFloat", int(QinFine::PES_LOAD_REAL), real);
    IoCall::queued(m_agent, "qfSetLoadFloat", int(QinFine::PES_LOAD_PRESS), press);
}

void QinFinePumpBackend::applyWorkMode(quint8 mode, quint8 flag)
{
    IoCall::queued(m_agent, "qfSetWorkMode", int(mode), int(flag));
}

void QinFinePumpBackend::applyFlowCalib(bool on)
{
    applyWorkMode(QinFine::WORK_FLOWCALIB, on ? 1 : 0);
}

void QinFinePumpBackend::applyPressCalib(bool on)
{
    applyWorkMode(QinFine::WORK_PRESSCALIB, on ? 1 : 0);
}

void QinFinePumpBackend::writeFlowTable(const QVector<RatePoint> &t)
{
    IoCall::queued(m_agent, "qfSetTableCmd", int(QinFine::PES_FLOW_CMD), int(QinFine::TBL_BEGIN));
    for (int i = 0; i < t.size(); ++i)
        IoCall::queued(m_agent, "qfSetTablePoint", int(QinFine::PES_FLOW_DATA), t[i].rpm, t[i].rate);
    IoCall::queued(m_agent, "qfSetTableCmd", int(QinFine::PES_FLOW_CMD), int(QinFine::TBL_END));
}

void QinFinePumpBackend::writePressTable(const QVector<PressPoint> &t)
{
    IoCall::queued(m_agent, "qfSetTableCmd", int(QinFine::PES_PRESS_CMD), int(QinFine::TBL_BEGIN));
    for (int i = 0; i < t.size(); ++i)
        IoCall::queued(m_agent, "qfSetTablePoint", int(QinFine::PES_PRESS_DATA), t[i].adc, t[i].pressure);
    IoCall::queued(m_agent, "qfSetTableCmd", int(QinFine::PES_PRESS_CMD), int(QinFine::TBL_END));
}

void QinFinePumpBackend::writePulseTable(const QVector<PulsePoint> &t, bool save)
{
    IoCall::queued(m_agent, "qfSetTableCmd", int(QinFine::PES_PULSE_CMD), int(QinFine::TBL_BEGIN));
    for (int i = 0; i < t.size(); ++i)
        IoCall::queued(m_agent, "qfSetTablePoint", int(QinFine::PES_PULSE_DATA), t[i].position, t[i].factor);
    IoCall::queued(m_agent, "qfSetTableCmd", int(QinFine::PES_PULSE_CMD), int(QinFine::TBL_END));
    if (save)
        IoCall::queued(m_agent, "qfSetTableCmd", int(QinFine::PES_PULSE_CMD), int(QinFine::TBL_SAVE));
}

void QinFinePumpBackend::clearPulseTable()
{
    IoCall::queued(m_agent, "qfSetTableCmd", int(QinFine::PES_PULSE_CMD), int(QinFine::TBL_CLEAR));
}

void QinFinePumpBackend::requestFlowTable()
{
    IoCall::queued(m_agent, "qfGetTable", int(QinFine::PES_FLOW_DATA));
}

void QinFinePumpBackend::requestPressTable()
{
    IoCall::queued(m_agent, "qfGetTable", int(QinFine::PES_PRESS_DATA));
}

void QinFinePumpBackend::requestPulseTable()
{
    IoCall::queued(m_agent, "qfGetTable", int(QinFine::PES_PULSE_DATA));
}

void QinFinePumpBackend::requestLoadFloats()
{
    IoCall::queued(m_agent, "qfGetLoadFloat", int(QinFine::PES_LOAD_FLOW));
    IoCall::queued(m_agent, "qfGetLoadFloat", int(QinFine::PES_LOAD_REAL));
    IoCall::queued(m_agent, "qfGetLoadFloat", int(QinFine::PES_LOAD_PRESS));
}

void QinFinePumpBackend::setTableCmd(quint8 sub, quint8 cmd)
{
    IoCall::queued(m_agent, "qfSetTableCmd", int(sub), int(cmd));
}

void QinFinePumpBackend::setTablePoint(quint8 sub, float a, float b)
{
    IoCall::queued(m_agent, "qfSetTablePoint", int(sub), double(a), double(b));
}

void QinFinePumpBackend::setLoadFloat(quint8 sub, float v)
{
    IoCall::queued(m_agent, "qfSetLoadFloat", int(sub), double(v));
}
