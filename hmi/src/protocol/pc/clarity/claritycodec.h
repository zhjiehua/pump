#ifndef PROTOCOL_PC_CLARITY_CODEC_H
#define PROTOCOL_PC_CLARITY_CODEC_H

#include <QByteArray>
#include <QtGlobal>

namespace ClarityCodec {

QByteArray encode(quint8 machineCode, quint8 ai, quint8 pfc, quint32 value);
bool decode(const QByteArray &frame, quint8 *id, quint8 *ai, quint8 *pfc, quint32 *value);
QByteArray encodeAck(bool ok);

} // namespace ClarityCodec

#endif
