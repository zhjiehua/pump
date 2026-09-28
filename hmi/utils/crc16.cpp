#include "utils/crc16.h"

quint16 crc16Modbus(const QByteArray &data)
{
    quint16 crc = 0xFFFF;
    for (unsigned char b : data)
    {
        crc ^= b;
        for (int i = 0; i < 8; ++i)
        {
            const quint16 carry = crc & 0x0001;
            crc >>= 1;
            if (carry)
                crc ^= 0xA001;
        }
    }
    return crc;
}

bool crc16Ok(const QByteArray &dataWithCrc)
{
    if (dataWithCrc.size() < 2)
        return false;
    const QByteArray body = dataWithCrc.left(dataWithCrc.size() - 2);
    const auto *p = reinterpret_cast<const unsigned char *>(dataWithCrc.constData());
    const int n = dataWithCrc.size();
    const quint16 got = (quint16(p[n - 2]) << 8) | p[n - 1];
    return got == crc16Modbus(body);
}
