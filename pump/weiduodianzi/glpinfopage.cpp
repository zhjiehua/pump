#include "glpinfopage.h"
#include <QDebug>
#include "baseMainPage.h"
#include <QApplication>
#include <QKeyEvent>

//#define TIME_UNIT_BASE 1//单位是s，测试用;
#define TIME_UNIT_BASE 3600//单位小时，真正用;

static quint32 curPumpType;

GlpInfoPage::GlpInfoPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("GLP Information"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = tr("GLP Information");
    setStyleSheet(BTNGLOBALSTYLE);

	QTimer *pTimer = new QTimer(this);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(updateUsedTime()));
	pTimer->start(1000);
}

GlpInfoPage::~GlpInfoPage()
{

}


void GlpInfoPage::initFocusList()
{
	xList.append(ui.pumpMod);

	yList.append(ui.pumpMod);

	ui.pumpMod->setVar(QString("pumptype"));//泵类型;

	connect(ui.pumpMod, SIGNAL(currentIndexChanged(int)), this, SLOT(pumpChange(int)));

    ////栓塞更新绑定;
    connect(MachineStat::getInstance(), SIGNAL(updateBugleCnt(quint32)), this, SLOT(updateBugleDisp(quint32)) );
    ui.bltUsedLbl->setText(DataBase::getInstance()->queryData("bugleCnt"));

	curPumpType = DataBase::getInstance()->queryData("pumptype").toInt();
	if(MachineStat::getInstance()->pwdOK)
		MachineStat::getInstance()->pwdNeed = false;
	else
		MachineStat::getInstance()->pwdNeed = true;
	MachineStat::getInstance()->pwdOK = false;

	ui.pumpMod->installEventFilter(this);

	QString temp = DataBase::getInstance()->queryData("manufYear");
	temp += "-";
	temp += DataBase::getInstance()->queryData("manufMonth");
	temp += "-";
	temp += DataBase::getInstance()->queryData("manufDay");
	ui.manufLabel->setText(temp);

	temp = DataBase::getInstance()->queryData("instYear");
	temp += "-";
	temp += DataBase::getInstance()->queryData("instMonth");
	temp += "-";
	temp += DataBase::getInstance()->queryData("instDay");
	ui.instLabel->setText(temp);

	temp = DataBase::getInstance()->queryData("repairYear");
	temp += "-";
	temp += DataBase::getInstance()->queryData("repairMonth");
	temp += "-";
	temp += DataBase::getInstance()->queryData("repairDay");
	ui.repairLabel->setText(temp);

	updateUsedTime();
}

CBasePage* GlpInfoPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new GlpInfoPage(parent, index, previndex, add);
}


void GlpInfoPage::updateBugleDisp(quint32 cnt)
{
	quint32 pumpType = DataBase::getInstance()->queryData("pumptype").toInt();
	QList<double> pumpVolume;

	//pumpVolume<<60.8057<<248.1608<<1<<1<<1<<1<<1; //单位uL
	//double totalFluid = pumpVolume.at(pumpType) * cnt/1000;

	//pumpVolume<<0.0608057<<0.2481608<<1<<1<<1<<1<<1; //单位uL
	pumpVolume<<0.0608057<<0.2481608<<1<<1<<1<<1<<1<<1<<1<<1; //单位uL
	double totalFluid = pumpVolume.at(pumpType) * cnt;
    ui.bltUsedLbl->setText(QString::number(cnt));
	ui.totalFluidLbl->setText(QString::number(totalFluid));
}

void GlpInfoPage::pumpChange(int pumpType)
{
	//if(MachineStat::getInstance()->pwdNeed)
	//{
	//	ui.pumpMod->setCurrentIndex(curPumpType);
	//	MachineStat::getInstance()->whichPage = g_pMainWindow->GetPageIndex();
	//	MachineStat::getInstance()->usrType = MachineStat::USER;
	//	g_pMainWindow->changePage(PWDPAGE_INDEX);
	//	return;
	//}

	MachineStat::getInstance()->pumpTypeChanged(true);
	MachineStat::getInstance()->m_bPumpTypeChange = true;

	//更新最大最小压力
	//int array[7]={42, 25 , 20, 20, 15, 10, 10};
	int maxPressArray[11]={42, 25 , 20, 20, 20, 15, 15, 10, 10, 10, 10};
	QString temp = QString::number(maxPressArray[pumpType]);
	DataBase::getInstance()->updateDate("maxpress", temp);

	//更新最小压力
	int minPress = DataBase::getInstance()->queryData("minpress").toInt();
	if(minPress > maxPressArray[pumpType])
		minPress = 0;
	temp = QString::number(minPress);
	DataBase::getInstance()->updateDate("minpress", QString(temp));
}

bool GlpInfoPage::eventFilter(QObject *obj, QEvent *event)
{
	if(event->type() == QEvent::KeyPress )
	{
		QKeyEvent *ke = (QKeyEvent *)(event);
		if( ke->key() == Qt::Key_Return && MachineStat::getInstance()->pwdNeed)
		{
			MachineStat::getInstance()->whichPage = g_pMainWindow->GetPageIndex();
			MachineStat::getInstance()->usrType = MachineStat::ADMIN;
			g_pMainWindow->changePage(PWDPAGE_INDEX);
			return true;
		}
	}

	return false;
}

#define YEAR_SECOND	(60×60×24×365)
void GlpInfoPage::updateUsedTime()
{
	int totalSec = MachineStat::getInstance()->getTime(MachineStat::SYSTIME);
	int totalHour = totalSec/TIME_UNIT_BASE;
	int totalDay = totalHour/24;
	totalHour%=24;
	int totalMin = totalSec % TIME_UNIT_BASE / 60 ;
	totalSec = totalSec % TIME_UNIT_BASE % 60 ;

	QString strDisp = QString::number(totalDay)+tr("Day")+"  "+QString::number(totalHour)+tr("Hour")+"  "+QString::number(totalMin)+tr("Min")+"  "+QString::number(totalSec)+tr("Sec");
	ui.usedTimeLbl->setText(strDisp);
}
