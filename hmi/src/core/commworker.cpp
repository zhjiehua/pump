#include "core/commworker.h"

#include <QTimer>

CommWorker::CommWorker(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    m_timer->setInterval(500);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
}

CommWorker::~CommWorker() {}

void CommWorker::onTimer()
{
    emit pollTick();
}

void CommWorker::startPolling(int intervalMs)
{
    m_timer->setInterval(intervalMs > 0 ? intervalMs : 500);
    m_timer->start();
}

void CommWorker::stopPolling()
{
    m_timer->stop();
}
