#ifndef GRADIENTENGINE_H
#define GRADIENTENGINE_H

#include <QObject>
#include <QVector>
#include "core/appsettings.h"

/** Runtime gradient flow lookup (weiduodianzi updateFlowByGradientList). */
class GradientEngine : public QObject
{
    Q_OBJECT
public:
    explicit GradientEngine(AppSettings *settings, QObject *parent = nullptr);

    void reload(int tableIndex);
    quint32 endTimeSec() const { return m_endSec; }
    double flowAtElapsed(quint32 elapsedSec) const;
    bool flowChangedAt(quint32 elapsedSec, double *flowOut) const;

signals:
    void flowStep(double mlMin);

private:
    AppSettings *m_settings = nullptr;
    QVector<GradientPoint> m_points;
    quint32 m_endSec = 60;
};

#endif
