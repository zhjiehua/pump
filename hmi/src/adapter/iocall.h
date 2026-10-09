#ifndef ADAPTER_IOCALL_H
#define ADAPTER_IOCALL_H

#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QThread>
#include <QVariantMap>

/** GUI → IO: always Queued except open/close which wait for a bool. */
namespace IoCall {

inline Qt::ConnectionType blocking(const QObject *obj)
{
    if (!obj)
        return Qt::DirectConnection;
    return (QThread::currentThread() == obj->thread())
        ? Qt::DirectConnection
        : Qt::BlockingQueuedConnection;
}

inline void queued(QObject *obj, const char *method)
{
    if (obj)
        QMetaObject::invokeMethod(obj, method, Qt::QueuedConnection);
}

inline void queued(QObject *obj, const char *method, double a)
{
    if (obj)
        QMetaObject::invokeMethod(obj, method, Qt::QueuedConnection, Q_ARG(double, a));
}

inline void queued(QObject *obj, const char *method, int a)
{
    if (obj)
        QMetaObject::invokeMethod(obj, method, Qt::QueuedConnection, Q_ARG(int, a));
}

inline void queued(QObject *obj, const char *method, bool a)
{
    if (obj)
        QMetaObject::invokeMethod(obj, method, Qt::QueuedConnection, Q_ARG(bool, a));
}

inline void queued(QObject *obj, const char *method, uint a)
{
    if (obj)
        QMetaObject::invokeMethod(obj, method, Qt::QueuedConnection, Q_ARG(uint, a));
}

inline void queued(QObject *obj, const char *method, int a, int b)
{
    if (obj)
        QMetaObject::invokeMethod(obj, method, Qt::QueuedConnection, Q_ARG(int, a), Q_ARG(int, b));
}

inline void queued(QObject *obj, const char *method, int a, double b)
{
    if (obj)
        QMetaObject::invokeMethod(obj, method, Qt::QueuedConnection, Q_ARG(int, a), Q_ARG(double, b));
}

inline void queued(QObject *obj, const char *method, int a, double b, double c)
{
    if (obj)
        QMetaObject::invokeMethod(obj, method, Qt::QueuedConnection,
                                  Q_ARG(int, a), Q_ARG(double, b), Q_ARG(double, c));
}

inline bool blockingBool(QObject *obj, const char *method, const QString &port, int baud, int addr)
{
    bool ok = false;
    if (!obj)
        return false;
    QMetaObject::invokeMethod(obj, method, blocking(obj),
                              Q_RETURN_ARG(bool, ok),
                              Q_ARG(QString, port), Q_ARG(int, baud), Q_ARG(int, addr));
    return ok;
}

inline bool blockingBool(QObject *obj, const char *method, const QString &port, int baud)
{
    bool ok = false;
    if (!obj)
        return false;
    QMetaObject::invokeMethod(obj, method, blocking(obj),
                              Q_RETURN_ARG(bool, ok),
                              Q_ARG(QString, port), Q_ARG(int, baud));
    return ok;
}

inline bool blockingBool(QObject *obj, const char *method, const QVariantMap &cfg)
{
    bool ok = false;
    if (!obj)
        return false;
    QMetaObject::invokeMethod(obj, method, blocking(obj),
                              Q_RETURN_ARG(bool, ok), Q_ARG(QVariantMap, cfg));
    return ok;
}

inline void blockingVoid(QObject *obj, const char *method)
{
    if (obj)
        QMetaObject::invokeMethod(obj, method, blocking(obj));
}

} // namespace IoCall

#endif
