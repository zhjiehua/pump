#include "ui/pages/permitpage.h"
#include "core/authservice.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/pagescroll.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

PermitPage::PermitPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);
    root->setSpacing(0);

    root->addStretch(1);

    auto *top = new QHBoxLayout;
    top->setSpacing(0);
    top->addStretch(1);
    m_serialBtn = new QPushButton(tr("Verif Code"));
    m_serialBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    top->addWidget(m_serialBtn, 6);
    top->addStretch(1);
    m_license = new EditCtrl;
    m_license->setValRange(0, 9999999999ULL, 0, true);
    connect(m_license, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    top->addWidget(m_license, 12);
    top->addStretch(1);
    root->addLayout(top, 2);

    root->addStretch(1);

    auto *mid = new QHBoxLayout;
    mid->addStretch(2);
    auto *daysCap = new QLabel(tr("Days left:"));
    daysCap->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_days = new QLabel;
    m_days->setAlignment(Qt::AlignCenter);
    m_days->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mid->addWidget(daysCap, 4);
    mid->addWidget(m_days, 6);
    mid->addStretch(2);
    root->addLayout(mid, 2);

    root->addStretch(2);

    auto *bot = new QHBoxLayout;
    bot->addStretch(2);
    m_register = new QPushButton(tr("Register"));
    m_register->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bot->addWidget(m_register, 4);
    bot->addStretch(2);
    root->addLayout(bot, 2);

    root->addStretch(1);

    installPageScroll(this, inner);

    refreshDays();

    connect(m_register, SIGNAL(clicked()), this, SLOT(onRegister()));
    connect(m_serialBtn, SIGNAL(clicked()), this, SLOT(onSerial()));
    connect(m_c->auth(), SIGNAL(authChanged()), this, SLOT(refreshDays()));
}

void PermitPage::initFocusList()
{
    xList.append(m_serialBtn);
    xList.append(m_license);
    xList.append(m_register);
    yList.append(m_serialBtn);
    yList.append(m_license);
    yList.append(m_register);
}

void PermitPage::onRegister()
{
    const QString lic = m_license->text().trimmed();
    if (lic.isEmpty())
    {
        QMessageBox::warning(this, tr("Tips"), tr("Register failed!"));
        return;
    }
    const quint64 code = lic.toULongLong();
    if (!m_c->auth()->activate(code))
    {
        QMessageBox::warning(this, tr("Tips"), tr("Register failed!"));
        return;
    }
    refreshDays();
    QMessageBox::information(this, tr("Tips"), tr("Register success!"));
}

void PermitPage::onSerial()
{
    QMessageBox::information(this, tr("Tips"),
                             tr("Serial: %1").arg(m_c->settings()->serial));
}

void PermitPage::refreshDays()
{
    auto *s = m_c->settings();
    if (s->bActive)
        m_days->setText(tr("infinite"));
    else
    {
        const int left = qMax(0, s->tryDay - int(s->sysUsedSec / (24 * 3600)));
        m_days->setText(QString::number(left));
    }
    m_license->setText(s->license);
}
