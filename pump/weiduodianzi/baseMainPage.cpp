
#include "baseMainPage.h"
#include <QPainter>
#include <QDebug>
#include <QDate>
#include <QMouseEvent>
#include <QVBoxLayout>
#include "Common.h"
#include "BasePage.h"
#include "runpage.h"
#include "setuppage.h"
#include "runconst.h"
#include "runparam.h"
#include "pwdpage.h"
#include "language.h"
#include "msgpage.h"
#include "permitpage.h"
#include "runpage.h"
#include "setuppage.h"
#include "PressFixPage.h"
#include "SensitivePage.h"
#include "DebugMcuProtoPage.h"
#include "FlowFixPage.h"
#include "TimePage.h"
#include "FixPage.h"
#include "logopage.h"
#include "AdminPage.h"
#include "GradientPage.h"
#include "GradientTable.h"
#include "netconfigpage.h"
#include "glpinfopage.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QTime>
#include <math.h>

extern bool bSyncFlag;

#ifndef WIN32
#define NAVIGATORCNT	3
#else
#define NAVIGATORCNT	4
#endif



BaseMainPage *g_pMainWindow;

BaseMainPage::BaseMainPage(QString strTitle, QWidget *parent)
	: QMainWindow(parent)
	, m_strTitle(strTitle)
	, m_nPageIndex(STARTUPPAGE_INDEX)
	, m_pCtrlPage(0)
	,adminFlag(false)
	,m_bNavigatorMode(true)
	,m_nNavigatorCnt(NAVIGATORCNT)
	,m_nCurrentNavigator(0)
{
	ui.setupUi(this);
	g_pMainWindow = this;
	initShotCut();
	initMachine();
	initTranslation();
	initTimer();
	initPage();
	backToPage();
}



BaseMainPage::~BaseMainPage()
{
	DataBase::getInstance()->saveDb();
}



//********************************************************初始化********************************************************
void BaseMainPage::initMachine()
{
	m_pMachine = MachineStat::getInstance();
	connect(m_pMachine, SIGNAL(machineStatChanged(MachineStat::MachineStatment)), this, SLOT(machineStatChanged(MachineStat::MachineStatment)));
}


void BaseMainPage::initTimer()
{
	pAdminTimer = new QTimer(this);
	connect(pAdminTimer, SIGNAL(timeout()), this, SLOT(timeoutFunc()));
	pAdminTimer->start(1000);
}


void BaseMainPage::initTranslation()
{
	m_pTranslator = new QTranslator();
	m_pTranslator->load("weiduodianzi_zh.qm", ":/weiduodianzi");

	int nLanguage = DataBase::getInstance()->queryData("language").toInt();
	installLanguage(nLanguage);
}

void BaseMainPage::initShotCut()
{
	QShortcut *pShortCut = new QShortcut(this);
	pShortCut->setKey(KEY_UP);
	connect(pShortCut, SIGNAL(activated()), this, SLOT(focusNextUpChild()));
	shortCutList.append(pShortCut);

	pShortCut = new QShortcut(this);
	pShortCut->setKey(KEY_DOWN);
	connect(pShortCut, SIGNAL(activated()), this, SLOT(focusNextDownChild()));
	shortCutList.append(pShortCut);

	pShortCut = new QShortcut(this);
	pShortCut->setKey(KEY_LEFT);
	connect(pShortCut, SIGNAL(activated()), this, SLOT(focusNextLeftChild()));
	shortCutList.append(pShortCut);

	pShortCut = new QShortcut(this);
	pShortCut->setKey(KEY_RIGHT);
	connect(pShortCut, SIGNAL(activated()), this, SLOT(focusNextRightChild()));
	shortCutList.append(pShortCut);

	pShortCut = new QShortcut(this);
	pShortCut->setKey(KEY_BACKSPACE);
	connect(pShortCut, SIGNAL(activated()), this, SLOT(backToPage()));
	shortCutList.append(pShortCut);

	pShortCut = new QShortcut(this);
	pShortCut->setKey(KEY_PUMPSTOP);
	connect(pShortCut, SIGNAL(activated()), this, SLOT(shortCutPumpStop()));
	shortCutList.append(pShortCut);

	pShortCut = new QShortcut(this);
	pShortCut->setKey(KEY_STARTHOLD);
	connect(pShortCut, SIGNAL(activated()), this, SLOT(shortCutStartHold()));
	shortCutList.append(pShortCut);

	pShortCut = new QShortcut(this);
	pShortCut->setKey(KEY_PURGE);
	connect(pShortCut, SIGNAL(activated()), this, SLOT(shortCutPurge()));
	shortCutList.append(pShortCut);

	pShortCut = new QShortcut(this);
	pShortCut->setKey(KEY_SUPER);
	connect(pShortCut, SIGNAL(activated()), this, SLOT(shortCutSuper()));
	shortCutList.append(pShortCut);
}

