#include "protocol/pc/clarity/claritycodec.h"
#include "protocol/pc/clarity/clarityids.h"

namespace ClarityCodec {

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

QByteArray encode(quint8 machineCode, quint8 ai, quint8 pfc, quint32 value)
{
    QByteArray f(16, char(0));
    f[0] = char(ClarityPc::STX);
    f.replace(1, 2, hexAscii(machineCode, 2));
    f.replace(3, 1, hexAscii(ai, 1));
    f.replace(4, 2, hexAscii(pfc, 2));
    f.replace(6, 6, decAsciiPad(value, 6, true));
    unsigned char sum = 0;
    for (int i = 0; i < 12; ++i)
        sum += static_cast<unsigned char>(f.at(i));
    f.replace(12, 3, QByteArray::number(int(sum)).rightJustified(3, '0'));
    f[15] = char(ClarityPc::ETX);
    return f;
}

bool decode(const QByteArray &frame, quint8 *id, quint8 *ai, quint8 *pfc, quint32 *value)
{
    if (frame.size() != 16)
        return false;
    if (quint8(frame.at(0)) != ClarityPc::STX || quint8(frame.at(15)) != ClarityPc::ETX)
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

QByteArray encodeAck(bool ok)
{
    return QByteArray(1, char(ok ? ClarityPc::ACK : ClarityPc::NAK));
}

} // namespace ClarityCodec
