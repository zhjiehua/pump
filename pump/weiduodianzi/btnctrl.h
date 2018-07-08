#ifndef BTNCTRL_H
#define BTNCTRL_H

#include <QPushButton>

class BtnCtrl : public QPushButton
{
	Q_OBJECT

public:
	BtnCtrl(QWidget *parent = 0);
	~BtnCtrl();

protected:
	void keyPressEvent(QKeyEvent *event);

private:

};

#endif // BTNCTRL_H
