#include "GradientPage.h"
#include <QMessageBox>
#include <QKeyEvent>
#include "msgbox.h"

//Ìê¶È±í1-10
#define TABLE_GRADIENT_1	0
#define TABLE_GRADIENT_2	1
#define TABLE_GRADIENT_3	2
#define TABLE_GRADIENT_4	3
#define TABLE_GRADIENT_5	4
#define TABLE_GRADIENT_6	5
#define TABLE_GRADIENT_7	6
#define TABLE_GRADIENT_8	7
#define TABLE_GRADIENT_9	8
#define TABLE_GRADIENT_10	9



GradientPage::GradientPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Gradient"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = tr("Gradient");
	setStyleSheet( BTNGLOBALSTYLE);
}


GradientPage::~GradientPage(void)
{
	
}

void GradientPage::initFocusList()
{
	xList.append(ui.RGBtn);
	xList.append(ui.RGBtn6);
	xList.append(ui.FGBtn);
	xList.append(ui.RGBtn2);
	xList.append(ui.RGBtn7);
	xList.append(ui.CGBtn);
	xList.append(ui.RGBtn3);
	xList.append(ui.RGBtn8);
	xList.append(ui.RGBtn4);
	xList.append(ui.RGBtn9);
	//xList.append(ui.RGBtn5);
	//xList.append(ui.RGBtn10);

	yList.append(ui.RGBtn);
	yList.append(ui.RGBtn2);
	yList.append(ui.RGBtn3);
	yList.append(ui.RGBtn4);
	yList.append(ui.RGBtn5);
	yList.append(ui.RGBtn6);
	yList.append(ui.RGBtn7);
	yList.append(ui.RGBtn8);
	yList.append(ui.RGBtn9);
	yList.append(ui.RGBtn10);
	//yList.append(ui.FGBtn);
	//yList.append(ui.CGBtn);

	ui.FGBtn->hide();
	ui.CGBtn->hide();
}

void GradientPage::on_RGBtn_clicked()
{
	g_pMainWindow->changePage(GRADIENTTABLE_INDEX, TABLE_GRADIENT_1);
}

void GradientPage::on_RGBtn2_clicked()
{
	g_pMainWindow->changePage(GRADIENTTABLE_INDEX, TABLE_GRADIENT_2);
}

void GradientPage::on_RGBtn3_clicked()
{
	g_pMainWindow->changePage(GRADIENTTABLE_INDEX, TABLE_GRADIENT_3);
}

void GradientPage::on_RGBtn4_clicked()
{
	g_pMainWindow->changePage(GRADIENTTABLE_INDEX, TABLE_GRADIENT_4);
}

void GradientPage::on_RGBtn5_clicked()
{
	g_pMainWindow->changePage(GRADIENTTABLE_INDEX, TABLE_GRADIENT_5);
}

void GradientPage::on_RGBtn6_clicked()
{
	g_pMainWindow->changePage(GRADIENTTABLE_INDEX, TABLE_GRADIENT_6);
}

void GradientPage::on_RGBtn7_clicked()
{
	g_pMainWindow->changePage(GRADIENTTABLE_INDEX, TABLE_GRADIENT_7);
}

void GradientPage::on_RGBtn8_clicked()
{
	g_pMainWindow->changePage(GRADIENTTABLE_INDEX, TABLE_GRADIENT_8);
}

void GradientPage::on_RGBtn9_clicked()
{
	g_pMainWindow->changePage(GRADIENTTABLE_INDEX, TABLE_GRADIENT_9);
}

void GradientPage::on_RGBtn10_clicked()
{
	g_pMainWindow->changePage(GRADIENTTABLE_INDEX, TABLE_GRADIENT_10);
}

void GradientPage::on_FGBtn_clicked()
{
	g_pMainWindow->changePage(GRADIENTTABLE_INDEX,10);
}

void GradientPage::on_CGBtn_clicked()
{
	g_pMainWindow->changePage(GRADIENTTABLE_INDEX, 11);
}

CBasePage* GradientPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new GradientPage(parent, index, previndex, add);
}



