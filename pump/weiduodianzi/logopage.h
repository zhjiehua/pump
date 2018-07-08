#ifndef LOGOPAGE_H
#define LOGOPAGE_H

#include "ui_logpage.h"
#include "BasePage.h"

class LogoPage : public CBasePage
{
	Q_OBJECT

public:
	LogoPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~LogoPage();
	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

private:
	Ui::LogoPageClass ui;
	

private slots:
	void gotoSetupPage();
};

#endif // LOGOPAGE_H
