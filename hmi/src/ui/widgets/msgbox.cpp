#include "ui/widgets/msgbox.h"
#include "utils/hmikeys.h"

#include <QAbstractButton>
#include <QEvent>
#include <QKeyEvent>
#include <QShowEvent>

MsgBox::MsgBox(QWidget *parent, const QString &title, const QString &text,
               StandardButtons buttons)
    : QMessageBox(QMessageBox::Information, title, text, buttons, parent)
{
}

int MsgBox::question(QWidget *parent, const QString &title, const QString &text)
{
    MsgBox box(parent, title, text, QMessageBox::Yes | QMessageBox::No);
    box.setDefaultButton(QMessageBox::Yes);
    return box.exec();
}

void MsgBox::information(QWidget *parent, const QString &title, const QString &text)
{
    MsgBox box(parent, title, text, QMessageBox::Ok);
    box.exec();
}

void MsgBox::warning(QWidget *parent, const QString &title, const QString &text)
{
    MsgBox box(parent, title, text, QMessageBox::Ok);
    box.setIcon(QMessageBox::Warning);
    box.exec();
}

void MsgBox::cycleFocus()
{
    focusNextChild();
}

bool MsgBox::handleNavKey(int key)
{
    if (key != KEY_LEFT && key != KEY_UP && key != KEY_RIGHT && key != KEY_DOWN)
        return false;
    cycleFocus();
    return true;
}

void MsgBox::keyPressEvent(QKeyEvent *event)
{
    if (handleNavKey(event->key()))
        return;
    QMessageBox::keyPressEvent(event);
}

void MsgBox::showEvent(QShowEvent *event)
{
    QMessageBox::showEvent(event);
    const QList<QAbstractButton *> btns = buttons();
    for (int i = 0; i < btns.size(); ++i)
    {
        if (QAbstractButton *b = btns.at(i))
            b->installEventFilter(this);
    }
}

bool MsgBox::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        auto *ke = static_cast<QKeyEvent *>(event);
        if (handleNavKey(ke->key()))
            return true;
    }
    return QMessageBox::eventFilter(obj, event);
}
