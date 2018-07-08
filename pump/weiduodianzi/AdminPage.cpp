#include "AdminPage.h"
#include <QKeyEvent>
#include "ui_AdminPage.h"
#include <QDebug>
#include <QMessageBox>
#include "baseMainPage.h"
#include "machinestat.h"

#include "msgbox.h"
#include <QFile>

#ifdef WIN32
#define SRCFILE			"H:/weiduodianzi.txt"
#define TARGETFILE		"K:/weiduodianzi.txt"

#define SRCDBFILE		"H:/wda.txt"
#define TARGETDBFILE	"K:/wda.txt"
#else
#define SRCFILE			"/sdcard/sepuyi"
#define TARGETFILE		"/bin/weiduodianzi"

#define SRCDBFILE		"/sdcard/wda.db"
#define TARGETDBFILE	"/bin/wda.db"
#endif

AdminPage::AdminPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("AdminPage"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = tr("Admin");
	initDisplay();
	setStyleSheet(BTNGLOBALSTYLE);
}


AdminPage::~AdminPage(void)
{
}


void AdminPage::initFocusList()
{
	xList.append(ui.activeBtn);
	xList.append(ui.utcBtn);
	xList.append(ui.btcBtn);
	xList.append(ui.rstBtn);//恢复出厂设置，测试用;
	xList.append(ui.saveDataBtn);
	xList.append(ui.updateDataBtn);
	xList.append(ui.manufYearEdit);
	xList.append(ui.manufMonthEdit);
	xList.append(ui.manufDayEdit);
	xList.append(ui.instYearEdit);
	xList.append(ui.instMonthEdit);
	xList.append(ui.instDayEdit);
	xList.append(ui.repairYearEdit);
	xList.append(ui.repairMonthEdit);
	xList.append(ui.repairDayEdit);
	xList.append(ui.licenseEdit);
	xList.append(ui.proCombo);
	xList.append(ui.serialEdit);
    xList.append(ui.connectCombo);
	xList.append(ui.pmaxEdit);

	yList.append(ui.activeBtn);
	yList.append(ui.utcBtn);
	yList.append(ui.btcBtn);
	yList.append(ui.rstBtn);//恢复出厂设置，测试用;
	yList.append(ui.saveDataBtn);
	yList.append(ui.updateDataBtn);
	yList.append(ui.manufYearEdit);
	yList.append(ui.instYearEdit);
	yList.append(ui.repairYearEdit);
	yList.append(ui.manufMonthEdit);
	yList.append(ui.instMonthEdit);
	yList.append(ui.repairMonthEdit);
	yList.append(ui.manufDayEdit);
	yList.append(ui.instDayEdit);
	yList.append(ui.repairDayEdit);
	yList.append(ui.licenseEdit);
	yList.append(ui.serialEdit);
	yList.append(ui.pmaxEdit);
	yList.append(ui.proCombo);
    yList.append(ui.connectCombo);
	
	
	ui.pmaxEdit->setValRange(QString("pmax"),0,200, 2);
	ui.pmaxEdit->setText(DataBase::getInstance()->queryData("pmax"));
}



CBasePage* AdminPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new AdminPage(parent, index, previndex, add);
}



void AdminPage::on_activeBtn_clicked()
{
	MsgBox msgBox(this, tr("Tips"), tr("Comfirm to Cancel Active!!!"));
	if(msgBox.exec() == QMessageBox::Ok)
	{
		MachineStat::getInstance()->activeMachine(0, 0);				//取消激活;
		MachineStat::getInstance()->clearTime(MachineStat::SYSTIME);	//试用期清零重新计算;
	}
}

void AdminPage::on_utcBtn_clicked()
{
	MsgBox msgBox(this, tr("Tips"), tr("Comfirm to Clear!!!"));
	if(msgBox.exec() == QMessageBox::Ok)
		MachineStat::getInstance()->clearTime(MachineStat::SYSTIME);
}

void AdminPage::on_btcBtn_clicked()
{
	MsgBox msgBox(this, tr("Tips"), tr("Comfirm to Clear!!!"));
	if(msgBox.exec() == QMessageBox::Ok)
		MachineStat::getInstance()->clearTime(MachineStat::PUMPTIME);
}

