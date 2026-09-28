#include "ui/pages/logopage.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

LogoPage::LogoPage(MainWindow *main, QWidget *parent)
    : QWidget(parent)
    , m_main(main)
{
    Q_UNUSED(parent);
    auto *root = new QVBoxLayout(this);
    root->addStretch(1);
    auto *logo = new QLabel;
    logo->setAlignment(Qt::AlignCenter);
    logo->setPixmap(PictureManager::instance().pixmap(PictureManager::Logo));
    logo->setScaledContents(true);
    logo->setMaximumSize(200, 120);
    root->addWidget(logo, 0, Qt::AlignCenter);
    root->addStretch(1);

    QTimer::singleShot(1500, this, SLOT(onTimeout()));
}

void LogoPage::onTimeout()
{
    if (m_main)
        m_main->navigate(MainWindow::Run);
}
