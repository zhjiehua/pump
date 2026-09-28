#ifndef CXTHPCIDS_H
#define CXTHPCIDS_H

#include <QtGlobal>

namespace CxthPc {

constexpr quint8 STX = 0x21;
constexpr quint8 ETX = 0x0A;
constexpr quint8 ACK = 0x23;
constexpr quint8 NAK = 0x24;

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

constexpr quint8 PFCC_READ_ID = 0x01;
constexpr quint8 PFCC_LICENSE_H = 0x02;
constexpr quint8 PFCC_LICENSE_L = 0x03;
constexpr quint8 PFCC_STATUS = 0x04;
constexpr quint8 PFCC_SET_FLOW = 0x10;
constexpr quint8 PFCC_SET_PERCENT = 0x11;
constexpr quint8 PFCC_SYNCTIME = 0x12;
constexpr quint8 PFCC_MAX_PRESS = 0x13;
constexpr quint8 PFCC_MIN_PRESS = 0x14;
constexpr quint8 PFCC_START = 0x15;
constexpr quint8 PFCC_STOP = 0x16;
constexpr quint8 PFCC_PRESSCLEAR = 0x17;
constexpr quint8 PFCC_READ_PRESS = 0x18;
constexpr quint8 PFCC_PURGE = 0x19;
constexpr quint8 PFCC_HOLD = 0x1A;
constexpr quint8 PFCC_SEND_PRESS = 0x90;
constexpr quint8 PFCC_EXT = 0x41;
constexpr quint8 PFCC_PRESS_COMPEN = 0x44;

constexpr quint8 AI_WORKMODE = 0x00;
constexpr quint8 AI_FLOW_CMD = 0x01;
constexpr quint8 AI_FLOW_A = 0x02;
constexpr quint8 AI_FLOW_B = 0x22;
constexpr quint8 AI_LOAD_FLOW = 0x03;
constexpr quint8 AI_LOAD_REAL = 0x04;
constexpr quint8 AI_LOAD_PRESS = 0x05;
constexpr quint8 AI_PRESS_CMD = 0x06;
constexpr quint8 AI_PRESS_A = 0x07;
constexpr quint8 AI_PRESS_B = 0x27;
constexpr quint8 AI_PULSE_CMD = 0x08;
constexpr quint8 AI_PULSE_A = 0x09;
constexpr quint8 AI_PULSE_B = 0x29;
constexpr quint8 AI_PRESS_COMPEN = 0x0C;

constexpr quint8 CALIB_END = 0;
constexpr quint8 CALIB_BEGIN = 1;
constexpr quint8 CALIB_SAVE = 2;
constexpr quint8 CALIB_CLEAR = 3;
constexpr quint8 CALIB_DUMP = 4;

/** Integer counts (rpm, adc, pulse position). Fits the 6-digit CDS field. */
inline quint32 packCount(double v)
{
    if (v < 0)
        v = 0;
    if (v > 999999.0)
        v = 999999.0;
    return quint32(v + 0.5);
}

inline double unpackCount(quint32 v)
{
    return double(v);
}

/** Milli-units for rates, MPa, and factors. */
inline quint32 packMilli(double v)
{
    double s = v * 1000.0;
    if (s < 0)
        s = 0;
    if (s > 999999.0)
        s = 999999.0;
    return quint32(s + 0.5);
}

inline double unpackMilli(quint32 v)
{
    return double(v) / 1000.0;
}

} // namespace CxthPc

#endif
