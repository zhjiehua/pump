#include "PressFixPage.h"
#include "mytablemodel.h"
#include "editordelegate.h"
#include "msgbox.h"
#include "baseMainPage.h"

extern int g_nActScreenX;
extern int g_nActScreenY;

PressFixPage::PressFixPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Press Calibration"), index, previndex, add, parent )
	, flag(false)
{
	ui.setupUi(this);
	m_strTitle = tr("Press Calibration");
	initDisplay();
}


PressFixPage::~PressFixPage(void)
{
	if(flag)
	{
		MachineStat::getInstance()->setMachineStat(MachineStat::STOP, true);
		pTimer->stop();
		MachineStat::getInstance()->setFixMode(false);
	}

	DataBase::getInstance()->readPressCompensationTableIntoList();
}

void PressFixPage::initFocusList()
{
	xList.append(ui.tableView);
	xList.append(ui.flowEdit);
	xList.append(ui.clearBtn);
	xList.append(ui.startBtn);
	xList.append(ui.saveBtn);
	xList.append(ui.backBtn);

	yList.append(ui.tableView);
	yList.append(ui.saveBtn);
	yList.append(ui.flowEdit);
	yList.append(ui.clearBtn);
	yList.append(ui.startBtn);
	yList.append(ui.backBtn);
}


void PressFixPage::initDisplay()
{
	MyTableModel *gradient_model = new MyTableModel(this, 2);
	gradient_model->readDataFromTable(21);
	ui.tableView->setModel(gradient_model);
	if(g_nActScreenX < 420)
	{
		ui.tableView->setColumnWidth(0,65);    
		ui.tableView->setColumnWidth(1,65);  
	}
	else
	{
		ui.tableView->setColumnWidth(0,65*2);    
		ui.tableView->setColumnWidth(1,65*2); 
	}
	  
	ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.tableView->setEditTriggers(QAbstractItemView::AnyKeyPressed);

	//为每一列设置代理;
	ui.tableView->setItemDelegateForColumn(0, new EditorDelegate("000.0000"));//压力;
	ui.tableView->setItemDelegateForColumn(1, new EditorDelegate("000.0000"));//压力校准;
	ui.tableView->initIndex();

	connect(ui.tableView, SIGNAL(getOutFocus(int)), this, SLOT(getOutTableFocus(int)));
	connect(MachineStat::getInstance(),SIGNAL(updatePressDisplay(QString, quint8)), this, SLOT(updatePresVal(QString, quint8)));
	setStyleSheet( BTNGLOBALSTYLE);

	/*int array[7]={42, 25 , 20, 20, 15, 10, 10};
	int pumpType = DataBase::getInstance()->queryData("pumptype").toInt();
	if(pumpType > 6)
	pumpType = 0;
	double maxFlow = array[pumpType];*/
	double maxFlow = MachineStat::getInstance()->getMaxPress();
	//ui.flowEdit->setValRange("flowRate", 0, maxFlow, 4);
	if(DataBase::getInstance()->queryData("pumptype").toInt() == 0)//10ml泵
		ui.flowEdit->setValRange("pumpPurgeFlowVal", 0, maxFlow, 4);//张杰华修改@2016-06-28，将小数点位数改成1个，无效？？
	else
		ui.flowEdit->setValRange("pumpPurgeFlowVal", 0, maxFlow, 3);

	pTimer = new QTimer(this);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(updateFlow()));

}

void PressFixPage::getOutTableFocus(int dir)
{
	if(dir == 0)//上
		ui.backBtn->setFocus();
	else if(dir == 1)//下;
		ui.saveBtn->setFocus();
	else if(dir == 2)//左
		ui.backBtn->setFocus();
	else if(dir == 3)//右
		ui.flowEdit->setFocus();

	updatePressList();
}

void PressFixPage::on_saveBtn_clicked()
{
	MyTableModel *mod = static_cast<MyTableModel *>(ui.tableView->model());
	DataBase::getInstance()->updatePressCompensationTable(mod->tableData);
	MsgBox msgBox(this,tr("Tips"), tr("save success!"), QMessageBox::Ok);
	msgBox.exec();
}

void PressFixPage::on_backBtn_clicked()
{
	g_pMainWindow->backToPage();
}


void PressFixPage::on_startBtn_clicked()
{
	flag = !flag;
	if(flag)
	{
		MachineStat::getInstance()->updateFlow(ui.flowEdit->text().toDouble(), MachineStat::DEBUG_MODE);//开始
		ui.startBtn->setText(tr("Stop"));
		//pTimer->start(100);
		MachineStat::getInstance()->setFixMode(true);
		MachineStat::getInstance()->clearPressWarn();
	}
	else
	{
		ui.startBtn->setText(tr("Start"));
		MachineStat::getInstance()->setMachineStat(MachineStat::STOP, true);
		MachineStat::getInstance()->setFixMode(false);
		pTimer->stop();
	}
}

void PressFixPage::updatePresVal(QString disp, quint8 warning)
{
	ui.pressLbl->setText(disp);
	//qDebug()<<"update press!!!";
}

void PressFixPage::updatePressList()
{
	MyTableModel *mod = static_cast<MyTableModel *>(ui.tableView->model());
	DataBase::getInstance()->readPressCompensationTableIntoList(mod->tableData);
}

void PressFixPage::on_clearBtn_clicked()
{
	MachineStat::getInstance()->clearPress();
}

CBasePage* PressFixPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new PressFixPage(parent, index, previndex, add);
}

void PressFixPage::updateFlow()
{
	if(!flag)
		return;
	
	MachineStat::getInstance()->updateFlow(ui.flowEdit->text().toDouble(), MachineStat::DEBUG_MODE);//开始
}





