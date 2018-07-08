#ifndef RUNPARAMPAGE_H
#define RUNPARAMPAGE_H

#include <BasePage.h>
//#include "ui_RunParamPage.h"
#include "ui_RunParamPage.h"

class RunParamPage : public CBasePage
{
	Q_OBJECT


public:
	RunParamPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~RunParamPage();

	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	virtual void initFocusList();
	void initDisplay();

private:
	Ui::RunParamPageClass ui;
	

private slots:
	void setMinAndMaxPress();
	void goToGradientPage();
};

#endif // RunPARAMPAGE_H
