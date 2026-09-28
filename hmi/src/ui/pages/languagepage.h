#ifndef LANGUAGEPAGE_H
#define LANGUAGEPAGE_H

#include "core/appsettings.h"
#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QPushButton;

/** Language selection: Chinese / English icon buttons. */
class LanguagePage : public FocusPage
{
    Q_OBJECT
public:
    LanguagePage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void onChinese();
    void onEnglish();

private:
    void setLanguage(AppSettings::Language lang);
    void refreshIcons();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QPushButton *m_chinese = nullptr;
    QPushButton *m_english = nullptr;
};

#endif
