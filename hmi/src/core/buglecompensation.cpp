#include "core/buglecompensation.h"

BugleCompensation *BugleCompensation::instance()
{
    static BugleCompensation s;
    return &s;
}

BugleCompensation::BugleCompensation(QObject *parent)
    : QObject(parent)
{
}

void BugleCompensation::bugleSignal()
{
    m_factor = 1.0;
    emit outputChanged(m_factor);
}

void BugleCompensation::pauseOutput()
{
}

void BugleCompensation::adjustOutputByInput(quint32 rawPress)
{
    Q_UNUSED(rawPress);
}
