#include "protocol/mcu/cxth/cxthmcucodec.h"
#include <QVector>

namespace CxthMcu {

QByteArray encodeCmd(quint8 cmd, quint32 arg)
{
    QByteArray out(kFrameSize, char(0));
    out[0] = char(cmd | kCmdHead);
    out[1] = char((arg >> 21) & 0x7f);
    out[2] = char((arg >> 14) & 0x7f);
    out[3] = char((arg >> 7) & 0x7f);
    out[4] = char((arg >> 0) & 0x7f);
    return out;
}

Frame decodeFrame(const QByteArray &five)
{
    Frame f;
    if (five.size() < kFrameSize)
        return f;
    const auto *p = reinterpret_cast<const quint8 *>(five.constData());
    if ((p[0] & kCmdHead) == 0)
        return f;
    f.cmd = quint8(p[0] & 0x0f);
    quint32 arg = p[1] & 0x7f;
    arg = (arg << 7) | (p[2] & 0x7f);
    arg = (arg << 7) | (p[3] & 0x7f);
    arg = (arg << 7) | (p[4] & 0x7f);
    f.arg = arg;
    f.ok = true;
    return f;
}

QVector<Frame> feed(QByteArray *rx, const QByteArray &chunk)
{
    QVector<Frame> out;
    if (!rx)
        return out;
    rx->append(chunk);

    while (!rx->isEmpty())
    {
        int start = -1;
        for (int i = 0; i < rx->size(); ++i)
        {
            if (quint8(rx->at(i)) & kCmdHead)
            {
                start = i;
                break;
            }
        }
        if (start < 0)
        {
            rx->clear();
            break;
        }
        if (start > 0)
            rx->remove(0, start);
        if (rx->size() < kFrameSize)
            break;

        bool bodyOk = true;
        for (int i = 1; i < kFrameSize; ++i)
        {
            if (quint8(rx->at(i)) & kCmdHead)
            {
                bodyOk = false;
                rx->remove(0, i);
                break;
            }
        }
        if (!bodyOk)
            continue;

        Frame f = decodeFrame(rx->left(kFrameSize));
        rx->remove(0, kFrameSize);
        if (f.ok)
            out.append(f);
    }
    return out;
}

} // namespace CxthMcu
