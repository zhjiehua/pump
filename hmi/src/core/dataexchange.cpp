#include "core/dataexchange.h"

#include "core/appsettings.h"
#include "core/recordstore.h"
#include "log/log.h"
#include "utils/configpaths.h"
#include "utils/hmiconfig.h"
#include "utils/qjsonshim.h"
#include "utils/securepack.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace {

const char kMagicConfig[8] = {'P', 'U', 'M', 'P', 'C', 'F', 'G', '1'};
const char kMagicRecords[8] = {'P', 'U', 'M', 'P', 'R', 'E', 'C', '1'};

bool writePack(const QString &path, const char magic[8], const QByteArray &plain, QString *error)
{
    QByteArray blob;
    if (!SecurePack::pack(magic, plain, &blob))
    {
        if (error)
            *error = QStringLiteral("pack failed");
        return false;
    }
    QDir().mkpath(QFileInfo(path).absolutePath());
    if (!SecurePack::writeFile(path, blob))
    {
        if (error)
            *error = path;
        return false;
    }
    return true;
}

bool readPack(const QString &path, const char magic[8], QByteArray *plain, QString *error)
{
    QByteArray blob;
    if (!SecurePack::readFile(path, &blob))
    {
        if (error)
            *error = path;
        return false;
    }
    if (!SecurePack::unpack(blob, magic, plain))
    {
        if (error)
            *error = QStringLiteral("decrypt failed");
        return false;
    }
    return true;
}

QJsonArray collectLogFiles()
{
    QJsonArray logs;
    const QDir dir(Log::directory());
    QStringList names = dir.entryList(QStringList() << QStringLiteral("pump.log")
                                                    << QStringLiteral("pump.*.log"),
                                      QDir::Files, QDir::Name);
    names.removeDuplicates();
    for (int i = 0; i < names.size(); ++i)
    {
        QFile f(dir.filePath(names.at(i)));
        if (!f.open(QIODevice::ReadOnly))
            continue;
        QJsonObject o;
        o.insert(QStringLiteral("name"), names.at(i));
        o.insert(QStringLiteral("text"), QString::fromUtf8(f.readAll()));
        logs.append(o);
    }
    return logs;
}

} // namespace

namespace DataExchange {

QString exportDir()
{
#if HMI_EMBEDDED
    const QString sd(QStringLiteral("/sdcard"));
    if (!QDir(sd).exists())
        return QString();
    return sd;
#else
    const QString dir = QDir(ConfigPaths::writableAppConfigDir()).filePath(QStringLiteral("export"));
    QDir().mkpath(dir);
    return dir;
#endif
}

QString configPackPath()
{
    return QDir(exportDir()).filePath(QStringLiteral("pump_config.ppk"));
}

QString recordsPackPath()
{
    return QDir(exportDir()).filePath(QStringLiteral("pump_records.ppk"));
}

bool exportConfig(const AppSettings *settings, QString *error)
{
    if (!settings)
        return false;
    const QString dir = exportDir();
    if (dir.isEmpty())
    {
        if (error)
            *error = QStringLiteral("no media");
        return false;
    }
    return writePack(configPackPath(), kMagicConfig, settings->exportBundleJson(), error);
}

bool importConfig(AppSettings *settings, QString *error)
{
    if (!settings)
        return false;
    if (exportDir().isEmpty())
    {
        if (error)
            *error = QStringLiteral("no media");
        return false;
    }
    QByteArray plain;
    if (!readPack(configPackPath(), kMagicConfig, &plain, error))
        return false;
    if (!settings->importBundleJson(plain))
    {
        if (error)
            *error = QStringLiteral("parse failed");
        return false;
    }
    return true;
}

bool exportRecords(const RecordStore *records, QString *error)
{
    if (!records)
        return false;
    const QString dir = exportDir();
    if (dir.isEmpty())
    {
        if (error)
            *error = QStringLiteral("no media");
        return false;
    }

    Log::flush();

    QJsonObject root;
    root.insert(QStringLiteral("kind"), QStringLiteral("records"));
    const QJsonDocument recDoc = QJsonDocument::fromJson(records->exportJson());
    if (recDoc.isObject())
        root.insert(QStringLiteral("records"), recDoc.object());
    root.insert(QStringLiteral("logs"), collectLogFiles());

    return writePack(recordsPackPath(), kMagicRecords,
                     QJsonDocument(root).toJson(QJsonDocument::Compact), error);
}

} // namespace DataExchange
