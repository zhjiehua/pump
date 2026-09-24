#include "core/i18nmanager.h"
#include "core/appsettings.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

I18nManager::I18nManager(AppSettings *settings, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
{
}

bool I18nManager::applyLanguage(int lang)
{
    QApplication *app = qobject_cast<QApplication *>(QCoreApplication::instance());
    if (!app)
        return false;

    app->removeTranslator(&m_translator);
    if (lang == int(AppSettings::Chinese))
    {
        const QString base = QCoreApplication::applicationDirPath();
        const QStringList paths = {
            QDir(base).filePath(QStringLiteral("hmi_zh.qm")),
            QDir(base).filePath(QStringLiteral("translations/hmi_zh.qm")),
            QStringLiteral(":/translations/hmi_zh.qm"),
        };
        for (const QString &p : paths)
        {
            if (m_translator.load(p))
            {
                app->installTranslator(&m_translator);
                break;
            }
        }
    }
    if (m_settings)
    {
        m_settings->language = AppSettings::Language(lang);
        m_settings->save();
    }
    return true;
}

bool I18nManager::applyFromSettings()
{
    if (!m_settings)
        return false;
    return applyLanguage(int(m_settings->language));
}
