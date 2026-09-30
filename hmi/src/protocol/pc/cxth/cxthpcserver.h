#ifndef PROTOCOL_PC_CXTH_SERVER_H
#define PROTOCOL_PC_CXTH_SERVER_H

#include "protocol/pc/pcserver.h"

class CxthPcServer : public PcServer
{
public:
    explicit CxthPcServer(QObject *parent = nullptr);

    void sendPressure(double mpa) override;

protected:
    void handle(const QByteArray &chunk) override;

private:
    void handleFrame(const QByteArray &frame);
};

#endif
