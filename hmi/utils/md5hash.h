#ifndef MD5HASH_H
#define MD5HASH_H

#include <QByteArray>

/** Bundled MD5 (Qt 4.8 / embedded may lack QCryptographicHash). */
namespace Md5Hash {
QByteArray digest(const QByteArray &data);
QByteArray hex(const QByteArray &data);
}

#endif
