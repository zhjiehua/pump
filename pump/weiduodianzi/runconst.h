#ifndef RUNCONSTPAGE_H
#define RUNCONSTPAGE_H

#include <BasePage.h>
#include "ui_RunConstPage.h"

class RunConstPage : public CBasePage
{
	Q_OBJECT

public:
	RunConstPage(QWidget *parent = 0);
	~RunConstPage();


private:
	Ui::RunConstPageClass ui;
	

private slots:
	void gotoRunConstPage();
};

#endif // RunConstPage_H
