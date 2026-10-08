#include "ui/pages/logopage.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"

#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

LogoPage::LogoPage(MainWindow *main, QWidget *parent)
    : QWidget(parent)
    , m_main(main)
{
    Q_UNUSED(parent);
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    auto *logo = new QLabel;
    logo->setAlignment(Qt::AlignCenter);
    logo->setPixmap(PictureManager::instance().pixmap(PictureManager::Logo));
    logo->setScaledContents(true);
    logo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    root->addWidget(logo);

    QTimer::singleShot(1500, this, SLOT(onTimeout()));
}

void LogoPage::onTimeout()
{
    if (m_main)
        m_main->navigate(MainWindow::Run);
}
