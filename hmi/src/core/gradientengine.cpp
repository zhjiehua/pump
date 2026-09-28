#include "core/gradientengine.h"

GradientEngine::GradientEngine(AppSettings *settings, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
{
}

void GradientEngine::reload(int tableIndex)
{
    m_points.clear();
    m_endSec = 60;
    if (!m_settings)
        return;
    m_points = m_settings->gradientTable(tableIndex);
    if (m_points.isEmpty())
        return;
    const double lastMin = m_points.last().timeMin;
    m_endSec = quint32(qMax(1.0, lastMin * 60.0));
}

double GradientEngine::flowAtElapsed(quint32 elapsedSec) const
{
    if (m_points.isEmpty())
        return 0;
    const quint32 t = m_endSec > 0 ? elapsedSec % m_endSec : elapsedSec;
    double flow = m_points.first().flow;
    for (const GradientPoint &p : m_points)
    {
        const quint32 pt = quint32(p.timeMin * 60.0);
        if (t >= pt)
            flow = p.flow;
    }
    return flow;
}

bool GradientEngine::flowChangedAt(quint32 elapsedSec, double *flowOut) const
{
    if (!flowOut || m_points.isEmpty())
        return false;
    const quint32 t = m_endSec > 0 ? elapsedSec % m_endSec : elapsedSec;
    for (const GradientPoint &p : m_points)
    {
        const quint32 pt = quint32(p.timeMin * 60.0);
        if (t == pt)
        {
            *flowOut = p.flow;
            return true;
        }
    }
    return false;
}
