#ifndef LEGACYMCUCODEC_H
#define LEGACYMCUCODEC_H

#include <QByteArray>
#include <QtGlobal>
#include <QVector>

namespace LegacyMcu {

constexpr quint8 kCmdHead = 0x80;
constexpr int kFrameSize = 5;

constexpr quint8 CMD_SET_PARAM = 0x01;
constexpr quint8 CMD_MOTOR_INI = 0x02;
constexpr quint8 CMD_WAVEADD_MOTOR = 0x04;
constexpr quint8 CMD_WAVEDEC_MOTOR = 0x05;
constexpr quint8 CMD_READ_PARAM = 0x09;
constexpr quint8 CMD_READ_VERSION = 0x0a;
constexpr quint8 CMD_READ_AU_VAL = 0x0b;
constexpr quint8 CMD_READ_AU_VALB = 0x0c;

/** Encode 5-byte MCU frame: [cmd|0x80][arg>>21][arg>>14][arg>>7][arg]. */
QByteArray encodeCmd(quint8 cmd, quint32 arg);

struct Frame {
    quint8 cmd = 0;
    quint32 arg = 0;
    bool ok = false;
};

/** Feed bytes; returns completed frames (may be empty). Keeps leftover in *rx. */
QVector<Frame> feed(QByteArray *rx, const QByteArray &chunk);

Frame decodeFrame(const QByteArray &five);

} // namespace LegacyMcu

#endif
