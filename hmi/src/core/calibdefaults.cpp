#include "core/calibdefaults.h"

#include <QtGlobal>

namespace CalibDefaults {

namespace {

int clampPumpType(int pumpType)
{
    return qBound(0, pumpType, 10);
}

QVector<RatePoint> identityFlow(const double *flows, int count)
{
    QVector<RatePoint> out;
    out.reserve(count);
    for (int i = 0; i < count; ++i)
        out.append(RatePoint(flows[i], flows[i]));
    return out;
}

QVector<PressPoint> identityPress(const double *press, int count)
{
    QVector<PressPoint> out;
    out.reserve(count);
    for (int i = 0; i < count; ++i)
        out.append(PressPoint(press[i], press[i]));
    return out;
}

} // namespace

QVector<RatePoint> defaultFlowTable(int pumpType)
{
    switch (clampPumpType(pumpType))
    {
    case 0: {
        static const double k[] = {0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1, 2, 5, 10};
        return identityFlow(k, int(sizeof(k) / sizeof(k[0])));
    }
    case 1: {
        static const double k[] = {0.05, 0.2, 0.5, 1, 2, 5, 10, 20, 35, 50};
        return identityFlow(k, int(sizeof(k) / sizeof(k[0])));
    }
    case 2: {
        static const double k[] = {1, 2, 4, 8, 20, 50, 70, 80, 85, 100};
        return identityFlow(k, int(sizeof(k) / sizeof(k[0])));
    }
    case 3: {
        static const double k[] = {2, 5, 10, 20, 40, 60, 80, 100, 125, 150};
        return identityFlow(k, int(sizeof(k) / sizeof(k[0])));
    }
    case 4: {
        static const double k[] = {2, 5, 10, 20, 50, 125, 175, 200, 210, 250};
        return identityFlow(k, int(sizeof(k) / sizeof(k[0])));
    }
    case 5: {
        static const double k[] = {5, 10, 20, 40, 70, 100, 150, 200, 250, 300};
        return identityFlow(k, int(sizeof(k) / sizeof(k[0])));
    }
    case 6: {
        static const double k[] = {5, 10, 20, 40, 100, 250, 350, 400, 425, 500};
        return identityFlow(k, int(sizeof(k) / sizeof(k[0])));
    }
    case 7: {
        static const double k[] = {10, 20, 40, 80, 150, 250, 350, 500, 650, 800};
        return identityFlow(k, int(sizeof(k) / sizeof(k[0])));
    }
    case 8: {
        static const double k[] = {10, 20, 40, 80, 200, 500, 700, 800, 850, 1000};
        return identityFlow(k, int(sizeof(k) / sizeof(k[0])));
    }
    case 9: {
        static const double k[] = {30, 60, 120, 240, 500, 750, 1000, 1300, 1600, 2000};
        return identityFlow(k, int(sizeof(k) / sizeof(k[0])));
    }
    case 10: {
        static const double k[] = {30, 60, 120, 240, 600, 1500, 2100, 2400, 2600, 3000};
        return identityFlow(k, int(sizeof(k) / sizeof(k[0])));
    }
    default:
        return QVector<RatePoint>();
    }
}

QVector<PressPoint> defaultPressTable(int pumpType)
{
    switch (clampPumpType(pumpType))
    {
    case 0: {
        static const double k[] = {0.1, 0.5, 1, 3, 5, 10, 15, 20, 25, 30, 35, 40, 45};
        return identityPress(k, int(sizeof(k) / sizeof(k[0])));
    }
    case 1: {
        static const double k[] = {0.1, 0.5, 1, 3, 5, 10, 15, 20, 25, 30, 35};
        return identityPress(k, int(sizeof(k) / sizeof(k[0])));
    }
    default: {
        static const double k[] = {0.1, 0.5, 1, 3, 5, 7, 9, 10, 12};
        return identityPress(k, int(sizeof(k) / sizeof(k[0])));
    }
    }
}

void applyDefaultTables(AppSettings *settings)
{
    if (!settings)
        return;
    const int pt = settings->pumpType;
    settings->flowTable = defaultFlowTable(pt);
    settings->pressTable = defaultPressTable(pt);
}

} // namespace CalibDefaults
