#include "ui/widgets/btnctrl.h"
#include "utils/hmikeys.h"

#include <QEvent>
#include <QKeyEvent>

BtnCtrl::BtnCtrl(QWidget *parent)
    : QPushButton(parent)
{
}

BtnCtrl::BtnCtrl(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
}

bool BtnCtrl::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride)
    {
        const int key = static_cast<QKeyEvent *>(event)->key();
        if (key == KEY_RETURN || key == Qt::Key_Enter)
        {
            event->accept();
            return true;
        }
    }
    return QPushButton::event(event);
}

void BtnCtrl::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key == KEY_RETURN || key == Qt::Key_Enter)
    {
        emit clicked();
        return;
    }
    QPushButton::keyPressEvent(event);
}
