#include "core/calibinterp.h"

#include <QtGlobal>
#include <algorithm>

namespace CalibInterp {

namespace {

double safeDiv(double num, double den, double fallback)
{
    if (den == 0.0 || den != den)
        return fallback;
    return num / den;
}

struct Xy {
    double x;
    double y;
    Xy() : x(0), y(0) {}
    Xy(double xx, double yy) : x(xx), y(yy) {}
};

double lerpTable(const QVector<Xy> &pts, double x, bool passIfFirstXZero)
{
    const int size = pts.size();
    if (size == 0)
        return x;

    int i = 0;
    for (; i < size; ++i)
    {
        if (x <= pts[i].x)
            break;
    }

    if (i == 0 || size == 1)
    {
        if (passIfFirstXZero && pts[0].x == 0.0)
            return x;
        return safeDiv(pts[0].y, pts[0].x, 1.0) * x;
    }
    if (i == size)
        --i;
    return safeDiv(pts[i].y - pts[i - 1].y, pts[i].x - pts[i - 1].x, 1.0)
               * (x - pts[i - 1].x)
           + pts[i - 1].y;
}

template <typename Point, typename GetX>
QVector<Point> sanitizeByX(const QVector<Point> &table, int maxPoints, GetX getX)
{
    QVector<Point> sorted = table;
    std::sort(sorted.begin(), sorted.end(),
              [&getX](const Point &a, const Point &b) { return getX(a) < getX(b); });

    QVector<Point> out;
    out.reserve(qMin(sorted.size(), maxPoints));
    for (int i = 0; i < sorted.size() && out.size() < maxPoints; ++i)
    {
        const double x = getX(sorted[i]);
        if (x < 0.0)
            continue;
        if (!out.isEmpty() && getX(out.last()) >= x)
            continue;
        out.append(sorted[i]);
    }
    return out;
}

} // namespace

QVector<RatePoint> sanitizeFlowTable(const QVector<RatePoint> &table)
{
    return sanitizeByX(table, kMaxFlowPoints,
                       [](const RatePoint &p) { return p.rpm; });
}

QVector<PressPoint> sanitizePressTable(const QVector<PressPoint> &table)
{
    return sanitizeByX(table, kMaxPressPoints,
                       [](const PressPoint &p) { return p.adc; });
}

double commandFlowFromTable(const QVector<RatePoint> &table, double setFlow)
{
    const QVector<RatePoint> pts = sanitizeFlowTable(table);
    if (pts.isEmpty())
        return setFlow;

    QVector<Xy> trans;
    trans.reserve(pts.size());
    for (int i = 0; i < pts.size(); ++i)
    {
        const double s = pts[i].rpm;
        const double a = pts[i].rate;
        trans.append(Xy(s, safeDiv(s, a, s) * s));
    }
    return lerpTable(trans, setFlow, false);
}

double displayPressFromTable(const QVector<PressPoint> &table, double linearPress)
{
    const QVector<PressPoint> pts = sanitizePressTable(table);
    if (pts.isEmpty())
        return linearPress;

    QVector<Xy> mapped;
    mapped.reserve(pts.size());
    for (int i = 0; i < pts.size(); ++i)
        mapped.append(Xy(pts[i].adc, pts[i].pressure));
    return lerpTable(mapped, linearPress, true);
}

} // namespace CalibInterp
