#ifndef EDITFIELD_H
#define EDITFIELD_H

#include "ui/widgets/editctrl.h"

#include <QLabel>
#include <QSizePolicy>
#include <QString>

/** Compact centered EditCtrl for IP/date fields. */
inline EditCtrl *makeEditField(QWidget *parent = nullptr)
{
    auto *e = new EditCtrl(parent);
    e->setAlignment(Qt::AlignCenter);
    e->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return e;
}

inline QLabel *makeSep(const QString &text, QWidget *parent = nullptr)
{
    auto *l = new QLabel(text, parent);
    l->setAlignment(Qt::AlignCenter);
    l->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return l;
}

#endif
