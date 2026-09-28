#ifndef CRC16_H
#define CRC16_H

#include <QtGlobal>
#include <QByteArray>

quint16 crc16Modbus(const QByteArray &data);
bool crc16Ok(const QByteArray &dataWithCrc);

#endif
