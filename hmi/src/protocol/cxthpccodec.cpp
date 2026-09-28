#include "protocol/cxthpccodec.h"
#include "protocol/cxthpcids.h"

namespace CxthPcCodec {

static QByteArray hexAscii(quint32 v, int width)
{
    return QByteArray::number(v, 16).toUpper().rightJustified(width, '0');
}

static QByteArray decAsciiPad(quint32 v, int width, bool spacePad)
{
    QByteArray s = QByteArray::number(v);
    if (s.size() > width)
        s = s.right(width);
    if (spacePad)
        return s.rightJustified(width, ' ');
    return s.rightJustified(width, '0');
}

QByteArray encodeClarity(quint8 machineCode, quint8 ai, quint8 pfc, quint32 value)
{
    QByteArray f(16, char(0));
    f[0] = char(CxthPc::STX);
    f.replace(1, 2, hexAscii(machineCode, 2));
    f.replace(3, 1, hexAscii(ai, 1));
    f.replace(4, 2, hexAscii(pfc, 2));
    f.replace(6, 6, decAsciiPad(value, 6, true));
    unsigned char sum = 0;
    for (int i = 0; i < 12; ++i)
        sum += static_cast<unsigned char>(f.at(i));
    f.replace(12, 3, QByteArray::number(int(sum)).rightJustified(3, '0'));
    f[15] = char(CxthPc::ETX);
    return f;
}

bool decodeClarity(const QByteArray &frame, quint8 *id, quint8 *ai, quint8 *pfc, quint32 *value)
{
    if (frame.size() != 16)
        return false;
    if (quint8(frame.at(0)) != CxthPc::STX || quint8(frame.at(15)) != CxthPc::ETX)
        return false;
    bool ok = false;
    const quint32 machine = frame.mid(1, 2).toUInt(&ok, 16);
    if (!ok)
        return false;
    const quint32 aiV = frame.mid(3, 1).toUInt(&ok, 16);
    if (!ok)
        return false;
    const quint32 pfcV = frame.mid(4, 2).toUInt(&ok, 16);
    if (!ok)
        return false;
    const QByteArray valBytes = frame.mid(6, 6).trimmed();
    const quint32 val = valBytes.toUInt(&ok, 10);
    if (!ok && !valBytes.isEmpty())
        return false;
    unsigned char sum = 0;
    for (int i = 0; i < 12; ++i)
        sum += static_cast<unsigned char>(frame.at(i));
    const quint32 recv = frame.mid(12, 3).toUInt(&ok, 10);
    if (!ok || (recv & 0xFF) != sum)
        return false;
    if (id)
        *id = quint8(machine);
    if (ai)
        *ai = quint8(aiV);
    if (pfc)
        *pfc = quint8(pfcV);
    if (value)
        *value = val;
    return true;
}

QByteArray encodeLegacy(quint8 cmd, quint32 arg, quint8 add)
{
    QByteArray f(5, char(0));
    f[0] = char(0x80 | (cmd & 0x0F));
    f[1] = char(add & 0x7F);
    f[2] = char((arg >> 14) & 0x7F);
    f[3] = char((arg >> 7) & 0x7F);
    f[4] = char(arg & 0x7F);
    return f;
}

bool decodeLegacy(const QByteArray &frame, quint8 *cmd, quint32 *arg, quint8 *add)
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

QByteArray encodeAck(bool ok)
{
    return QByteArray(1, char(ok ? CxthPc::ACK : CxthPc::NAK));
}

} // namespace CxthPcCodec
