#include "lzo.h"
#include "minilzo.h"

#include <QtGlobal>
#include <cstdlib>
#include <cstring>

namespace {

bool g_inited = false;

void appendU32le(QByteArray *out, quint32 n)
{
    char b[4];
    b[0] = char(n);
    b[1] = char(n >> 8);
    b[2] = char(n >> 16);
    b[3] = char(n >> 24);
    out->append(b, 4);
}

quint32 readU32le(const QByteArray &in, int offset)
{
    const uchar *p = reinterpret_cast<const uchar *>(in.constData() + offset);
    return quint32(p[0]) | (quint32(p[1]) << 8) | (quint32(p[2]) << 16) | (quint32(p[3]) << 24);
}

} // namespace

namespace lzo {

void init()
{
    if (g_inited)
        return;
    if (lzo_init() != LZO_E_OK)
        qWarning("lzo init failed");
    else
        g_inited = true;
}

QByteArray compress(const QByteArray &data)
{
    init();
    if (!g_inited)
        return QByteArray();

    QByteArray out;
    appendU32le(&out, quint32(data.size()));
    if (data.isEmpty())
        return out;

    lzo_uint outLen = lzo_uint(data.size() + data.size() / 16 + 64 + 3);
    unsigned char *buf = static_cast<unsigned char *>(std::malloc(outLen));
    lzo_voidp wrkmem = static_cast<lzo_voidp>(std::malloc(LZO1X_1_MEM_COMPRESS));
    if (!buf || !wrkmem)
    {
        std::free(buf);
        std::free(wrkmem);
        return QByteArray();
    }

    const int r = lzo1x_1_compress(reinterpret_cast<const unsigned char *>(data.constData()),
                                   lzo_uint(data.size()), buf, &outLen, wrkmem);
    if (r == LZO_E_OK)
        out.append(reinterpret_cast<const char *>(buf), int(outLen));
    else
        out.clear();

    std::free(buf);
    std::free(wrkmem);
    return out;
}

QByteArray uncompress(const QByteArray &data)
{
    init();
    if (!g_inited || data.size() < 4)
        return QByteArray();

    const quint32 origLen = readU32le(data, 0);
    if (origLen == 0)
        return QByteArray();
    if (origLen > 32u * 1024u * 1024u)
        return QByteArray();

    const unsigned char *src = reinterpret_cast<const unsigned char *>(data.constData() + 4);
    const lzo_uint srcLen = lzo_uint(data.size() - 4);
    QByteArray out;
    out.resize(int(origLen));
    lzo_uint dstLen = origLen;
    const int r = lzo1x_decompress_safe(src, srcLen,
                                        reinterpret_cast<unsigned char *>(out.data()), &dstLen,
                                        NULL);
    if (r != LZO_E_OK || dstLen != origLen)
        return QByteArray();
    return out;
}

} // namespace lzo
