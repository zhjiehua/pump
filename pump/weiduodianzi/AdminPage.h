#ifndef ADMINPAGE_H
#define ADMINPAGE_H

#include "BasePage.h"
#include "ui_AdminPage.h"
#include "msgbox.h"
#include "baseMainPage.h"

class AdminPage: public CBasePage
{
	Q_OBJECT

public:
	AdminPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~AdminPage(void);
	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	virtual void initFocusList();

private:
	Ui::AdminPage ui;
	void initDisplay();

private slots:
	void on_activeBtn_clicked();
	void on_utcBtn_clicked();
	void on_btcBtn_clicked();

	void on_rstBtn_clicked();//恢复出厂设置，测试用;

	void tryDayChanged(QString val);//清除使用时间，重新计时;

	void changePcPro(int idx);//更改协议;
	void changeConnectPort(int idx);

	void on_saveDataBtn_clicked();
	void on_updateDataBtn_clicked();
};

#endif //ADMINPAGE_H
