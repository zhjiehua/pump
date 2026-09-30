#include "core/i18nmanager.h"
#include "core/appsettings.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>

I18nManager::I18nManager(AppSettings *settings, QObject *parent)
    : QObject(parent)
    , m_settings(settings)
{
}

bool I18nManager::ensureLoaded()
{
    if (m_loaded)
        return true;

    // Same pattern as weiduodianzi: qm is compiled into the qrc.
    if (m_translator.load(QStringLiteral(":/hmi/translations/hmi_zh.qm"))
        || m_translator.load(QStringLiteral("hmi_zh.qm"), QStringLiteral(":/hmi/translations")))
    {
        m_loaded = true;
        return true;
    }

    const QString base = QCoreApplication::applicationDirPath();
    const QStringList fallbacks = {
        QDir(base).filePath(QStringLiteral("hmi_zh.qm")),
        QDir(base).filePath(QStringLiteral("translations/hmi_zh.qm")),
    };
    for (int i = 0; i < fallbacks.size(); ++i)
    {
        if (m_translator.load(fallbacks.at(i)))
        {
            m_loaded = true;
            return true;
        }
    }

    qWarning() << "failed to load Chinese translator hmi_zh.qm";
    return false;
}

bool I18nManager::applyLanguage(int lang)
{
    QApplication *app = qobject_cast<QApplication *>(QCoreApplication::instance());
    if (!app)
        return false;

    if (lang == int(AppSettings::Chinese))
    {
        if (!ensureLoaded())
            return false;
        app->installTranslator(&m_translator);
    }
    else
    {
        app->removeTranslator(&m_translator);
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
