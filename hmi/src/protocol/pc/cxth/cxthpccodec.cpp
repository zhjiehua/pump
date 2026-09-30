#include "protocol/pc/cxth/cxthpccodec.h"

namespace CxthPcCodec {

QByteArray encode(quint8 cmd, quint32 arg, quint8 add)
{
    QByteArray f(5, char(0));
    f[0] = char(0x80 | (cmd & 0x0F));
    f[1] = char(add & 0x7F);
    f[2] = char((arg >> 14) & 0x7F);
    f[3] = char((arg >> 7) & 0x7F);
    f[4] = char(arg & 0x7F);
    return f;
}

bool decode(const QByteArray &frame, quint8 *cmd, quint32 *arg, quint8 *add)
{
    if (frame.size() < 4)
        return false;
    if ((quint8(frame.at(0)) & 0x80) == 0)
        return false;
    const quint8 c = quint8(frame.at(0)) & 0x0F;
    quint8 extra = 0;
    quint32 a = 0;
    if (frame.size() >= 5)
    {
        extra = quint8(frame.at(1)) & 0x7F;
        a = (quint32(frame.at(2) & 0x7F) << 14)
            | (quint32(frame.at(3) & 0x7F) << 7)
            | quint32(frame.at(4) & 0x7F);
    }
    else
    {
        a = (quint32(frame.at(1) & 0x3F) << 14)
            | (quint32(frame.at(2) & 0x7F) << 7)
            | quint32(frame.at(3) & 0x7F);
    }
    if (cmd)
        *cmd = c;
    if (arg)
        *arg = a;
    if (add)
        *add = extra;
    return true;
}

} // namespace CxthPcCodec
