#include "utils/securepack.h"

extern "C" {
#include "miniaes.h"
}

#include "lzo.h"

#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <cstring>

namespace {

QMutex g_aesMutex;

// Derived at runtime so the binary does not contain a plaintext AES key.
const uint8_t kPrimers[32] = {
    0xa1, 0x5c, 0x27, 0x9e, 0x44, 0x08, 0xd3, 0x6b,
    0xf2, 0x11, 0x8a, 0xc0, 0x3d, 0x77, 0xe5, 0x19,
    0x62, 0xb4, 0x0f, 0x98, 0x2c, 0xde, 0x51, 0x87,
    0x13, 0x46, 0xab, 0x70, 0x9d, 0x04, 0xee, 0x38
};

uint8_t g_key[16];
uint8_t g_iv[16];
bool g_ready = false;

void ensureKey()
{
    if (g_ready)
        return;
    for (int i = 0; i < 16; ++i)
    {
        g_key[i] = uint8_t(kPrimers[i * 2] + (17 + i));
        g_iv[i] = uint8_t(kPrimers[i * 2 + 1] + (17 + i));
    }
    g_ready = true;
}

QByteArray aesCtr(const QByteArray &in)
{
    QMutexLocker lock(&g_aesMutex);
    ensureKey();
    QByteArray copy = in;
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, g_key, g_iv);
    AES_CTR_xcrypt_buffer(&ctx, reinterpret_cast<uint8_t *>(copy.data()),
                          size_t(copy.size()));
    return copy;
}

QByteArray pkcs7Pad(const QByteArray &in)
{
    int padding = 16 - (in.size() % 16);
    if (padding == 0)
        padding = 16;
    QByteArray out = in;
    out.append(QByteArray(padding, char(padding)));
    return out;
}

bool pkcs7Unpad(QByteArray *data)
{
    if (!data || data->isEmpty())
        return false;
    const int padding = int(uchar(data->at(data->size() - 1)));
    if (padding < 1 || padding > 16 || padding > data->size())
        return false;
    for (int i = 0; i < padding; ++i)
    {
        if (uchar(data->at(data->size() - 1 - i)) != uchar(padding))
            return false;
    }
    data->chop(padding);
    return true;
}

} // namespace

namespace SecurePack {

bool pack(const char magic[8], const QByteArray &plain, QByteArray *out)
{
    if (!magic || !out)
        return false;
    const QByteArray compressed = lzo::compress(plain);
    if (compressed.isEmpty())
        return false;
    const QByteArray encrypted = aesCtr(pkcs7Pad(compressed));
    out->clear();
    out->append(magic, 8);
    out->append(encrypted);
    return true;
}

bool unpack(const QByteArray &blob, const char magic[8], QByteArray *plain)
{
    if (!magic || !plain || blob.size() < 8 + 16)
        return false;
    if (std::memcmp(blob.constData(), magic, 8) != 0)
        return false;
    QByteArray decrypted = aesCtr(blob.mid(8));
    if (!pkcs7Unpad(&decrypted))
        return false;
    *plain = lzo::uncompress(decrypted);
    if (!plain->isEmpty())
        return true;
    if (decrypted.size() < 4)
        return false;
    const quint32 origLen = quint32(uchar(decrypted.at(0)))
                            | (quint32(uchar(decrypted.at(1))) << 8)
                            | (quint32(uchar(decrypted.at(2))) << 16)
                            | (quint32(uchar(decrypted.at(3))) << 24);
    return origLen == 0;
}

bool writeFile(const QString &path, const QByteArray &blob)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    return f.write(blob) == blob.size();
}

bool readFile(const QString &path, QByteArray *blob)
{
    if (!blob)
        return false;
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return false;
    *blob = f.readAll();
    return !blob->isEmpty();
}

} // namespace SecurePack
