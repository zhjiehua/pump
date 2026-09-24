#ifndef I18NMANAGER_H
#define I18NMANAGER_H

#include <QObject>
#include <QTranslator>

class AppSettings;

class I18nManager : public QObject
{
    Q_OBJECT
public:
    explicit I18nManager(AppSettings *settings, QObject *parent = nullptr);

    bool applyLanguage(int lang);
    bool applyFromSettings();

private:
    AppSettings *m_settings = nullptr;
    QTranslator m_translator;
};

#endif
