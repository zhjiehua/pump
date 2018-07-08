#include "runpage.h"
#include <QDebug>
#include <QTimer>
#include <QTime>
#include "baseMainPage.h"

RunPage::RunPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Run"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = tr("Run");
	initDisplay();

	setStyleSheet("QLineEdit:focus{border:2px solid blue;outline:0px;}");
}

RunPage::~RunPage()
{

}


void RunPage::initFocusList()
{
	xList.append(ui.rateFlow);
 	xList.append(ui.statCombo);
	
	yList.append(ui.rateFlow);
 	yList.append(ui.statCombo);
}

void RunPage::updateTime(quint32 startupTime)
{
	//更新开机时间;
	int hour = startupTime/3600;
	int minute = (startupTime%3600)/60;
	int second = (startupTime%3600)%60;
	
	QString startup;
	startup = QString("%1:%2:%3").arg(hour, 2, 10, QChar('0')).arg(minute, 2, 10, QChar('0')).arg(second, 2, 10, QChar('0'));
	ui.rtLabel->setText(startup);
}

void RunPage::initDisplay()
{
	ui.rateFlow->setText(DataBase::getInstance()->queryData("pumpPurgeFlowVal"));
	//ui.rateFlow->setText(DataBase::getInstance()->queryData("flowrate"));
	ui.statCombo->setCurrentIndex(MachineStat::getInstance()->getCurrentStat());
	//ui.gradCombo->installEventFilter(this);

	/*int array[7]={42, 25 , 20, 20, 15, 10, 10};
	int pumpType = DataBase::getInstance()->queryData("pumptype").toInt();
	if(pumpType > 6)
		pumpType = 0;
	double maxFlow = array[pumpType];*/
	double maxFlow = MachineStat::getInstance()->getMaxFlow();
	if(DataBase::getInstance()->queryData("pumptype").toInt() == 0)//10ml泵
		ui.rateFlow->setValRange("pumpPurgeFlowVal", 0, maxFlow, 4);//张杰华修改@2016-06-28，将小数点位数改成1个，无效？？
	else
		ui.rateFlow->setValRange("pumpPurgeFlowVal", 0, maxFlow, 3);

	//setStyleSheet( BTNGLOBALSTYLE);

	//实时值;
	connect(MachineStat::getInstance(),SIGNAL(updatePressDisplay(QString, quint8)), this, SLOT(updatePresVal(QString, quint8)));
	connect(MachineStat::getInstance(), SIGNAL(updateFlowDisplay(QString)), this, SLOT(updateFlowVal(QString)));
	connect(MachineStat::getInstance(), SIGNAL(machineStatChanged(MachineStat::MachineStatment)), this,  SLOT(updateMachineStat(MachineStat::MachineStatment)));
	connect(MachineStat::getInstance(), SIGNAL(updatePercentDisplay(QString)), this, SLOT(updatePercentVal( QString)));
	connect(MachineStat::getInstance(), SIGNAL(updateStartupTimeDisplay(quint32)), this, SLOT(updateTime( quint32)));
//	connect(ui.gradCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateGradient(int)));
	connect(ui.statCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeMachineStat(int)));
	connect(ui.rateFlow, SIGNAL(dataChanging(QString)), this, SLOT(changeFlow(QString)) );

	//如果PC端更新了流速则显示最新的PC设置值;
	MachineStat::getInstance()->isUpdateFlowFromPC();
}



void RunPage::updateFlowVal( QString disp )
{
	ui.rateFlow->setText(disp);
}

void RunPage::updateGradient(int index)
{
	if(index < 10)
		DataBase::getInstance()->updateDate("currentGradient", QString::number(index));
}

void RunPage::updateMachineStat( MachineStat::MachineStatment stat)
{
	ui.statCombo->setCurrentIndex(stat);
}

bool RunPage::eventFilter(QObject *obj, QEvent *event)
{

	//if(obj == ui.gradCombo)
	//{
	//	if(event->type() == QEvent::KeyPress)
	//	{
	//		QKeyEvent *ke = static_cast<QKeyEvent *>(event);
	//		if(ke->key() == Qt::Key_Return)
	//		{
	//			//查找当前的机器状态;
	//			//当运行状态处于运行、泵液、清洗、暂停时，锁定梯度栏不能被修改
	//			if(MachineStat::getInstance()->getCurrentStat() != MachineStat::STOP)
	//				return true;
	//			else
	//				return false; //继续传递
	//		}
	//		else
	//			return CBasePage::eventFilter(obj, event);
	//	}
	//	else
	//		return CBasePage::eventFilter(obj, event);
	//}
	//else
		return CBasePage::eventFilter(obj, event);

}

void RunPage::updatePresVal(QString disp, quint8 warning)
{
	ui.press->setText(disp);
}

void RunPage::changeMachineStat( int stat)
{
	if(stat == 5)//PC
		DataBase::getInstance()->updateDate("gradientTable", "10");
	else
		DataBase::getInstance()->updateDate("gradientTable",DataBase::getInstance()->queryData("currentGradient"));

	MachineStat::getInstance()->setMachineStat((MachineStat::MachineStatment)stat);
}

void RunPage::updatePercentVal( QString disp )
{
	if(disp.indexOf("%") == -1)//吴杰能添加@2016-06-19
	{
		disp=disp + "%";
	}

	ui.percentLbl->setText(disp);

}

CBasePage* RunPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new RunPage(parent, index, previndex, add);
}

void RunPage::changeFlow( QString str )
{
	if(MachineStat::getInstance()->getCurrentStat() == MachineStat::PUMP || MachineStat::getInstance()->getCurrentStat() == MachineStat::PURGE)
		MachineStat::getInstance()->updateFlow(str.toDouble(), MachineStat::PUMP_PURGE_MODE);
}

