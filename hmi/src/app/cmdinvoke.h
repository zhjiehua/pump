#ifndef APP_CMDINVOKE_H
#define APP_CMDINVOKE_H

#include <QByteArray>
#include <QMetaObject>
#include <QObject>
#include <QThread>

/** Call a GUI PumpCommand slot from any thread without nested blocking IO. */
namespace CmdInvoke {

inline Qt::ConnectionType typeFor(const QObject *obj)
{
    if (!obj)
        return Qt::DirectConnection;
    return (QThread::currentThread() == obj->thread())
        ? Qt::DirectConnection
        : Qt::BlockingQueuedConnection;
}

inline bool callBool(QObject *obj, const char *method, int source)
{
    if (!obj)
        return false;
    bool ok = false;
    QMetaObject::invokeMethod(obj, method, typeFor(obj),
                              Q_RETURN_ARG(bool, ok), Q_ARG(int, source));
    return ok;
}

inline bool callBool(QObject *obj, const char *method, double a, int source)
{
    if (!obj)
        return false;
    bool ok = false;
    QMetaObject::invokeMethod(obj, method, typeFor(obj),
                              Q_RETURN_ARG(bool, ok), Q_ARG(double, a), Q_ARG(int, source));
    return ok;
}

inline bool callBool(QObject *obj, const char *method, double a, double b, int source)
{
    if (!obj)
        return false;
    bool ok = false;
    QMetaObject::invokeMethod(obj, method, typeFor(obj),
                              Q_RETURN_ARG(bool, ok), Q_ARG(double, a), Q_ARG(double, b),
                              Q_ARG(int, source));
    return ok;
}

inline bool callBoolU8(QObject *obj, const char *method, int v, int source)
{
    if (!obj)
        return false;
    bool ok = false;
    QMetaObject::invokeMethod(obj, method, typeFor(obj),
                              Q_RETURN_ARG(bool, ok), Q_ARG(int, v), Q_ARG(int, source));
    return ok;
}

inline void callVoid(QObject *obj, const char *method)
{
    if (!obj)
        return;
    QMetaObject::invokeMethod(obj, method, typeFor(obj));
}

inline void callVoid(QObject *obj, const char *method, int a)
{
    if (!obj)
        return;
    QMetaObject::invokeMethod(obj, method, typeFor(obj), Q_ARG(int, a));
}

inline void callVoid(QObject *obj, const char *method, int sub, const QByteArray &payload)
{
    if (!obj)
        return;
    QMetaObject::invokeMethod(obj, method, typeFor(obj),
                              Q_ARG(int, sub), Q_ARG(QByteArray, payload));
}

} // namespace CmdInvoke

#endif
