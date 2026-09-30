#ifndef PROTOCOL_MCU_QINFINE_CODEC_H
#define PROTOCOL_MCU_QINFINE_CODEC_H

#include <QByteArray>
#include <QtGlobal>

namespace QinFine {

constexpr char kHead = ':';
constexpr char kTail = '!';
constexpr char kAck = '#';
constexpr char kNack = '$';
constexpr quint8 kSetBit = 0x80;

constexpr quint8 PFC_FLOW = 0x50;
constexpr quint8 PFC_PERCENT = 0x51;
constexpr quint8 PFC_PMIN = 0x52;
constexpr quint8 PFC_PMAX = 0x53;
constexpr quint8 PFC_START_STOP = 0x55;
constexpr quint8 PFC_PAUSE = 0x56;
constexpr quint8 PFC_PURGE = 0x57;
constexpr quint8 PFC_PRESS_ZERO = 0x5A;
constexpr quint8 PFC_PRESS_COMPEN = 0x5C;
constexpr quint8 PFC_PRESS = 0x5E;
constexpr quint8 PFC_EXT_SYSTEM = 0x6E;
constexpr quint8 PFC_TICK = 0x0A;

constexpr quint8 PES_WORKMODE = 0x00;
constexpr quint8 PES_FLOW_CMD = 0x01;
constexpr quint8 PES_FLOW_DATA = 0x02;
constexpr quint8 PES_LOAD_FLOW = 0x03;
constexpr quint8 PES_LOAD_REAL = 0x04;
constexpr quint8 PES_LOAD_PRESS = 0x05;
constexpr quint8 PES_PRESS_CMD = 0x06;
constexpr quint8 PES_PRESS_DATA = 0x07;
constexpr quint8 PES_PULSE_CMD = 0x08;
constexpr quint8 PES_PULSE_DATA = 0x09;

constexpr quint8 TBL_END = 0;
constexpr quint8 TBL_BEGIN = 1;
constexpr quint8 TBL_SAVE = 2;
constexpr quint8 TBL_CLEAR = 3;

constexpr quint8 WORK_FLOWCALIB = 0x01;
constexpr quint8 WORK_PRESSCALIB = 0x02;
constexpr quint8 WORK_PULSECOMPEN = 0x08;

QByteArray encode(quint8 addr, quint8 pfc, const QByteArray &payload);
QByteArray encodeU8(quint8 addr, quint8 pfc, quint8 v);
QByteArray encodeFloat(quint8 addr, quint8 pfc, float v);
QByteArray encodeExtU8(quint8 addr, quint8 sub, quint8 v);
QByteArray encodeExtU8U8(quint8 addr, quint8 sub, quint8 a, quint8 b);
QByteArray encodeExtFloat(quint8 addr, quint8 sub, float v);
QByteArray encodeExt2Float(quint8 addr, quint8 sub, float a, float b);
QByteArray encodeGet(quint8 addr, quint8 pfc);
QByteArray encodeExtGet(quint8 addr, quint8 sub);

struct Frame {
    quint8 addr = 0;
    quint8 pfc = 0;
    QByteArray data;
    bool ok = false;
};

QByteArray hexToAscii(const QByteArray &hex);
QByteArray asciiToHex(const QByteArray &ascii);
Frame decode(const QByteArray &asciiInnerWithCrc);
QByteArray floatBytes(float v);
float beFloat(const QByteArray &data, int off = 0);
quint8 beU8(const QByteArray &data, int off = 0);

} // namespace QinFine

#endif
