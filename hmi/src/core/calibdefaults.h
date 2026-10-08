#ifndef CALIBDEFAULTS_H
#define CALIBDEFAULTS_H

#include "core/appsettings.h"

#include <QVector>

/** Factory default flow/press tables (weiduodianzi COMPENSATIONTABLE0/1, factor 1). */
namespace CalibDefaults {

QVector<RatePoint> defaultFlowTable(int pumpType);
QVector<PressPoint> defaultPressTable(int pumpType);

void applyDefaultTables(AppSettings *settings);

} // namespace CalibDefaults

#endif
