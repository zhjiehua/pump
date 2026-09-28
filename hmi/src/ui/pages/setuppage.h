#ifndef SETUPPAGE_H
#define SETUPPAGE_H

#include "ui/focuspage.h"

class MainWindow;
class QPushButton;

/** Setup icon grid (weiduodianzi order). */
class SetupPage : public FocusPage
{
    Q_OBJECT
public:
    explicit SetupPage(MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void goLanguage();
    void goFix();
    void goPermit();
    void goTime();
    void goMsg();
    void goNet();
    void goGradient();
    void goGlp();
    void onAdmin();
    void goInternal();

private:
    MainWindow *m_main = nullptr;
    QPushButton *m_lang = nullptr;
    QPushButton *m_cal = nullptr;
    QPushButton *m_perm = nullptr;
    QPushButton *m_clock = nullptr;
    QPushButton *m_msg = nullptr;
    QPushButton *m_net = nullptr;
    QPushButton *m_grid = nullptr;
    QPushButton *m_glp = nullptr;
    QPushButton *m_admin = nullptr;
    QPushButton *m_internal = nullptr;
};

#endif
