#ifndef SETUPPAGE_H
#define SETUPPAGE_H

#include "ui/focuspage.h"

class MainWindow;
class BtnCtrl;
class QLabel;

/** Setup icon grid (weiduodianzi order). */
class SetupPage : public FocusPage
{
    Q_OBJECT
public:
    explicit SetupPage(MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void goLanguage();
    void goFix();
    void goPermit();
    void goTime();
    void goMsg();
    void goNet();
    void goGradient();
    void goGlp();

private:
    MainWindow *m_main = nullptr;
    BtnCtrl *m_lang = nullptr;
    BtnCtrl *m_cal = nullptr;
    BtnCtrl *m_perm = nullptr;
    BtnCtrl *m_clock = nullptr;
    BtnCtrl *m_msg = nullptr;
    BtnCtrl *m_net = nullptr;
    BtnCtrl *m_grid = nullptr;
    BtnCtrl *m_glp = nullptr;
    QLabel *m_langLabel = nullptr;
    QLabel *m_calLabel = nullptr;
    QLabel *m_permLabel = nullptr;
    QLabel *m_clockLabel = nullptr;
    QLabel *m_msgLabel = nullptr;
    QLabel *m_netLabel = nullptr;
    QLabel *m_gridLabel = nullptr;
    QLabel *m_glpLabel = nullptr;
};

#endif
