#include "bottomwidget.h"
#include <QDebug>
#include "baseMainPage.h"
#include <QShortcut>
#include "machinestat.h"

#define BTNNORMALSTYLE "QPushButton{border-image:url(:/weiduodianzi/ui/normalNavigator.png);}""QPushButton{border: 2px solid green;outline: 0px;}"
#define BTNFOCUSSTYLE "QPushButton:focus{border-image:url(:/weiduodianzi/ui/focusNavigator.png);border: 2px solid green;outline: 0px;}""QPushButton{border-image:url(:/weiduodianzi/ui/activeNavigator.png);border: 2px solid green;outline: 0px;}"

#define WEEPINGSTYLE  "QLabel{border-image:url(:/weiduodianzi/ui/weeping.png);}"

#define LOWPRESSSTYLE  "QLabel{border-image:url(:/weiduodianzi/ui/down.png);}"
#define HIGHPRESSSTYLE  "QLabel{border-image:url(:/weiduodianzi/ui/up.png);}"

#define CONNECTSTYLE  "QLabel{border-image:url(:/weiduodianzi/ui/connect_established.png);}"
#define DISCONNECTSTYLE  "QLabel{border-image:url(:/weiduodianzi/ui/disconnect.png);}"

BottomWidget::BottomWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initNavigator();
	connect(MachineStat::getInstance(), SIGNAL(systemError(int, QString)), this, SLOT(updateWarning(int, QString)));

	//QPalette pa;
	//pa.setColor(QPalette::WindowText, Qt::red);
	//ui.label->setPalette(pa);
	updateWarning(0, tr("No Warn"));
}



BottomWidget::~BottomWidget()
{

}


void BottomWidget::initNavigator()
{
	m_btnList.append(ui.runBtn);
	m_btnList.append(ui.paramBtn);
	m_btnList.append(ui.setBtn);
#ifdef WIN32
	//m_btnList.append(ui.dbgBtn);
	ui.dbgBtn->hide();
#else
	ui.dbgBtn->hide();
#endif
}





void BottomWidget::on_runBtn_clicked()
{
	g_pMainWindow->navigatorPageAt(0);
}



void BottomWidget::on_paramBtn_clicked()
{
	g_pMainWindow->navigatorPageAt(1);
}



void BottomWidget::on_setBtn_clicked()
{
	g_pMainWindow->navigatorPageAt(2);
}



void BottomWidget::on_dbgBtn_clicked()
{
	g_pMainWindow->navigatorPageAt(3);
}




void BottomWidget::changeNavigatorDisp(quint8 index)
{
	for(int i = 0; i < m_btnList.count(); i++)
	{
		if(i != index)
			m_btnList.at(i)->setStyleSheet(BTNNORMALSTYLE);
		else
		{
			m_btnList.at(i)->setStyleSheet(BTNFOCUSSTYLE);
			m_btnList.at(i)->setFocus();
		}
	}

}

void BottomWidget::updateLanguage()
{
	ui.runBtn->setText(tr("Run"));
	ui.paramBtn->setText(tr("Param"));
	ui.setBtn->setText(tr("Setup"));
	ui.dbgBtn->setText(tr("Debug"));
	MachineStat::getInstance()->updateWarning();//Îâ½ÜÄÜÌí¼Ó@2016-06-18
}

extern double overPress;

void BottomWidget::updateWarning( int err, QString str)
{
	////QPalette pa;
	//if(err == 0)
	//{
	//	//pa.setColor(QPalette::WindowText, Qt::blue);
	//	ui.label->setText("");
	//}
	//else
	//{
	//	//pa.setColor(QPalette::WindowText, Qt::red);
	//	//ui.label->setPalette(pa);
	//	ui.label->setText(str);
	//}

	if(err == NOWARN)
	{
		ui.label->setStyleSheet(CONNECTSTYLE);
		ui.pressLabel->setStyleSheet("");
		ui.weepingLabel->setStyleSheet("");
	}
	else if(err == COMUNICATION_ERR)
	{
		ui.label->setStyleSheet(DISCONNECTSTYLE);
	}
	else if(err == OVERPRESS_ERR)
	{
		if(overPress < DataBase::getInstance()->queryData("minpress").toDouble())
			ui.pressLabel->setStyleSheet(LOWPRESSSTYLE);
		else
			ui.pressLabel->setStyleSheet(HIGHPRESSSTYLE);
	}
	else if(err == WEEPING)
	{
		ui.weepingLabel->setStyleSheet(WEEPINGSTYLE);
	}
}






