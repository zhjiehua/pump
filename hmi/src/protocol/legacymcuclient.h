#ifndef LEGACYMCUCLIENT_H
#define LEGACYMCUCLIENT_H

#include <QObject>
#include "platform/hmiserialport.h"

/** Old weiduodianzi MCU UART client: 5-byte 0x80 frames @ typically 9600 8N1. */
class LegacyMcuClient : public QObject
{
    Q_OBJECT
public:
    explicit LegacyMcuClient(QObject *parent = nullptr);

    bool open(const QString &port, int baud);
    void close();
    bool isOpen() const;

    /** Write motor control word (flow * wordFactor). */
    void setFlowWord(quint32 word);
    void stopMotor();
    void pollPressure();
    void sendCmd(quint8 cmd, quint32 arg);

signals:
    void connectedChanged(bool on);
    void pressureRaw(quint32 raw);
    void cmdEcho(quint8 cmd, quint32 arg);
    void errorText(const QString &text);

private slots:
    void onReadyRead();

private:
    void send(const QByteArray &frame);

    HmiSerialPort m_port;
    QByteArray m_rx;
};

#endif
