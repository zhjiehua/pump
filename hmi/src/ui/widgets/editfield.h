#ifndef EDITFIELD_H
#define EDITFIELD_H

#include <QLabel>
#include <QLineEdit>
#include <QSizePolicy>
#include <QString>

/** Compact line edit styled like weiduodianzi EditCtrl (blue focus border). */
inline QLineEdit *makeEditField(QWidget *parent = nullptr)
{
    auto *e = new QLineEdit(parent);
    e->setAlignment(Qt::AlignCenter);
    e->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    e->setStyleSheet(
        QStringLiteral("QLineEdit{border:1px solid #888;background:white;}"
                       "QLineEdit:focus{border:2px solid blue;}"));
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