void BaseMainPage::initPage()
{
	ui.midWidget->addPage(RunPage::getInstance, RUNPAGE_INDEX);
	ui.midWidget->addPage(RunParamPage::getInstance, RUNPARAMPAGE_INDEX);
	ui.midWidget->addPage(SetupPage::getInstance, SETUPPAGE_INDEX);
	ui.midWidget->addPage(AdminPage::getInstance, RUNPAGE_INDEX);
	ui.midWidget->addPage(PwdPage::getInstance, RUNPAGE_INDEX);
	ui.midWidget->addPage(LanguagePage::getInstance, SETUPPAGE_INDEX);
	ui.midWidget->addPage(MsgPage::getInstance, SETUPPAGE_INDEX);
	ui.midWidget->addPage(PermitPage::getInstance, SETUPPAGE_INDEX);
	ui.midWidget->addPage(FixPage::getInstance, SETUPPAGE_INDEX);
	ui.midWidget->addPage(FlowFixPage::getInstance, FIXPAGE_INDEX);
	ui.midWidget->addPage(TimePage::getInstance, SETUPPAGE_INDEX);
	ui.midWidget->addPage(PressFixPage::getInstance, FIXPAGE_INDEX);
	ui.midWidget->addPage(GradientPage::getInstance, RUNPARAMPAGE_INDEX);
	ui.midWidget->addPage(GradientTable::getInstance, SETUPPAGE_INDEX);
	ui.midWidget->addPage(DebugMcuProtoPage::getInstance, RUNPAGE_INDEX);
	ui.midWidget->addPage(LogoPage::getInstance, RUNPAGE_INDEX);
	ui.midWidget->addPage(NetConfigPage::getInstance, SETUPPAGE_INDEX);
    ui.midWidget->addPage(GlpInfoPage::getInstance, SETUPPAGE_INDEX);
}




//***************************************************************逻辑处理******************************************************
#define CHECK_TRYOUT
void BaseMainPage::changePage(quint8 id, quint32 add)
{
	//判断id范围，有效性检查;

	if(id < RUNPAGE_INDEX || id > MAXPAGEINDEX)
		return;

#ifdef 	CHECK_TRYOUT
	//检查是否已经到达试用时间;
	if( MachineStat::getInstance()->isTryDone() )
	{
		/*if(m_pCtrlPage)
		{
			if( (m_pCtrlPage->GetPageIndex() == ADMINPAGE_INDEX || m_pCtrlPage->GetPageIndex() == PWDPAGE_INDEX) && id == PWDPAGE_INDEX)
			{
				return;
			}
			else if( m_pCtrlPage->GetPageIndex() == ADMINPAGE_INDEX && id != PWDPAGE_INDEX )
			{
				id = PWDPAGE_INDEX;
			}
			else if(id != PWDPAGE_INDEX && id!=ADMINPAGE_INDEX)
				return;
		}*/
		if(m_pCtrlPage)
		{
			if( (m_pCtrlPage->GetPageIndex() == PERMITPAGE_INDEX ) && id == PERMITPAGE_INDEX)
			{
				return;
			}
			else if( m_pCtrlPage->GetPageIndex() == PERMITPAGE_INDEX && id != PERMITPAGE_INDEX )
			{
				return;
			}
			else if(id != PERMITPAGE_INDEX)
				return;
		}
	}

#endif

	m_pCtrlPage = ui.midWidget->changePage(id, add);

	if(id != PWDPAGE_INDEX)
		m_nPageIndex = id;

	if(m_pCtrlPage)
	{
		ui.topWidget->setTitle(m_pCtrlPage->GetTitle());
	}
	return;
}



