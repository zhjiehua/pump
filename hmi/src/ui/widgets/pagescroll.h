#ifndef PAGESCROLL_H
#define PAGESCROLL_H

#include <QFrame>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

/** Shared vertical scroll style for Setup and its sub-pages. */
inline void applyPageScrollStyle(QScrollArea *scroll)
{
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setStyleSheet(QStringLiteral(
        "QScrollArea{background:transparent;border:0;}"
        "QScrollBar:vertical{width:10px;background:transparent;}"
        "QScrollBar::handle:vertical{background:#888;min-height:20px;border-radius:3px;}"
        "QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{height:0;}"));
}

inline QScrollArea *makePageScroll(QWidget *parent = nullptr)
{
    auto *scroll = new QScrollArea(parent);
    applyPageScrollStyle(scroll);
    return scroll;
}

/** Host fills with a vertical scroll area whose content is `content`. */
inline void installPageScroll(QWidget *host, QWidget *content)
{
    auto *root = new QVBoxLayout(host);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    auto *scroll = makePageScroll();
    scroll->setWidget(content);
    root->addWidget(scroll);
}

#endif
