#include "ui/pages/msgpage.h"
#include "core/machinecontroller.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"
#include "ui/widgets/pagescroll.h"
#include "utils/version.h"

#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
#if defined(Q_OS_LINUX)
const char kSrcProgram[] = "/sdcard/sepuyi";
const char kTargetProgram[] = "/bin/pump";
#endif
} // namespace

MsgPage::MsgPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);
    root->setSpacing(0);

    auto *body = new QHBoxLayout;
    body->addStretch(1);

    auto *iconCol = new QVBoxLayout;
    iconCol->addStretch(1);
    auto *icon = new QPushButton;
    icon->setEnabled(false);
    icon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    icon->setMinimumSize(48, 48);
    icon->setStyleSheet(PictureManager::instance().pushButtonBorderImage(PictureManager::Message));
    iconCol->addWidget(icon, 3);
    iconCol->addStretch(1);
    body->addLayout(iconCol, 5);
    body->addStretch(2);

    auto *info = new QVBoxLayout;
    info->addStretch(1);
    auto addRow = [&](const QString &cap, QLabel **value) {
        auto *row = new QHBoxLayout;
        auto *lab = new QLabel(cap);
        lab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        *value = new QLabel;
        (*value)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        row->addWidget(lab, 1);
        row->addWidget(*value, 3);
        info->addLayout(row, 2);
        info->addStretch(1);
    };
    addRow(tr("Version:"), &m_version);
    addRow(tr("License:"), &m_license);
    addRow(tr("Serial:"), &m_serial);
    body->addLayout(info, 10);
    body->addStretch(1);

    root->addLayout(body, 20);

    auto *btns = new QHBoxLayout;
    btns->addStretch(1);
    m_update = new QPushButton(tr("Update Program"));
    m_update->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btns->addWidget(m_update, 5);
    btns->addStretch(1);
    root->addLayout(btns, 3);
    root->addStretch(1);

    installPageScroll(this, inner);

    m_version->setText(QString::fromLatin1(HMI_APP_VERSION));
    refreshLabels();

    connect(m_update, SIGNAL(clicked()), this, SLOT(updateProgram()));
}

void MsgPage::initFocusList()
{
    xList.append(m_update);
    yList.append(m_update);
}

void MsgPage::refreshLabels()
{
    auto *s = m_c->settings();
    m_license->setText(s->license);
    m_serial->setText(s->serial);
}

void MsgPage::updateProgram()
{
    if (QMessageBox::question(this, tr("Tips"), tr("Comfirm to update program?"))
        != QMessageBox::Yes)
        return;

#if defined(Q_OS_LINUX)
    QFile src(QString::fromLatin1(kSrcProgram));
    if (!src.exists())
    {
        QMessageBox::warning(this, tr("Tips"), tr("file not found!"));
        return;
    }
    QFile target(QString::fromLatin1(kTargetProgram));
    if (target.exists())
        target.remove();
    if (QFile::copy(src.fileName(), target.fileName()))
        QMessageBox::information(this, tr("Tips"), tr("success!"));
    else
        QMessageBox::warning(this, tr("Tips"), tr("failed!"));
#else
    QMessageBox::information(this, tr("Tips"), tr("success!"));
#endif
}
