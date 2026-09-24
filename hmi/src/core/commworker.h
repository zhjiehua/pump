#ifndef COMMWORKER_H
#define COMMWORKER_H

#include <QObject>

class QTimer;

/** Isolated MCU/PC poll scheduler (main-thread timer; serial stays on GUI thread). */
class CommWorker : public QObject
{
    Q_OBJECT
public:
    explicit CommWorker(QObject *parent = nullptr);
    ~CommWorker() override;

    void startPolling(int intervalMs = 500);
    void stopPolling();

signals:
    void pollTick();

private slots:
    void onTimer();

private:
    QTimer *m_timer;
};

#endif
