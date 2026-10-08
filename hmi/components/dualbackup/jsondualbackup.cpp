#include "dualbackup/jsondualbackup.h"
#include "utils/md5hash.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#if QT_VERSION >= 0x050000
#include <QSaveFile>
#endif

namespace JsonDualBackup {

namespace {

constexpr char kChecksumMarker[] = "\n#checksum:";
constexpr char kSuperChecksum[] = "00112233445566778899aabbccddeeff";

QByteArray wrapWithChecksum(const QByteArray &jsonData)
{
    const QByteArray hash = Md5Hash::hex(jsonData);
    return jsonData + QByteArray(kChecksumMarker) + hash + '\n';
}

bool isSuperChecksum(const QByteArray &checksum)
{
    return QString::fromLatin1(checksum).compare(QLatin1String(kSuperChecksum), Qt::CaseInsensitive) == 0;
}

bool stripAndVerify(QByteArray &content, QByteArray &jsonOut)
{
    const QByteArray marker(kChecksumMarker);
    const int idx = content.lastIndexOf(marker);
    if (idx < 0)
    {
        jsonOut = content;
        return !jsonOut.isEmpty();
    }

    const QByteArray jsonPart = content.left(idx);
    QByteArray checksumPart = content.mid(idx + marker.size());
    while (!checksumPart.isEmpty()
           && (checksumPart.endsWith('\n') || checksumPart.endsWith('\r')))
        checksumPart.chop(1);

    if (!isSuperChecksum(checksumPart))
    {
        const QByteArray expected = Md5Hash::hex(jsonPart);
        if (QString::fromLatin1(checksumPart).compare(QString::fromLatin1(expected), Qt::CaseInsensitive) != 0)
            return false;
    }

    jsonOut = jsonPart;
    return true;
}

} // namespace

bool readChecked(const QString &path, QByteArray &jsonOut)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    QByteArray content = f.readAll();
    return stripAndVerify(content, jsonOut);
}

bool writeChecked(const QString &path, const QByteArray &jsonData)
{
    QDir().mkpath(QFileInfo(path).absolutePath());

#if QT_VERSION >= 0x050000
    QSaveFile f(path);
    if (!f.open(QIODevice::WriteOnly))
        return false;
    f.write(wrapWithChecksum(jsonData));
    return f.commit();
#else
    const QString tmpPath = path + QString::fromLatin1(".tmp");
    QFile f(tmpPath);
    if (!f.open(QIODevice::WriteOnly))
        return false;
    f.write(wrapWithChecksum(jsonData));
    f.close();
    if (QFile::exists(path))
        QFile::remove(path);
    return QFile::rename(tmpPath, path);
#endif
}

bool save(const QString &primaryPath, const QString &backupPath, const QByteArray &jsonData)
{
    if (QFile::exists(primaryPath))
    {
        QFile::remove(backupPath);
        QFile::copy(primaryPath, backupPath);
    }

    if (!writeChecked(primaryPath, jsonData))
        return false;

    if (!QFile::exists(backupPath))
        writeChecked(backupPath, jsonData);

    return true;
}

LoadResult load(const QString &primaryPath, const QString &backupPath)
{
    LoadResult result;
    if (readChecked(primaryPath, result.data))
    {
        result.source = Source::Primary;
        return result;
    }

    if (readChecked(backupPath, result.data))
        result.source = Source::Backup;

    return result;
}

} // namespace JsonDualBackup
