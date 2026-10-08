#include "utils/md5hash.h"

extern "C" {
#include "md5.h"
}

QByteArray Md5Hash::digest(const QByteArray &data)
{
    MD5_CTX ctx;
    MD5Init(&ctx);
    if (!data.isEmpty())
    {
        MD5Update(&ctx, const_cast<unsigned char *>(
                            reinterpret_cast<const unsigned char *>(data.constData())),
                  unsigned(data.size()));
    }
    unsigned char out[16];
    MD5Final(&ctx, out);
    return QByteArray(reinterpret_cast<const char *>(out), 16);
}

QByteArray Md5Hash::hex(const QByteArray &data)
{
    return digest(data).toHex();
}
