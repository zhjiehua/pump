#ifndef RUNPAGE_H
#define RUNPAGE_H

#include "ui_RunPage.h"
#include "BasePage.h"
#include "machinestat.h"

class RunPage : public CBasePage
{
	Q_OBJECT

public:
	RunPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~RunPage();

	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	virtual void initFocusList();
	bool eventFilter(QObject *obj, QEvent *event);

private:
	Ui::RunPageClass ui;
	void initDisplay();
	

private slots:
	void updateTime(quint32 startupTime);
	void updatePresVal(QString disp, quint8 warning);
	void updateFlowVal(QString disp);
	void updateGradient(int );
	void updateMachineStat(MachineStat::MachineStatment);
	void changeMachineStat(int);
	void updatePercentVal(QString disp);

	void changeFlow(QString str);//流速编辑栏改变了，需要执行相应变化;
};

#endif // RUNPAGE_H
