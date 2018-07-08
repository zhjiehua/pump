#include "btnctrl.h"
#include <QKeyEvent>
#include "BasePage.h"

BtnCtrl::BtnCtrl(QWidget *parent)
	: QPushButton(parent)
{
	//setStyleSheet( BTNGLOBALSTYLE);
}

BtnCtrl::~BtnCtrl()
{

}

void BtnCtrl::keyPressEvent( QKeyEvent *event )
{
	if(event->key() == KEY_RETURN)
	{
		emit(clicked());
	}
	else
		return QPushButton::keyPressEvent(event);
}
