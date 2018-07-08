#include "permitpage.h"
#include <QDebug>
#include "Common.h"
#include "baseMainPage.h"
#include "msgbox.h"
#include "machinestat.h"
#include "timehelper.h"
#include <QFile>




PermitPage::PermitPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Permission"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = tr("Permission");
	ui.permitBtn->setBkImage(":/weiduodianzi/ui/Permission.png");

	setStyleSheet(BTNGLOBALSTYLE);
}

PermitPage::~PermitPage()
{

}


void PermitPage::initFocusList()
{
	xList.append(ui.serialBtn);
	xList.append(ui.licenseEdit);
	xList.append(ui.registerBtn);
	
	

	yList.append(ui.serialBtn);
	yList.append(ui.licenseEdit);
	yList.append(ui.registerBtn);
	
	

	ui.licenseEdit->setValRange("lic", 0, 9999999999);//张杰华修改@2016-06-18
	ui.licenseEdit->setTextMode();
	

	showPermitition();
	
}

bool PermitPage::eventFilter(QObject *o, QEvent *e)
{
	return CBasePage::eventFilter(o,e);
}

void PermitPage::on_registerBtn_clicked()
{
	if( MachineStat::getInstance()->activeMachine(ui.licenseEdit->text().toULongLong()) )
	{
		MsgBox msgBox(this,tr("Tips"), tr("Register success!"));
		msgBox.exec();
		showPermitition();
	}
	else
	{
		MsgBox msgBox(this,tr("Tips"), tr("Register failed!"));
		msgBox.exec();
	}
}

CBasePage* PermitPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new PermitPage(parent, index, previndex, add);
}



void PermitPage::on_serialBtn_clicked()
{
	quint32 serialId = TimeHelper::getRandom();
	QString str = tr("Serial:");
	str += DataBase::getInstance()->queryData("serial");
	str += "\n";
	str += tr("Rand:");
	//再加上一个随机数;
	str += QString::number(serialId);
	MachineStat::getInstance()->updateSerialId(serialId);
	MsgBox msg(this, tr("tips"), str);
	msg.exec();
}



void PermitPage::showDeActive()
{
	int day = MachineStat::getInstance()->checkProbation();
	ui.labelDay->setText(QString::number(day));
}

void PermitPage::showActive()
{
	//ui.licenseEdit->hide();
	//ui.registerBtn->hide();
	//ui.label_2->hide();

	ui.labelDay->setText(tr("infinite"));
}

void PermitPage::showPermitition()
{
	int active = DataBase::getInstance()->queryData("bActive").toInt();
	if(active == 0)
		showDeActive();
	else
		showActive();
}




