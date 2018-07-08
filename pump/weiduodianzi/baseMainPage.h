#ifndef WEIDUODIANZI_H
#define WEIDUODIANZI_H

#include <QtGui/QMainWindow>
#include <QTranslator>
#include <QVBoxLayout>
#include "ui_BaseMainPage.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QShortcut>
#include "machinestat.h"




class CBasePage;

class BaseMainPage : public QMainWindow
{
	Q_OBJECT

public:
	BaseMainPage(QString strTitle, QWidget *parent = 0);
	~BaseMainPage();

	void changePage(quint8 id, quint32 add = 0);				//页面切换;
	void changeLanguage(const int nLang);						//切换语言;
	void setShortCutDisable(bool disable);						//禁止快捷键;
	void navigatorPageAt(int index, bool force = false);		//改变导航页;
	quint8 GetPageIndex()		{ return m_nPageIndex; }

private:
	Ui::BaseMainPage ui;
	QString m_strTitle;						//标题;
	int m_nPageIndex;						//页面ID;
	CBasePage *m_pCtrlPage;					//控件页面指针;
	QTranslator *m_pTranslator;				//翻译;

	bool adminFlag;
	QTimer *pAdminTimer;
	MachineStat *m_pMachine;

	bool m_bNavigatorMode;
	int m_nNavigatorCnt;				//标签页数目;
	int m_nCurrentNavigator;			//当前导航页;
	QList<QShortcut *>shortCutList;		//快捷键列表;

	void initMachine();					//初始化机器状态;
	void initTimer();					//初始化定时功能;
	void initTranslation();				//初始化翻译功能;
	void initPage();					//初始化页面;
	void initShotCut();					//初始化按键逻辑;

	bool checkPermission();				//检查是否允许切换页面;
	void installLanguage(const int nLang);

	//导航功能;
	bool isNavigatorMode(){return m_bNavigatorMode;}
	void setNavigatorMode(bool mode);
	inline void changeNavigator(quint8 index);							//改变导航索引;
	
	
	

private slots:
	void machineStatChanged(MachineStat::MachineStatment stat);

public slots:
	//按键操作;
	void focusNextLeftChild();					//向左切换焦点;
	void focusNextRightChild();					//向右切换焦点;
	void focusNextUpChild();					//向上切换焦点;
	void focusNextDownChild();					//向下切换焦点;
	void shortCutPumpStop();					//PUMP/STOP快捷键;
	void shortCutStartHold();					//START/HOLD快捷键;
	void shortCutPurge();						//Purge快捷键;
	void shortCutSuper();						//Super快捷键跳至高级页面;
	void backToPage();							//返回上一页;
	



};

extern BaseMainPage *g_pMainWindow;

#endif // WEIDUODIANZI_H