void BaseMainPage::backToPage()
{
	//张杰华添加@2016-06-28，如果在PCCTRL模式，则不能切换
	if(bSyncFlag)
		return;

	int idNext;
	//清除当前页面;
	if(m_pCtrlPage)
	{
		idNext = m_pCtrlPage->GetPrevPageIndex();
	}else
		idNext = STARTUPPAGE_INDEX;

	if(idNext== SETUPPAGE_INDEX || idNext== RUNPAGE_INDEX ||idNext== RUNPARAMPAGE_INDEX)
		setNavigatorMode(true);
	else
		changePage( idNext );
}



void BaseMainPage::changeLanguage(const int nLang)
{
	installLanguage(nLang);
	QMessageBox::information(this, tr("Tips"), tr("Change Language Success"));
	DataBase::getInstance()->updateDate("language", QString::number(nLang));
}


void BaseMainPage::machineStatChanged(MachineStat::MachineStatment stat)
{
	//切换对应页面;
	if(m_pCtrlPage)
	{
		if(m_pCtrlPage->GetPageIndex() != RUNPAGE_INDEX)
		{
			navigatorPageAt(0, true);
		}
	}
}



/*********************************************按键逻辑相关*********************************************/

bool BaseMainPage::checkPermission()
{
	return (MachineStat::getInstance()->getCurrentStat() ==  MachineStat::STOP);
}


void BaseMainPage::focusNextLeftChild()
{
	//张杰华添加@2016-06-28，如果在PCCTRL模式，则不能切换
	if(bSyncFlag)
		return;

	if(!m_pCtrlPage)
		return;

	if(isNavigatorMode())
	{
		if(m_nNavigatorCnt <= 0)
			return;

		int tIndex = 0;
		m_nCurrentNavigator == 0 ? tIndex = m_nNavigatorCnt-1 : tIndex = m_nCurrentNavigator-1;
		navigatorPageAt(tIndex);
		return;
	}

	QObjectList &list = m_pCtrlPage->xList;
	if(list.length() == 0)
		return;

	QObject *obj = m_pCtrlPage->focusWidget();
	int index = list.indexOf(obj);
	if(index == -1)
		index = 0;
	if(index == 0)
	{
		index = list.length();
	}
	index--;
	QWidget *tab =static_cast<QWidget *>(list.at(index));
	tab->setFocus();
}

void BaseMainPage::focusNextRightChild()
{
	//张杰华添加@2016-06-28，如果在PCCTRL模式，则不能切换
	if(bSyncFlag)
		return;

	if(!m_pCtrlPage)
		return;
	if(isNavigatorMode())
	{
		if(m_nNavigatorCnt <= 0)
			return;

		int tindex = (m_nCurrentNavigator+1)%m_nNavigatorCnt;
		navigatorPageAt(tindex);
		return;
	}

	QObjectList list = m_pCtrlPage->xList;
	if(list.length() == 0)
		return;

	QObject *obj = m_pCtrlPage->focusWidget();
	int index = list.indexOf(obj);
	if(index == -1)
		index = list.length()-1;
	if(index == list.length()-1)
	{
		index = -1;
	}
	index++;
	QWidget *tab =static_cast<QWidget *>(list.at(index));
	tab->setFocus();
}

void BaseMainPage::focusNextUpChild()
{
	//张杰华添加@2016-06-28，如果在PCCTRL模式，则不能切换
	if(bSyncFlag)
		return;

	if(!m_pCtrlPage)
		return;
	if(isNavigatorMode())
	{
		setNavigatorMode(false);
	}

	QObjectList &list = m_pCtrlPage->yList;
	if(list.length() == 0)
		return;

	QObject *obj = m_pCtrlPage->focusWidget();

	int index = list.indexOf(obj);
	if(index == -1 || index == 0)
	{
		index = list.length();
	}
	index--;
	QWidget *tab =static_cast<QWidget *>(list.at(index));
	tab->setFocus();
}


void BaseMainPage::focusNextDownChild()
{
	//张杰华添加@2016-06-28，如果在PCCTRL模式，则不能切换
	if(bSyncFlag)
		return;

	if(!m_pCtrlPage)
		return;
	if(isNavigatorMode())
	{
		setNavigatorMode(false);
	}

	QObjectList &list = m_pCtrlPage->yList;
	if(list.length() == 0)
		return;

	QObject *obj = m_pCtrlPage->focusWidget();
	int index = list.indexOf(obj);
	if(index == -1 || index == list.length()-1)
	{
		index = -1;
	}
	index++;
	QWidget *tab =static_cast<QWidget *>(list.at(index));
	tab->setFocus();
}

