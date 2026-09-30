#ifndef PROTOCOL_PC_CLARITY_IDS_H
#define PROTOCOL_PC_CLARITY_IDS_H

#include <QtGlobal>

namespace ClarityPc {

constexpr quint8 STX = 0x21;
constexpr quint8 ETX = 0x0A;
constexpr quint8 ACK = 0x23;
constexpr quint8 NAK = 0x24;

constexpr quint8 PFC_READ_ID = 0x01;
constexpr quint8 PFC_LICENSE_H = 0x02;
constexpr quint8 PFC_LICENSE_L = 0x03;
constexpr quint8 PFC_STATUS = 0x04;
constexpr quint8 PFC_SET_FLOW = 0x10;
constexpr quint8 PFC_SET_PERCENT = 0x11;
constexpr quint8 PFC_SYNCTIME = 0x12;
constexpr quint8 PFC_MAX_PRESS = 0x13;
constexpr quint8 PFC_MIN_PRESS = 0x14;
constexpr quint8 PFC_START = 0x15;
constexpr quint8 PFC_STOP = 0x16;
constexpr quint8 PFC_PRESSCLEAR = 0x17;
constexpr quint8 PFC_READ_PRESS = 0x18;
constexpr quint8 PFC_PURGE = 0x19;
constexpr quint8 PFC_HOLD = 0x1A;
constexpr quint8 PFC_SEND_PRESS = 0x90;
constexpr quint8 PFC_EXT = 0x41;
constexpr quint8 PFC_PRESS_COMPEN = 0x44;

} // namespace ClarityPc

#endif
