#include "setuppage.h"
#include <QDebug>
#include "baseMainPage.h"
#include <QKeyEvent>

#define TABLE_GRADIENT_1	0
//
SetupPage::SetupPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("SetupPage"), index, previndex, add,parent )
{
	ui.setupUi(this);
	m_strTitle = tr("Setup");
	ui.globalBtn->setStyleSheet("QPushButton{border-image:url(:/weiduodianzi/ui/global.png);}""QPushButton:focus{border-image:url(:/weiduodianzi/ui/globalFocus.png);outline: 0px;}");
	ui.calBtn->setStyleSheet("QPushButton{border-image:url(:/weiduodianzi/ui/Calibration.png);}""QPushButton:focus{border-image:url(:/weiduodianzi/ui/CalibrationFocus.png);outline: 0px;}");
	ui.permitBtn->setStyleSheet("QPushButton{border-image:url(:/weiduodianzi/ui/Permission.png);}""QPushButton:focus{border-image:url(:/weiduodianzi/ui/PermissionFocus.png);outline: 0px;}");
	ui.timeBtn->setStyleSheet("QPushButton{border-image:url(:/weiduodianzi/ui/clock.png);}""QPushButton:focus{border-image:url(:/weiduodianzi/ui/clockFocus.png);outline: 0px}");
	ui.msgBtn->setStyleSheet("QPushButton{border-image:url(:/weiduodianzi/ui/message.png);}""QPushButton:focus{border-image:url(:/weiduodianzi/ui/messageFocus.png);outline: 0px}");
    ui.netBtn->setStyleSheet("QPushButton{border-image:url(:/weiduodianzi/ui/netconfig.png);}""QPushButton:focus{border-image:url(:/weiduodianzi/ui/netconfigFocus.png);outline: 0px}");
	ui.glpBtn->setStyleSheet("QPushButton{border-image:url(:/weiduodianzi/ui/glpinfo.png);}""QPushButton:focus{border-image:url(:/weiduodianzi/ui/glpinfoFocus.png);outline: 0px}");
    ui.gridBtn->setStyleSheet("QPushButton{border-image:url(:/weiduodianzi/ui/grid.png);}""QPushButton:focus{border-image:url(:/weiduodianzi/ui/gridFocus.png);outline: 0px}");
}

SetupPage::~SetupPage()
{

}


void SetupPage::on_globalBtn_clicked()
{
	g_pMainWindow->changePage( LANGUAGEPAGE_INDEX );
}
void SetupPage::on_calBtn_clicked()
{
	g_pMainWindow->changePage( FIXPAGE_INDEX );
}

void SetupPage::on_permitBtn_clicked()
{
	g_pMainWindow->changePage( PERMITPAGE_INDEX );
}
void SetupPage::on_timeBtn_clicked()
{
	g_pMainWindow->changePage( TIMEPAGE_INDEX );
}
void SetupPage::on_msgBtn_clicked()
{
	g_pMainWindow->changePage( MSGPAGE_INDEX );
}

void SetupPage::on_netBtn_clicked()
{
    g_pMainWindow->changePage( NETPAGE_INDEX );
}

void SetupPage::on_gridBtn_clicked()
{
    g_pMainWindow->changePage(GRADIENTTABLE_INDEX, TABLE_GRADIENT_1);
}

void SetupPage::on_glpBtn_clicked()
{
    g_pMainWindow->changePage(GLPINFOPAGE_INDEX);
}

void SetupPage::initFocusList()
{
	xList.append(ui.globalBtn);
	xList.append(ui.calBtn);
	xList.append(ui.permitBtn);
    xList.append(ui.glpBtn);
	xList.append(ui.timeBtn);
    xList.append(ui.gridBtn);
	xList.append(ui.msgBtn);
    xList.append(ui.netBtn);

	yList.append(ui.globalBtn);
	yList.append(ui.timeBtn);
	yList.append(ui.calBtn);
	yList.append(ui.gridBtn);
	yList.append(ui.permitBtn);
	yList.append(ui.msgBtn);
    yList.append(ui.glpBtn);
    yList.append(ui.netBtn);
}



CBasePage* SetupPage::getInstance(QWidget *parent, quint8 index, quint8 prevIndex, quint32 add)
{
	return new SetupPage(parent, index, prevIndex, add);
}










