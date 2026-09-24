#ifndef PWDPAGE_H
#define PWDPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QLabel;
class QLineEdit;
class QPushButton;

/** Password gate before admin / protected pages. */
class PwdPage : public FocusPage
{
    Q_OBJECT
public:
    /** @a adminLogin true → Admin label; false → User (MainWindow::pendingAdmin()). */
    PwdPage(MachineController *c, MainWindow *main, bool adminLogin = true,
            QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void onLogin();
    void onBack();

private:
    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QLabel *m_usr = nullptr;
    QLineEdit *m_pwd = nullptr;
    QPushButton *m_login = nullptr;
    bool m_adminLogin = true;
};

#endif
