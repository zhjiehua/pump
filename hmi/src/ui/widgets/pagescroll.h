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
    // Do not participate in keyboard focus; otherwise showing a stacked page
    // restores the last icon/button instead of the bottom navigator.
    scroll->setFocusPolicy(Qt::NoFocus);
    if (scroll->viewport())
        scroll->viewport()->setFocusPolicy(Qt::NoFocus);
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

/** Keep the focused control inside the page scroll viewport. */
inline void ensureWidgetInScroll(QWidget *w)
{
    if (!w)
        return;
    for (QWidget *p = w->parentWidget(); p; p = p->parentWidget())
    {
        if (auto *scroll = qobject_cast<QScrollArea *>(p))
        {
            scroll->ensureWidgetVisible(w, 6, 12);
            return;
        }
    }
}

/** Host fills with a vertical scroll area whose content is `content`. */
inline void installPageScroll(QWidget *host, QWidget *content)
{
    auto *root = new QVBoxLayout(host);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    auto *scroll = makePageScroll();
    if (content)
        content->setFocusPolicy(Qt::NoFocus);
    if (host)
        host->setFocusPolicy(Qt::NoFocus);
    scroll->setWidget(content);
    root->addWidget(scroll);
}

#endif
