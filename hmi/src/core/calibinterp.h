#ifndef CALIBINTERP_H
#define CALIBINTERP_H

#include "core/appsettings.h"

#include <QVector>

/** Piecewise linear calibration matching HPLC_PUMP_Mini rate/pressure tables. */
namespace CalibInterp {

constexpr int kMaxFlowPoints = 15;
constexpr int kMaxPressPoints = 10;

QVector<RatePoint> sanitizeFlowTable(const QVector<RatePoint> &table);
QVector<PressPoint> sanitizePressTable(const QVector<PressPoint> &table);

/** Set flow -> command flow via S^2/A transform then lerp. Empty table is identity. */
double commandFlowFromTable(const QVector<RatePoint> &table, double setFlow);

/** Linear MPa -> displayed MPa. Empty table is identity. */
double displayPressFromTable(const QVector<PressPoint> &table, double linearPress);

}

#endif
