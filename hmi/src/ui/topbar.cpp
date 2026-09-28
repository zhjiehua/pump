#include "ui/topbar.h"
#include <QColor>
#include <QLabel>
#include <QSizePolicy>
#include <QVBoxLayout>

TopBar::TopBar(QWidget *parent)
    : QWidget(parent)
{
    // Qt5: stylesheets on QWidget need WA_StyledBackground to paint.
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoFillBackground(true);
    setObjectName(QStringLiteral("TopBar"));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setStyleSheet(QStringLiteral("QWidget#TopBar{background-color:rgb(85,170,255);}"));

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(85, 170, 255));
    pal.setColor(QPalette::WindowText, Qt::white);
    setPalette(pal);

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    m_title = new QLabel;
    m_title->setObjectName(QStringLiteral("titleLabel"));
    m_title->setAlignment(Qt::AlignCenter);
    m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QFont f = m_title->font();
    f.setBold(true);
    m_title->setFont(f);
    m_title->setStyleSheet(
        QStringLiteral("QLabel#titleLabel{color:rgb(255,255,255);"
                       "background-color:transparent;font-weight:bold;}"));
    lay->addWidget(m_title);
}

void TopBar::setTitle(const QString &title)
{
    m_title->setText(title);
}
