#ifndef RUNTIMEPAGE_H
#define RUNTIMEAGE_H

#include <BasePage.h>
#include "ui_PwdPage.h"

class PwdPage : public CBasePage
{
	Q_OBJECT

public:
	PwdPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~PwdPage();
	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	virtual void initFocusList();
	bool eventFilter(QObject *obj, QEvent *event);

private:
	Ui::PwdPage ui;
	

private slots:
	void loginAdmin();
};

#endif // RUNTIMEPAGE_H
