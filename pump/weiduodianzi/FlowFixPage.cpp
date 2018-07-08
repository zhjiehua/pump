#include "FlowFixPage.h"
#include <QPainter>
#include "mytablemodel.h"
#include "editordelegate.h"
#include "msgbox.h"
#include "database.h"
#include "machinestat.h"

extern int g_nActScreenX;
extern int g_nActScreenY;

FlowFixPage::FlowFixPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Flow Calibration"), index, previndex, add, parent )
	, flag(false)
{
	ui.setupUi(this);
	m_strTitle = tr("Flow Calibration");
	initDisplay();
	setStyleSheet( BTNGLOBALSTYLE);
}


FlowFixPage::~FlowFixPage(void)
{
	if(flag)
	{
		MachineStat::getInstance()->setMachineStat(MachineStat::STOP, true);
		MachineStat::getInstance()->setFixMode(false);
	}
	DataBase::getInstance()->readFlowCompensationTableIntoList();
}



void FlowFixPage::initDisplay()
{
	MyTableModel *gradient_model = new MyTableModel(this, 2);
	gradient_model->readDataFromTable(TABLE_FLOW_FIX);
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
 	ui.tableView->setItemDelegateForColumn(0, new EditorDelegate("0000.0000"));//流速;
 	ui.tableView->setItemDelegateForColumn(1, new EditorDelegate("000.0000"));//校准;
	ui.tableView->initIndex();
  
 	connect(ui.tableView, SIGNAL(getOutFocus(int)), this, SLOT(getOutTableFocus(int)));
	connect(MachineStat::getInstance(),SIGNAL(updatePressDisplay(QString, quint8)), this, SLOT(updatePresVal(QString, quint8)));

	pTimer = new QTimer(this);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(timeoutFunc()));

	//根据泵头的选择，设置流速最大值;
	/*int array[7]={42, 25 , 20, 20, 15, 10, 10};
	int pumpType = DataBase::getInstance()->queryData("pumptype").toInt();
	if(pumpType > 6)
	pumpType = 0;
	double maxFlow = array[pumpType];*/
	double maxFlow = MachineStat::getInstance()->getMaxFlow();
	//ui.flowEdit->setValRange("flowRate", 0, maxFlow, 4);
	if(DataBase::getInstance()->queryData("pumptype").toInt() == 0)//10ml泵
		ui.flowEdit->setValRange("pumpPurgeFlowVal", 0, maxFlow, 4);//张杰华修改@2016-06-28，将小数点位数改成1个，无效？？
	else
		ui.flowEdit->setValRange("pumpPurgeFlowVal", 0, maxFlow, 3);
}

void FlowFixPage::updateFlowList()
{
	MyTableModel *mod = static_cast<MyTableModel *>(ui.tableView->model());
	DataBase::getInstance()->readFlowCompensationTableIntoList(mod->tableData);
}

void FlowFixPage::on_saveBtn_clicked()
{
	MyTableModel *mod = static_cast<MyTableModel *>(ui.tableView->model());
	DataBase::getInstance()->updateFlowCompensationTable(mod->tableData);
	MsgBox msgBox(this,tr("Tips"), tr("save success!"), QMessageBox::Ok);
	msgBox.exec();
}



void FlowFixPage::on_startBtn_clicked()
{
	flag = !flag;
	if(flag)
	{
		quint32 timeMin[] = {1, 2, 3, 5, 10, 30};
		quint32 index = ui.timeCombo->currentIndex();
		//ui.timeCombo->setCurrentIndex(0);
		//QTimer::singleShot(60000*(index+1), this, SLOT(singleTimeOut()));

		pTimer->start(60000*timeMin[index]);

		MachineStat::getInstance()->updateFlow(ui.flowEdit->text().toDouble(), MachineStat::DEBUG_MODE);//开始
		ui.startBtn->setText(tr("Stop"));
		MachineStat::getInstance()->setFixMode(true);
		MachineStat::getInstance()->clearPressWarn();
	}
	else
	{
		pTimer->stop();
		ui.startBtn->setText(tr("Start"));
		MachineStat::getInstance()->setMachineStat(MachineStat::STOP, true);
		MachineStat::getInstance()->setFixMode(false);
	}
}

void FlowFixPage::updatePresVal(QString disp, quint8 warning)
{
	ui.pressLbl->setText(disp);
}


void FlowFixPage::getOutTableFocus( int dir )
{
	if(dir == 0)//上
		ui.backBtn->setFocus();
	else if(dir == 1)//下;
		ui.saveBtn->setFocus();
	else if(dir == 2)//左
		ui.backBtn->setFocus();
	else if(dir == 3)//右
		ui.flowEdit->setFocus();
	updateFlowList();
}

void FlowFixPage::on_backBtn_clicked()
{
	g_pMainWindow->backToPage();
}


CBasePage* FlowFixPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new FlowFixPage(parent, index, previndex, add);
}

void FlowFixPage::initFocusList()
{
	xList.append(ui.tableView);
	xList.append(ui.flowEdit);
	xList.append(ui.timeCombo);
	xList.append(ui.startBtn);
	xList.append(ui.saveBtn);
	xList.append(ui.backBtn);

	yList.append(ui.tableView);
	yList.append(ui.saveBtn);
	yList.append(ui.flowEdit);
	yList.append(ui.timeCombo);
	yList.append(ui.startBtn);
	yList.append(ui.backBtn);
}

void FlowFixPage::timeoutFunc()
{
	pTimer->stop();
	flag = !flag;
	ui.startBtn->setText(tr("Start"));
	MachineStat::getInstance()->setMachineStat(MachineStat::STOP, true);
	MachineStat::getInstance()->setFixMode(false);
}
