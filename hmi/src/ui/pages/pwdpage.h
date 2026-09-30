#ifndef PWDPAGE_H
#define PWDPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QLabel;
class EditCtrl;
class BtnCtrl;

/** Password gate before admin / protected pages (weiduodianzi PwdPage). */
class PwdPage : public FocusPage
{
    Q_OBJECT
public:
    PwdPage(MachineController *c, MainWindow *main, bool adminLogin = true,
            QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onLogin();

private:
    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QLabel *m_usrCap = nullptr;
    QLabel *m_pwdCap = nullptr;
    EditCtrl *m_usr = nullptr;
    EditCtrl *m_pwd = nullptr;
    BtnCtrl *m_login = nullptr;
    bool m_adminLogin = true;
};

#endif
