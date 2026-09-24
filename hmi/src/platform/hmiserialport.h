#ifndef HMISERIALPORT_H
#define HMISERIALPORT_H

#include <QObject>
#include <QString>
#include <QByteArray>

#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
#include <QSerialPort>
#else
#include "posix_qextserialport.h"
#endif

/** Cross-Qt serial port: QSerialPort (Qt5+) or qextserialport polling (Qt4). */
class HmiSerialPort : public QObject
{
    Q_OBJECT
public:
    explicit HmiSerialPort(QObject *parent = 0);
    ~HmiSerialPort();

    bool open(const QString &port, int baud);
    void close();
    bool isOpen() const;
    qint64 write(const QByteArray &data);
    QByteArray readAll();
    QString errorString() const;

signals:
    void readyRead();

private slots:
    void onPoll();

private:
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    QSerialPort m_port;
#else
    Posix_QextSerialPort *m_port;
    class QTimer *m_poll;
    QString m_lastError;
#endif
};

namespace HmiSerialPortInfo {
QStringList availablePortNames();
}

#endif
