#ifndef SECUREPACK_H
#define SECUREPACK_H

#include <QByteArray>
#include <QString>

/** Compress (zlib) then AES-128-CTR encrypt a payload with an 8-byte magic. */
namespace SecurePack {

bool pack(const char magic[8], const QByteArray &plain, QByteArray *out);
bool unpack(const QByteArray &blob, const char magic[8], QByteArray *plain);

bool writeFile(const QString &path, const QByteArray &blob);
bool readFile(const QString &path, QByteArray *blob);

} // namespace SecurePack

#endif
