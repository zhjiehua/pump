#ifndef PERMITPAGE_H
#define PERMITPAGE_H

#include <BasePage.h>
#include "ui_PermitPage.h"



class PermitPage : public CBasePage
{
	Q_OBJECT

public:
	PermitPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~PermitPage();

	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	virtual void initFocusList();
	bool eventFilter(QObject *o, QEvent *e);

private:
	Ui::PermitPageClass ui;
	void showDeActive();
	void showActive();
	void showPermitition();

private slots:
	void on_registerBtn_clicked();
	void on_serialBtn_clicked();


};

#endif // PERMITPAGE_H
