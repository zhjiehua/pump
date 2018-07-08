#include "DebugMcuProtoPage.h"
#include "baseMainPage.h"
#include "editordelegate.h"
#include "database.h"
#include "iomodule.h"
//#include <qwt_plot_magnifier.h>
//#include <qwt_plot_panner.h>


#define X_AXIS_EXTEND	300
#define Y_AXIS_EXTEND	6


#ifdef linux
#define PRESSPATH	"/sdcard/press.txt"
#else
#define PRESSPATH	"./press.txt"
#endif

DebugMcuProtoPage::DebugMcuProtoPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("DebugMcuProtoPage"), index, previndex, add, parent )
	, m_bCollectFlag(false)
	, m_bStartFlag(false)
	, m_nBulgeFlag(0)
	, m_dPeriod(0)
{
	ui.setupUi(this);
	m_strTitle = tr("DebugPressPage");
	initDisplay();
	//图表初始化;
	initPlotDisp();
}


DebugMcuProtoPage::~DebugMcuProtoPage(void)
{
	if(m_pTime)
		delete m_pTime;

	if(m_bStartFlag)
		MachineStat::getInstance()->setMachineStat(MachineStat::STOP, true);
}

void DebugMcuProtoPage::initFocusList()
{
	xList.append(ui.tableView);
	xList.append(ui.flowEdit);
	xList.append(ui.startBtn);
	xList.append(ui.clearBtn);
	xList.append(ui.collectBtn);
	xList.append(ui.plotBtn);
	xList.append(ui.bugleBtn);

	yList.append(ui.tableView);
	yList.append(ui.flowEdit);
	yList.append(ui.startBtn);
	yList.append(ui.clearBtn);
	yList.append(ui.collectBtn);
	yList.append(ui.plotBtn);
	yList.append(ui.bugleBtn);

	setStyleSheet(BTNGLOBALSTYLE);
}



CBasePage* DebugMcuProtoPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new DebugMcuProtoPage(parent, index, previndex, add);
}

void DebugMcuProtoPage::initDisplay()
{
	gradient_model = new MyTableModel(this, 2);
	ui.tableView->setModel(gradient_model);
	ui.tableView->setColumnWidth(0,85);    
	ui.tableView->setColumnWidth(1,85);    
	ui.tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui.tableView->setEditTriggers(QAbstractItemView::AnyKeyPressed);

	//为每一列设置代理;
	ui.tableView->setItemDelegateForColumn(0, new EditorDelegate("000.0000"));//压力;
	ui.tableView->setItemDelegateForColumn(1, new EditorDelegate("000.0000"));//校准;
	ui.tableView->initIndex();

	//隐藏线图;
	//ui.qwtPlot->hide();

	/*int array[7]={42, 25 , 20, 20, 15, 10, 10};
	int pumpType = DataBase::getInstance()->queryData("pumptype").toInt();
	if(pumpType > 6)
	pumpType = 0;
	double maxFlow = array[pumpType];*/
	double maxFlow = MachineStat::getInstance()->getMaxFlow();
	ui.flowEdit->setValRange("flowRate", 0, maxFlow, 4);

	connect(IoModule::getInstance(), SIGNAL(bulge()), this, SLOT(dealBulge()));
	connect(ui.tableView, SIGNAL(getOutFocus(int)), this, SLOT(getOutTableFocus(int)));
	connect(MachineStat::getInstance(), SIGNAL(updatePressDisplay(QString, quint8)), this, SLOT(updatePressVal(QString, quint8)));
	m_pTime = new QTime();

}

void DebugMcuProtoPage::getOutTableFocus(int dir)
{
	if(dir == 0)
		ui.bugleBtn->setFocus();
	else if(dir == 1)
		ui.flowEdit->setFocus();
	else if(dir == 2)
		ui.flowEdit->setFocus();
	else if(dir == 3)
		ui.flowEdit->setFocus();
}

void DebugMcuProtoPage::updatePressVal(QString press, quint8 add)
{
	if(m_bCollectFlag)
	{
		QStringList list;
		double timeSpec = (double)m_pTime->elapsed()/1000.0;
		list.append(QString::number(timeSpec));
		list.append(press);
		list.append(QString::number(getBulge()));
		int row = gradient_model->rowCount();
		gradient_model->insertRow(row, MyTableModel::USERDATA, list);
		if(row >= 1000)
		{
			gradient_model->removeRow(0);
		}
			
		ui.tableView->scrollToBottom();

		addDataToCurve(timeSpec, press.toDouble());
	}
	
}

void DebugMcuProtoPage::on_collectBtn_clicked()
{
	m_bCollectFlag = !m_bCollectFlag;
	if(m_bCollectFlag)
	{
		m_pTime->restart();
	}
	else
	{
		m_curveData.clear();
		//保存文件;

		DataBase::getInstance()->saveTableToFile(QString(PRESSPATH), gradient_model->getTableData());
	}
}

void DebugMcuProtoPage::on_clearBtn_clicked()
{
	MachineStat::getInstance()->clearPress();
}

void DebugMcuProtoPage::on_startBtn_clicked()
{
	m_bStartFlag = !m_bStartFlag;
	if(m_bStartFlag)
	{
		MachineStat::getInstance()->updateFlow(ui.flowEdit->text().toDouble(), MachineStat::DEBUG_MODE);//开始
		ui.startBtn->setText(tr("Stop"));
		
	}
	else
	{
		ui.startBtn->setText(tr("Start"));
		MachineStat::getInstance()->setMachineStat(MachineStat::STOP, true);
	}
}

void DebugMcuProtoPage::dealBulge()
{
	m_nBulgeFlag = 1;
}

int DebugMcuProtoPage::getBulge()
{
	if(m_nBulgeFlag)
	{
		m_nBulgeFlag = 0;
		return 1;
	}
	else
		return 0;
}

void DebugMcuProtoPage::on_plotBtn_clicked()
{
	static bool bDispTable = true;
	bDispTable = !bDispTable;
	if(bDispTable)
	{
		ui.tableView->show();
		//ui.qwtPlot->hide();
	}
	else
	{
		ui.tableView->hide();
		//ui.qwtPlot->show();
	}
}

void DebugMcuProtoPage::addDataToCurve(double x, double y)
{
	m_curveData.append(QPointF(x, y));
	if( m_curveData.count() > X_AXIS_EXTEND )
	{
		double x = m_curveData.first().rx();
		m_curveData.remove(0);
	}
#if 0
	m_curve.setSamples(m_curveData);
	ui.qwtPlot->replot();
#endif
}

void DebugMcuProtoPage::initPlotDisp()
{
#if 0
	m_curve.setSamples(m_curveData);
	//加到plot，plot由IDE创建
	m_curve.attach(ui.qwtPlot);
	m_curve.setCurveAttribute(QwtPlotCurve::Fitted, true);//是曲线更光滑  
	m_curve.setPen(QPen(Qt::blue));//设置画笔  
	//ui.qwtPlot->setAxisScale(QwtPlot::yLeft, 0, 10);
	ui.qwtPlot->setAxisScale(QwtPlot::xBottom, 0, X_AXIS_EXTEND);
	ui.qwtPlot->setAxisScale(QwtPlot::yLeft, 0, Y_AXIS_EXTEND);
	//(void) new QwtPlotMagnifier( ui.qwtPlot->canvas() );
	//(void) new QwtPlotPanner( ui.qwtPlot->canvas() );
#endif
}

void DebugMcuProtoPage::on_bugleBtn_clicked()
{
	dealBulge();
	MachineStat::getInstance()->dealBulge();
}