void BaseMainPage::setNavigatorMode(bool mode)
{
	m_bNavigatorMode = mode; 
	if(mode)
		navigatorPageAt(m_nCurrentNavigator);
}

void BaseMainPage::changeNavigator(quint8 index)
{
	if(index >= m_nNavigatorCnt)
		return;

	m_nCurrentNavigator = index;
}

void BaseMainPage::setShortCutDisable(bool disable)
{
	for (int i = 0; i < shortCutList.count(); i++)
	{
		shortCutList.at(i)->setEnabled(!disable);
	}
}

void BaseMainPage::shortCutPumpStop()
{
	//张杰华添加@2016-06-21，如果在PCCTRL模式，则不能切换
	if(bSyncFlag)
		return;

	MachineStat::MachineStatment stat;
	if(MachineStat::getInstance()->getCurrentStat()!=MachineStat::STOP)
		stat = MachineStat::STOP;
	else
		stat = MachineStat::PUMP;

	MachineStat::getInstance()->setMachineStat(stat);
}

void BaseMainPage::shortCutStartHold()
{
	//张杰华添加@2016-06-21，如果在PCCTRL模式，则不能切换
	if(bSyncFlag)
		return;

	//MachineStat::MachineStatment stat;
	//if(MachineStat::getInstance()->getCurrentStat() == MachineStat::STOP)
	//	stat = MachineStat::RUNNING;//运行;
	//else if(MachineStat::getInstance()->getCurrentStat() != MachineStat::PAUSE)
	//	stat = MachineStat::PAUSE;//暂停;
	//else//如果当前状态为暂停，则回复之前状态;
	//{
	//	//恢复 运行、泵夜或清洗;
	//	MachineStat::getInstance()->restoreOldStat();
	//	return;
	//}
	MachineStat::MachineStatment stat, current_state;
	current_state = MachineStat::getInstance()->getCurrentStat();
	if(current_state == MachineStat::STOP || current_state == MachineStat::PAUSE)//张杰华修改@2016-06-25
		stat = MachineStat::RUNNING;//运行;
	else if(current_state == MachineStat::RUNNING)
		stat = MachineStat::PAUSE;//暂停;
	else
		return;

	MachineStat::getInstance()->setMachineStat(stat);
}

void BaseMainPage::shortCutPurge()
{
	//张杰华添加@2016-06-21，如果在PCCTRL模式，则不能切换
	if(bSyncFlag)
		return;

	//张杰华修改@2016-06-25
	MachineStat::MachineStatment stat, current_state;
	current_state = MachineStat::getInstance()->getCurrentStat();
	if(current_state != MachineStat::PURGE && current_state == MachineStat::STOP)
		stat = MachineStat::PURGE;
	else
		return;

	MachineStat::getInstance()->setMachineStat(stat);
}



void  BaseMainPage::navigatorPageAt(int index, bool force)
{
	if(index < 0 || index >=4)
		return;

	if(!checkPermission() && !force)
		return;

	changeNavigator(index);

	QList<int> navigatorPageIndex;
	navigatorPageIndex.append(RUNPAGE_INDEX);
	navigatorPageIndex.append(RUNPARAMPAGE_INDEX);
	navigatorPageIndex.append(SETUPPAGE_INDEX);
	navigatorPageIndex.append(DEBUGPRESS_INDEX);

	ui.bottomWidget->changeNavigatorDisp(index);//改变导航栏显示;
	changePage(navigatorPageIndex.at(index));

}

void BaseMainPage::shortCutSuper()
{
	MachineStat::getInstance()->whichPage = ADMINPAGE_INDEX;
	MachineStat::getInstance()->usrType = MachineStat::ADMIN;
	changePage(PWDPAGE_INDEX);
}

void BaseMainPage::installLanguage( const int nLang )
{
	if(nLang == CHINESE)
		qApp->installTranslator(m_pTranslator);
	else
		qApp->removeTranslator(m_pTranslator);

	ui.bottomWidget->updateLanguage();
	ui.topWidget->setTitle(tr("Language"));
}





















