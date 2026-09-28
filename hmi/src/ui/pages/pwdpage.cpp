// MainWindow wiring (parent agent):
//   void tryLogin(const QString &pwd);
//   void requestAdminAccess();
//   void goPendingAfterPwd();
//   bool pendingAdmin() const;

#include "ui/pages/pwdpage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

PwdPage::PwdPage(MachineController *c, MainWindow *main, bool adminLogin, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
    , m_adminLogin(adminLogin)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);

    auto *form = new QFormLayout;
    m_usr = new QLabel(m_adminLogin ? tr("Admin") : tr("User"));
    m_pwd = new QLineEdit;
    m_pwd->setEchoMode(QLineEdit::Password);
    form->addRow(tr("User"), m_usr);
    form->addRow(tr("Password"), m_pwd);
    root->addLayout(form);

    auto *btns = new QHBoxLayout;
    m_login = new QPushButton(tr("Login"));
    auto *back = new QPushButton(tr("Back"));
    btns->addWidget(m_login);
    btns->addWidget(back);
    root->addLayout(btns);
    root->addStretch(1);

    connect(m_login, SIGNAL(clicked()), this, SLOT(onLogin()));
    connect(back, SIGNAL(clicked()), this, SLOT(onBack()));
}

void PwdPage::initFocusList()
{
    xList.append(m_pwd);
    xList.append(m_login);
    yList.append(m_pwd);
    yList.append(m_login);
}

void PwdPage::onLogin()
{
    m_main->tryLogin(m_pwd->text());
}

void PwdPage::onBack()
{
    m_main->goBack();
}
