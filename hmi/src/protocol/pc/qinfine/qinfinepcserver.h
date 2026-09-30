#ifndef PROTOCOL_PC_QINFINE_SERVER_H
#define PROTOCOL_PC_QINFINE_SERVER_H

#include "protocol/pc/pcserver.h"
#include "protocol/mcu/qinfine/qinfinecodec.h"

class QinFinePcServer : public PcServer
{
public:
    explicit QinFinePcServer(QObject *parent = nullptr);

    void sendPressure(double mpa) override;
    void dumpFlowTable();
    void dumpPressTable();
    void dumpPulseTable();
    void sendLoadFloat(quint8 sub, float v);

protected:
    void handle(const QByteArray &chunk) override;

private:
    quint8 addr() const;
    void ack(bool ok);
    void handleFrame(const QinFine::Frame &f);
    void handlePfc(quint8 pfc, bool isSet, const QByteArray &data);
    void handleExt(bool isSet, const QByteArray &data);
    void sendFloat(quint8 pfc, float v);
    void sendU8(quint8 pfc, quint8 v);
    void sendExtFloat(quint8 sub, float v);
    void sendExtPoint(quint8 sub, float a, float b);
};

#endif
