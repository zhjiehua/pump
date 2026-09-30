#include "ui/pages/permitpage.h"
#include "core/authservice.h"
#include "core/machinecontroller.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/imgbutton.h"
#include "ui/widgets/pagescroll.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTime>
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
    m_serialBtn = new BtnCtrl;
    m_serialBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    top->addWidget(m_serialBtn, 6);
    top->addStretch(1);
    m_license = new EditCtrl;
    m_license->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_license->setValRange(0, 9999999999ULL, 0, true);
    connect(m_license, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    top->addWidget(m_license, 12);
    top->addStretch(1);
    root->addLayout(top, 2);

    auto *mid = new QHBoxLayout;
    mid->setSpacing(0);
    mid->addStretch(1);

    auto *iconCol = new QVBoxLayout;
    iconCol->setSpacing(0);
    m_icon = new ImgButton;
    m_icon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_icon->setFocusPolicy(Qt::NoFocus);
    m_icon->setBkImage(PictureManager::Permission);
    iconCol->addWidget(m_icon, 7);
    mid->addLayout(iconCol, 6);
    mid->addStretch(1);

    auto *right = new QVBoxLayout;
    right->setSpacing(0);
    right->addStretch(1);
    m_register = new BtnCtrl;
    m_register->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right->addWidget(m_register, 2);
    right->addStretch(1);

    m_tips = new QLabel;
    m_tips->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_tips->setStyleSheet(QStringLiteral("color: rgb(170, 0, 0);"));
    right->addWidget(m_tips, 1);

    auto *daysRow = new QHBoxLayout;
    m_probation = new QLabel;
    m_probation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_days = new QLabel;
    m_days->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_days->setStyleSheet(QStringLiteral("color: rgb(170, 0, 0);"));
    m_dayUnit = new QLabel;
    m_dayUnit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    daysRow->addWidget(m_probation, 2);
    daysRow->addWidget(m_days, 2);
    daysRow->addWidget(m_dayUnit, 1);
    right->addLayout(daysRow, 2);

    mid->addLayout(right, 12);
    mid->addStretch(1);
    root->addLayout(mid, 10);

    root->addStretch(1);

    installPageScroll(this, inner);
    retranslateUi();
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

void PermitPage::retranslateUi()
{
    m_serialBtn->setText(tr("Verif Code"));
    m_register->setText(tr("Active"));
    m_tips->setText(tr("Tips:"));
    m_probation->setText(tr("Probation period:"));
    m_dayUnit->setText(tr("day"));
    refreshDays();
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
    qsrand(uint(QTime::currentTime().msec()));
    const quint32 serialId = quint32(qrand() % 99999999);
    auto *s = m_c->settings();
    s->serialId = serialId;
    s->save();

    QString str = tr("Serial:");
    str += s->serial;
    str += QChar('\n');
    str += tr("Rand:");
    str += QString::number(serialId);
    QMessageBox::information(this, tr("tips"), str);
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
