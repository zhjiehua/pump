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

	void changePage(quint8 id, quint32 add = 0);				//ҳ���л�;
	void changeLanguage(const int nLang);						//�л�����;
	void setShortCutDisable(bool disable);						//��ֹ��ݼ�;
	void navigatorPageAt(int index, bool force = false);		//�ı䵼��ҳ;
	quint8 GetPageIndex()		{ return m_nPageIndex; }

private:
	Ui::BaseMainPage ui;
	QString m_strTitle;						//����;
	int m_nPageIndex;						//ҳ��ID;
	CBasePage *m_pCtrlPage;					//�ؼ�ҳ��ָ��;
	QTranslator *m_pTranslator;				//����;

	bool adminFlag;
	QTimer *pAdminTimer;
	MachineStat *m_pMachine;

	bool m_bNavigatorMode;
	int m_nNavigatorCnt;				//��ǩҳ��Ŀ;
	int m_nCurrentNavigator;			//��ǰ����ҳ;
	QList<QShortcut *>shortCutList;		//��ݼ��б�;

	void initMachine();					//��ʼ������״̬;
	void initTimer();					//��ʼ����ʱ����;
	void initTranslation();				//��ʼ�����빦��;
	void initPage();					//��ʼ��ҳ��;
	void initShotCut();					//��ʼ�������߼�;

	bool checkPermission();				//����Ƿ������л�ҳ��;
	void installLanguage(const int nLang);

	//��������;
	bool isNavigatorMode(){return m_bNavigatorMode;}
	void setNavigatorMode(bool mode);
	inline void changeNavigator(quint8 index);							//�ı䵼������;
	
	
	

private slots:
	void machineStatChanged(MachineStat::MachineStatment stat);

public slots:
	//��������;
	void focusNextLeftChild();					//�����л�����;
	void focusNextRightChild();					//�����л�����;
	void focusNextUpChild();					//�����л�����;
	void focusNextDownChild();					//�����л�����;
	void shortCutPumpStop();					//PUMP/STOP��ݼ�;
	void shortCutStartHold();					//START/HOLD��ݼ�;
	void shortCutPurge();						//Purge��ݼ�;
	void shortCutSuper();						//Super��ݼ������߼�ҳ��;
	void backToPage();							//������һҳ;
	



};

extern BaseMainPage *g_pMainWindow;

#endif // WEIDUODIANZI_H
