#ifndef LANGUAGEPAGE_H
#define LANGUAGEPAGE_H

#include "core/appsettings.h"
#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class BtnCtrl;
class ImgButton;
class QLabel;

/** Language selection: globe + Chinese / English (weiduodianzi LanguagePage). */
class LanguagePage : public FocusPage
{
    Q_OBJECT
public:
    LanguagePage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onChinese();
    void onEnglish();

private:
    void setLanguage(AppSettings::Language lang);
    void refreshLabels();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    ImgButton *m_globe = nullptr;
    BtnCtrl *m_chinese = nullptr;
    BtnCtrl *m_english = nullptr;
    QLabel *m_chineseLabel = nullptr;
    QLabel *m_englishLabel = nullptr;
};

#endif
