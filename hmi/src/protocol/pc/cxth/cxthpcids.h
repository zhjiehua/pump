#ifndef PROTOCOL_PC_CXTH_IDS_H
#define PROTOCOL_PC_CXTH_IDS_H

#include <QtGlobal>

namespace CxthPc {

constexpr quint8 PFC_START = 0x00;
constexpr quint8 PFC_PURGE = 0x01;
constexpr quint8 PFC_STOP = 0x02;
constexpr quint8 PFC_READ_PRESS = 0x03;
constexpr quint8 PFC_HOLD = 0x05;
constexpr quint8 PFC_SET_FLOW1 = 0x08;
constexpr quint8 PFC_SET_MAXPRESS = 0x09;
constexpr quint8 PFC_SET_MINPRESS = 0x0A;
constexpr quint8 PFC_CALIB = 0x0B;
constexpr quint8 PFC_TIME_SYNC = 0x0E;

} // namespace CxthPc

#endif
