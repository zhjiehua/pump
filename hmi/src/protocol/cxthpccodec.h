#ifndef CXTHPCCODEC_H
#define CXTHPCCODEC_H

#include <QByteArray>
#include <QtGlobal>

namespace CxthPcCodec {

QByteArray encodeClarity(quint8 machineCode, quint8 ai, quint8 pfc, quint32 value);
bool decodeClarity(const QByteArray &frame, quint8 *id, quint8 *ai, quint8 *pfc, quint32 *value);

QByteArray encodeLegacy(quint8 cmd, quint32 arg, quint8 add = 0);
bool decodeLegacy(const QByteArray &frame, quint8 *cmd, quint32 *arg, quint8 *add);

QByteArray encodeAck(bool ok);

} // namespace CxthPcCodec

#endif
