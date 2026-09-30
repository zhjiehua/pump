#ifndef PROTOCOL_PC_CLARITY_SERVER_H
#define PROTOCOL_PC_CLARITY_SERVER_H

#include "protocol/pc/pcserver.h"

class ClarityPcServer : public PcServer
{
public:
    explicit ClarityPcServer(QObject *parent = nullptr);

    void sendPressure(double mpa) override;

protected:
    void handle(const QByteArray &chunk) override;

private:
    void handleFrame(const QByteArray &frame);
};

#endif
