#include "ui/bottombar.h"
#include "core/picturemanager.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

BottomBar::BottomBar(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto *root = new QHBoxLayout(this);
    // weiduodianzi bottomwidget.ui margin=6; keep compact for 1/11 height
    root->setContentsMargins(4, 1, 4, 1);
    root->setSpacing(2);

    auto *nav = new QHBoxLayout;
    nav->setSpacing(2);
    auto makeNav = [&](const QString &text) {
        auto *b = new QPushButton(text);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        b->setMinimumSize(0, 0);
        b->setFocusPolicy(Qt::StrongFocus);
        nav->addWidget(b, 1);
        m_btns.append(b);
        return b;
    };
    auto *run = makeNav(tr("Run"));
    auto *param = makeNav(tr("Param"));
    auto *setup = makeNav(tr("Setup"));
    nav->addStretch(2);
    root->addLayout(nav, 4);

    auto *icons = new QHBoxLayout;
    icons->setSpacing(2);
    m_weep = new QLabel;
    m_press = new QLabel;
    m_link = new QLabel;
    for (QLabel *l : {m_weep, m_press, m_link})
    {
        l->setMinimumSize(0, 0);
        l->setScaledContents(true);
        l->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        icons->addWidget(l);
    }
    root->addLayout(icons, 1);

    connect(run, SIGNAL(clicked()), this, SIGNAL(runClicked()));
    connect(param, SIGNAL(clicked()), this, SIGNAL(paramClicked()));
    connect(setup, SIGNAL(clicked()), this, SIGNAL(setupClicked()));

    setLinkOk(false);
    setPressWarn(0);
    setActiveNav(0);
}

void BottomBar::applyNavStyles()
{
    auto &pics = PictureManager::instance();
    for (int i = 0; i < m_btns.size(); ++i)
        m_btns[i]->setStyleSheet(pics.navButtonStyle(i == m_active));
}

void BottomBar::setActiveNav(int index)
{
    m_active = qBound(0, index, m_btns.size() - 1);
    applyNavStyles();
}

void BottomBar::focusNav(int index)
{
    if (m_btns.isEmpty())
        return;
    if (index < 0)
        index = m_active;
    index = qBound(0, index, m_btns.size() - 1);
    setActiveNav(index);
    m_btns.at(index)->setFocus(Qt::OtherFocusReason);
}

void BottomBar::setLinkOk(bool ok)
{
    auto &pics = PictureManager::instance();
    m_link->setStyleSheet(pics.labelBorderImage(
        ok ? PictureManager::ConnectEstablished : PictureManager::Disconnect));
}

void BottomBar::setPressWarn(int kind)
{
    auto &pics = PictureManager::instance();
    if (kind == 1)
        m_press->setStyleSheet(pics.labelBorderImage(PictureManager::Down));
    else if (kind == 2)
        m_press->setStyleSheet(pics.labelBorderImage(PictureManager::Up));
    else
        m_press->setStyleSheet(QString());
}
