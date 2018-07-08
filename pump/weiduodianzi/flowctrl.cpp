#include "flowctrl.h"

FlowCtrl::FlowCtrl(QObject *parent)
	: QObject(parent)
	, m_dFlowPercent(0)
{

}


FlowCtrl::~FlowCtrl()
{

}


FlowCtrl * FlowCtrl::getInstance()
{
	static FlowCtrl *pFlow = NULL;
	if(!pFlow)
	{
		pFlow = new FlowCtrl();
	}
	return pFlow;
}


void FlowCtrl::updateFlowPercentInPcMode( quint32 percent )
{
	static quint32 tpercent = 100000;

	if(tpercent == percent)
		return;
	else
		tpercent = percent;

	QString temp;
	if(percent == 0)
	{
		temp = "100%";
	}
	else
	{
		temp = QString("%1").arg(percent -1);
	}
	emit(updatePercentDisplay(temp));
}


void FlowCtrl::updateFlowPercent( quint32 percent )
{
	static quint32 tpercent = 100000;

	if(tpercent == percent)
		return;
	else
		tpercent = percent;

	QString temp = QString("%1").arg(percent -1);
	m_dFlowPercent = (double)(percent)/100.0;

	emit(updatePercentDisplay(temp));
}

void FlowCtrl::updateFlowInPcMode( double flow )
{

}

void FlowCtrl::enableUpdateFlow()
{

}

void FlowCtrl::disableUpdateFlow()
{

}
