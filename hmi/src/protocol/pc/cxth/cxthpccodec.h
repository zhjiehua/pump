#ifndef PROTOCOL_PC_CXTH_CODEC_H
#define PROTOCOL_PC_CXTH_CODEC_H

#include <QByteArray>
#include <QtGlobal>

namespace CxthPcCodec {

QByteArray encode(quint8 cmd, quint32 arg, quint8 add = 0);
bool decode(const QByteArray &frame, quint8 *cmd, quint32 *arg, quint8 *add);

} // namespace CxthPcCodec

#endif
