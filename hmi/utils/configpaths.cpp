#include "configpaths.h"

#include <QCoreApplication>
#include <QDir>

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif

namespace {

QString ensureSubdir(const QString &name)
{
    const QString dir = QDir(ConfigPaths::writableAppConfigDir()).filePath(name);
    QDir().mkpath(dir);
    return dir;
}

} // namespace

QString ConfigPaths::writableAppConfigDir()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    if (!appDir.isEmpty() && QDir(appDir).exists())
        return appDir;
#if QT_VERSION >= 0x050000
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
#else
    return QDir::homePath() + QString::fromLatin1("/.pump");
#endif
}

QString ConfigPaths::dataDir()
{
    return ensureSubdir(QStringLiteral("data"));
}

QString ConfigPaths::recordsDir()
{
    return ensureSubdir(QStringLiteral("records"));
}