void AdminPage::initDisplay()
{
	ui.repairYearEdit->setValRange("repairYear", 1990, 2050, 0);
	ui.repairMonthEdit->setValRange("repairMonth", 1, 12, 0);
	ui.repairDayEdit->setValRange("repairDay", 0, 31, 0);

	ui.manufYearEdit->setValRange("manufYear", 1990, 2050, 0);
	ui.manufMonthEdit->setValRange("manufMonth", 1, 12, 0);
	ui.manufDayEdit->setValRange("manufDay", 0, 31, 0);

	ui.instYearEdit->setValRange("instYear", 1990, 2050, 0);
	ui.instMonthEdit->setValRange("instMonth", 1, 12, 0);
	ui.instDayEdit->setValRange("instDay", 0, 31, 0);
	//ui.licenseEdit->setValRange("license",0, 9999999999, 0, 10);
	ui.licenseEdit->setValRange("license",0, 9999999999);////吴杰能修改@2016-06-08
	ui.serialEdit->setValRange("serial",0, 9999999999, 0, 10);
	//ui.dayEdit->setValRange("tryDay", 1, 1000, 0);
	ui.proCombo->setVar("pcProtocol");
    ui.connectCombo->setVar("connect_port");
	//ui.activeBtn->setFocus();

	connect(ui.proCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changePcPro(int)));
	connect(ui.connectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeConnectPort(int)));

	//connect(ui.dayEdit, SIGNAL(dataChanging(QString)), this, SLOT(tryDayChanged(QString)) );
	//ui.dayEdit->setEnabled(false);//屏蔽试用日期的编辑

	ui.licenseEdit->setTextMode();
	ui.serialEdit->setTextMode();
	ui.licenseEdit->setReadOnly(true);
	ui.instYearEdit->setReadOnly(true);
}

void AdminPage::on_rstBtn_clicked()
{
	MsgBox msgBox(this, tr("Tips"), tr("Comfirm to Restore!!!"));
	if(msgBox.exec() == QMessageBox::Ok)
		DataBase::getInstance()->restoreCompensationTable();
}

void AdminPage::tryDayChanged(QString val)
{
	MachineStat::getInstance()->clearTime(MachineStat::SYSTIME);
}

void AdminPage::changePcPro(int idx)
{
	MachineStat::getInstance()->setPcProtocol(idx);
}

void AdminPage::changeConnectPort(int idx)
{
	MachineStat::getInstance()->setConnectPort(idx);
}

//保存数据
void AdminPage::on_saveDataBtn_clicked()
{
	//qDebug() << "on_saveDataBtn_clicked()";
	QFile file(TARGETDBFILE);
	if(!file.exists())
	{
		MsgBox msgBox(this,tr("Tips"), tr("file not found!"));
		msgBox.exec();
		return;
	}

	QFile target(SRCDBFILE);
	if(target.exists())
		qDebug()<<target.remove();
	if(QFile::copy(TARGETDBFILE, SRCDBFILE) )
	{
		MsgBox msgBox(this,tr("Tips"), tr("success!"));
		msgBox.exec();
	}
	else
	{
		MsgBox msgBox(this,tr("Tips"), tr("failed!"));
		msgBox.exec();
	}
}

//更新数据
void AdminPage::on_updateDataBtn_clicked()
{
	MsgBox msgBox(this, tr("Tips"), tr("Comfirm to update data?"));
	if(msgBox.exec() != QMessageBox::Ok)
		return;

	//qDebug() << "on_updateDataBtn_clicked()";
	QFile file(SRCDBFILE);
	if(!file.exists())
	{
		MsgBox msgBox(this,tr("Tips"), tr("file not found!"));
		msgBox.exec();
		return;
	}

	QFile target(TARGETDBFILE);
	if(target.exists())
		qDebug()<<target.remove();
	if(QFile::copy(SRCDBFILE, TARGETDBFILE))
	{
		MsgBox msgBox(this,tr("Tips"), tr("success!"));
		msgBox.exec();
	}
	else
	{
		MsgBox msgBox(this,tr("Tips"), tr("failed!"));
		msgBox.exec();
	}
}

