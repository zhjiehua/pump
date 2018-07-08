#include "runparam.h"
#include <QDebug>
#include "Common.h"
#include "baseMainPage.h"

RunParamPage::RunParamPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Parameters"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = tr("Parameters");
	initDisplay();
}

RunParamPage::~RunParamPage()
{

}



void RunParamPage::initFocusList()
{
	//xList.append(ui.pumpMod);
	xList.append(ui.maxPress);
	xList.append(ui.minPress);
	xList.append(ui.gradient);
	xList.append(ui.coefficient);

	//yList.append(ui.pumpMod);
	yList.append(ui.maxPress);
	yList.append(ui.minPress);
	yList.append(ui.gradient);
	yList.append(ui.coefficient);

	//ui.pumpMod->setVar("pumptype");//泵类型;
	ui.coefficient->setValRange("coefficient", 0, 100, 0);
	//流量补偿...........
	ui.gradient->setVar("gradient");

	//connect(ui.pumpMod, SIGNAL(currentIndexChanged(int)), this, SLOT(setMinAndMaxPress()));
}


void RunParamPage::setMinAndMaxPress()
{
	int maxPressArray[11]={42, 25 , 20, 20, 20, 15, 15, 10, 10, 10, 10};
	//int pumpType = ui.pumpMod->currentIndex();
	int pumpType = DataBase::getInstance()->queryData("pumptype").toInt(); 
	if(pumpType > 10)
		return;

	//设置最大压力输入范围与现实内容;
	QString temp;
	int pmax = DataBase::getInstance()->queryData("pmax").toInt();
	if(pmax != 0)
	{
		temp = QString::number(pmax);
		ui.maxPress->setValRange("maxpress", 0, pmax, 2);
	}
	else
	{
		temp = QString::number(maxPressArray[pumpType]);
		ui.maxPress->setValRange("maxpress", 0, maxPressArray[pumpType], 2);
	}
	ui.maxPress->setText(temp);
	DataBase::getInstance()->updateDate("maxpress", temp);

	//设置最小压力输入范围;
	ui.minPress->setValRange("minpress", -50, maxPressArray[pumpType], 2);
	int minPress = DataBase::getInstance()->queryData("minpress").toInt();
	if(minPress > maxPressArray[pumpType])
		minPress = 0;
	temp = QString::number(minPress);
	ui.minPress->setText(temp);
	DataBase::getInstance()->updateDate("minpress", temp);

	//MachineStat::getInstance()->pumpTypeChanged();
}

void RunParamPage::goToGradientPage()
{
	g_pMainWindow->changePage(GRADIENTPAGE_INDEX);
}

void RunParamPage::initDisplay()
{
	//int array[7]={42, 25 , 20, 20, 15, 10, 10};
	int maxPressArray[11]={42, 25 , 20, 20, 20, 15, 15, 10, 10, 10, 10};
	//int pumpType = ui.pumpMod->currentIndex();
	int pumpType = DataBase::getInstance()->queryData("pumptype").toInt(); 
	if(pumpType > 10)
		return;
	
	QString temp;
	float curMaxPress = DataBase::getInstance()->queryData(QString("maxpress")).toFloat();
	float pmax = DataBase::getInstance()->queryData(QString("pmax")).toFloat();
	if(pmax > 0)
	{
		temp = QString::number(pmax);
		ui.maxPress->setValRange("maxpress", 0, pmax, 2);
		if(curMaxPress > pmax)
			DataBase::getInstance()->updateDate("maxpress", temp);
	}
	else
	{
		//temp = QString::number(array[pumpType]);
		ui.maxPress->setValRange("maxpress", 0, maxPressArray[pumpType], 2);
	}
	//ui.maxPress->setValRange("maxpress", 0, array[pumpType], 2);
	ui.minPress->setValRange("minpress", -50, maxPressArray[pumpType], 2);
	setStyleSheet( BTNGLOBALSTYLE);

	if(MachineStat::getInstance()->m_bPumpTypeChange)
	{
		MachineStat::getInstance()->m_bPumpTypeChange = false;
		setMinAndMaxPress();
	}
}

CBasePage* RunParamPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new RunParamPage(parent, index, previndex, add);
}





