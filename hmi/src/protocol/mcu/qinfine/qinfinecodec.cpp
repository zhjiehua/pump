#include "protocol/mcu/qinfine/qinfinecodec.h"
#include "utils/crc16.h"
#include <limits>
#include <QtEndian>
#include <cstring>

namespace QinFine {

QByteArray floatBytes(float v)
{
    quint32 bits = 0;
    static_assert(sizeof(float) == 4, "float");
    std::memcpy(&bits, &v, 4);
    bits = qToBigEndian(bits);
    QByteArray out(4, 0);
    std::memcpy(out.data(), &bits, 4);
    return out;
}

QByteArray hexToAscii(const QByteArray &hex)
{
    static const char *digits = "0123456789ABCDEF";
    QByteArray out;
    out.resize(hex.size() * 2);
    for (int i = 0; i < hex.size(); ++i)
    {
        const unsigned char b = static_cast<unsigned char>(hex.at(i));
        out[i * 2] = digits[b >> 4];
        out[i * 2 + 1] = digits[b & 0x0F];
    }
    return out;
}

QByteArray asciiToHex(const QByteArray &ascii)
{
    auto nibble = [](char c) -> int {
        if (c >= '0' && c <= '9')
            return c - '0';
        if (c >= 'A' && c <= 'F')
            return c - 'A' + 10;
        if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        return -1;
    };
    if (ascii.size() % 2)
        return {};
    QByteArray out;
    out.resize(ascii.size() / 2);
    for (int i = 0; i < out.size(); ++i)
    {
        const int hi = nibble(ascii.at(i * 2));
        const int lo = nibble(ascii.at(i * 2 + 1));
        if (hi < 0 || lo < 0)
            return {};
        out[i] = char((hi << 4) | lo);
    }
    return out;
}

QByteArray encode(quint8 addr, quint8 pfc, const QByteArray &payload)
{
    QByteArray hex;
    hex.append(char(addr));
    hex.append(char(pfc));
    hex.append(payload);
    const quint16 crc = crc16Modbus(hex);
    hex.append(char(crc >> 8));
    hex.append(char(crc & 0xFF));
    QByteArray frame;
    frame.append(kHead);
    frame.append(hexToAscii(hex));
    frame.append(kTail);
    return frame;
}

QByteArray encodeU8(quint8 addr, quint8 pfc, quint8 v)
{
    return encode(addr, pfc | kSetBit, QByteArray(1, char(v)));
}

QByteArray encodeFloat(quint8 addr, quint8 pfc, float v)
{
    return encode(addr, pfc | kSetBit, floatBytes(v));
}

QByteArray encodeExtU8(quint8 addr, quint8 sub, quint8 v)
{
    QByteArray p;
    p.append(char(sub));
    p.append(char(v));
    return encode(addr, PFC_EXT_SYSTEM | kSetBit, p);
}

QByteArray encodeExtU8U8(quint8 addr, quint8 sub, quint8 a, quint8 b)
{
    QByteArray p;
    p.append(char(sub));
    p.append(char(a));
    p.append(char(b));
    return encode(addr, PFC_EXT_SYSTEM | kSetBit, p);
}

QByteArray encodeExtFloat(quint8 addr, quint8 sub, float v)
{
    QByteArray p;
    p.append(char(sub));
    p.append(floatBytes(v));
    return encode(addr, PFC_EXT_SYSTEM | kSetBit, p);
}

QByteArray encodeExt2Float(quint8 addr, quint8 sub, float a, float b)
{
    QByteArray p;
    p.append(char(sub));
    p.append(floatBytes(a));
    p.append(floatBytes(b));
    return encode(addr, PFC_EXT_SYSTEM | kSetBit, p);
}

QByteArray encodeGet(quint8 addr, quint8 pfc)
{
    return encode(addr, pfc, {});
}

QByteArray encodeExtGet(quint8 addr, quint8 sub)
{
    return encode(addr, PFC_EXT_SYSTEM, QByteArray(1, char(sub)));
}

Frame decode(const QByteArray &asciiInnerWithCrc)
{
    Frame f;
    const QByteArray hex = asciiToHex(asciiInnerWithCrc);
    if (hex.size() < 4 || !crc16Ok(hex))
        return f;
    f.addr = quint8(hex.at(0));
    f.pfc = quint8(hex.at(1));
    f.data = hex.mid(2, hex.size() - 4);
    f.ok = true;
    return f;
}

float beFloat(const QByteArray &data, int off)
{
    if (data.size() < off + 4)
        return 0;
    quint32 bits = 0;
    std::memcpy(&bits, data.constData() + off, 4);
    bits = qFromBigEndian(bits);
    float v = 0;
    std::memcpy(&v, &bits, 4);
    return v;
}

quint8 beU8(const QByteArray &data, int off)
{
    if (data.size() <= off)
        return 0;
    return quint8(data.at(off));
}

} // namespace QinFine
