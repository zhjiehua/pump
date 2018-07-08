#include "GradientTable.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <QStandardItemModel>
#include "mytablemodel.h"
#include "editordelegate.h"
#include "msgbox.h"

extern int g_nActScreenX;
extern int g_nActScreenY;

GradientTable::GradientTable(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Gradient"), index, previndex, add, parent )
	, m_nWhich(add)
{
	ui.setupUi(this);
	m_strTitle = tr("Gradient");
	initDisplay();
	setStyleSheet( BTNGLOBALSTYLE);

	MachineStat::getInstance()->pwdNeed = false;
}


GradientTable::~GradientTable(void)
{
	MachineStat::getInstance()->pwdNeed = true;
}

void GradientTable::initFocusList()
{
	xList.append(ui.tableView);
	xList.append(ui.saveBtn);
	xList.append(ui.backBtn);

	yList.append(ui.tableView);
	yList.append(ui.saveBtn);
	yList.append(ui.backBtn);
}

bool GradientTable::eventFilter( QObject *obj, QEvent *event )
{
	return false;
}

void GradientTable::initDisplay()
{
	MyTableModel *gradient_model = new MyTableModel(this, 2, m_nWhich);
	gradient_model->readDataFromTable(m_nWhich);
 	ui.tableView->setModel(gradient_model);
	if(g_nActScreenX < 420)
	{
		ui.tableView->setColumnWidth(0,80);    
		ui.tableView->setColumnWidth(1,80);
	}
	else
	{
		ui.tableView->setColumnWidth(0,80*2);    
		ui.tableView->setColumnWidth(1,80*2);
	}
    
	//ui.tableView->setColumnWidth(2,80);    
	//ui.tableView->setColumnWidth(3,80);
	ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.tableView->setEditTriggers(QAbstractItemView::AnyKeyPressed);

	//为每一列设置代理;
	ui.tableView->setItemDelegateForColumn(0, new EditorDelegate("000.0"));//时间;
	if(DataBase::getInstance()->queryData("pumptype").toInt() == 0)//10ml泵
		ui.tableView->setItemDelegateForColumn(1, new EditorDelegate("0000.0000"));//流速;
	else
		ui.tableView->setItemDelegateForColumn(1, new EditorDelegate("0000.000"));//流速;
	//ui.tableView->setItemDelegateForColumn(1, new EditorDelegate("0000.0000"));//流速;
	
	//ui.tableView->setItemDelegateForColumn(2, new EditorDelegate("000.0"));//A;
	//ui.tableView->setItemDelegateForColumn(3, new EditorDelegate("000.0"));//B;
 	connect(ui.tableView, SIGNAL(getOutFocus(int)), this, SLOT(getOutTableFocus(int)));


}

void GradientTable::getOutTableFocus(int dir)
{
	if(dir == 0)//上
		ui.backBtn->setFocus();
	else if(dir == 1)//下;
		ui.saveBtn->setFocus();
	else if(dir == 2)//左
		ui.backBtn->setFocus();
	else if(dir == 3)//右
		ui.saveBtn->setFocus();
}


void GradientTable::on_saveBtn_clicked()
{
	MyTableModel *mod = static_cast<MyTableModel *>(ui.tableView->model());
	DataBase::getInstance()->updateGradientTable(m_nWhich, mod->tableData);
	MsgBox msgBox(this,tr("Tips"), tr("save success!"), QMessageBox::Ok);
	msgBox.exec();
}


void GradientTable::on_backBtn_clicked()
{
	g_pMainWindow->backToPage();
}

CBasePage* GradientTable::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new GradientTable(parent, index, previndex, add);
}


