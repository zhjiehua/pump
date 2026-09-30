#include "ui/pages/pwdpage.h"
#include "core/machinecontroller.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/pagescroll.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

PwdPage::PwdPage(MachineController *c, MainWindow *main, bool adminLogin, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
    , m_adminLogin(adminLogin)
{
    auto *inner = new QWidget;
    auto *root = new QHBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);
    root->setSpacing(0);

    root->addStretch(1);

    auto *iconCol = new QVBoxLayout;
    iconCol->addStretch(1);
    auto *icon = new QLabel;
    icon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    icon->setFocusPolicy(Qt::NoFocus);
    icon->setStyleSheet(PictureManager::instance().labelBorderImage(PictureManager::User));
    iconCol->addWidget(icon, 2);
    iconCol->addStretch(1);
    root->addLayout(iconCol, 5);

    root->addStretch(1);

    auto *form = new QVBoxLayout;
    form->setSpacing(6);
    form->addStretch(1);

    auto *usrRow = new QHBoxLayout;
    m_usrCap = new QLabel;
    m_usrCap->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_usr = new EditCtrl;
    m_usr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_usr->setReadOnly(true);
    m_usr->setEnabled(false);
    m_usr->setFocusPolicy(Qt::NoFocus);
    usrRow->addWidget(m_usrCap, 3);
    usrRow->addWidget(m_usr, 7);
    form->addLayout(usrRow, 1);
    form->addStretch(1);

    auto *pwdRow = new QHBoxLayout;
    m_pwdCap = new QLabel;
    m_pwdCap->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pwd = new EditCtrl;
    m_pwd->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pwd->setEchoMode(QLineEdit::Password);
    m_pwd->setValRange(0, 999999, 0);
    connect(m_pwd, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    pwdRow->addWidget(m_pwdCap, 3);
    pwdRow->addWidget(m_pwd, 7);
    form->addLayout(pwdRow, 1);
    form->addStretch(1);

    auto *loginRow = new QHBoxLayout;
    loginRow->addStretch(3);
    m_login = new BtnCtrl;
    m_login->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    loginRow->addWidget(m_login, 7);
    form->addLayout(loginRow, 1);
    form->addStretch(1);

    root->addLayout(form, 8);
    root->addStretch(1);

    installPageScroll(this, inner);
    retranslateUi();

    connect(m_login, SIGNAL(clicked()), this, SLOT(onLogin()));
}

void PwdPage::initFocusList()
{
    m_adminLogin = m_main->pendingAdmin();
    m_usr->setText(m_adminLogin ? QStringLiteral("Admin") : QStringLiteral("User"));
    m_pwd->clear();
    xList.append(m_pwd);
    xList.append(m_login);
    yList.append(m_pwd);
    yList.append(m_login);
}

void PwdPage::retranslateUi()
{
    m_usrCap->setText(tr("User:"));
    m_pwdCap->setText(tr("Pwd:"));
    m_login->setText(tr("Login"));
    m_usr->setText(m_main->pendingAdmin() ? QStringLiteral("Admin") : QStringLiteral("User"));
}

void PwdPage::onLogin()
{
    m_main->tryLogin(m_pwd->text());
}
