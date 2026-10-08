#ifndef HMI_LZO_H
#define HMI_LZO_H

#include <QByteArray>

/** MiniLZO wrappers. Do not use qCompress — Qt 4.8 / embedded builds may lack zlib. */
namespace lzo {

void init();
QByteArray compress(const QByteArray &data);
QByteArray uncompress(const QByteArray &data);

} // namespace lzo

#endif
