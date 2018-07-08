#include "msgbox.h"
#include <QKeyEvent>
#include "Common.h"

MsgBox::MsgBox(QWidget *parent, const QString & title, const QString & text, StandardButtons buttons)
	:QMessageBox(QMessageBox::Information, title, text, buttons)
{

}

MsgBox::~MsgBox()
{

}

void MsgBox::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == KEY_LEFT || event->key() == KEY_UP || event->key() == KEY_RIGHT || event->key() == KEY_DOWN)
	{
		focusNextChild();
	}
	else
		return QMessageBox::keyPressEvent(event);
}
