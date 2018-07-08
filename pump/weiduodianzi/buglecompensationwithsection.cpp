#include "buglecompensationwithsection.h"
#include "timehelper.h"
#include <QDebug>

#define OUTPUT_STABLE_PERIOD_MSEC	100

BugleCompensationWithSection::BugleCompensationWithSection(QObject *parent)
	:BugleCompensation(parent)
	,m_nTimeoutIndex(0)
{
	m_factorList<<1<<0.9<<0.8<<0.9<<1<<1.1<<1.2<<1.1<<1;
	//m_factorList<<1<<0.5<<0.1<<0.5<<1<<1.5<<2<<1.5<<1;

	m_pTimer = new QTimer(this);
	m_pTimer->setSingleShot(true);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(bugleTimeOut()) );
}

BugleCompensationWithSection::~BugleCompensationWithSection()
{

}

void BugleCompensationWithSection::updateOutput(quint32 output, double flow)
{
	m_bOutputStable = false;
	m_nCurrentOutput = output;

	if(flow > 10000)//大于1ml时候屏蔽补偿
	{
		m_nTimeoutIndex = 0;
		m_pTimer->stop();
		return;
	}

	updateTimeoutList(flow);				//更新补偿表格;
	waitOutputStable();
}

void BugleCompensationWithSection::adjustOutputByInput(qint32 inputData)
{

}

void BugleCompensationWithSection::stopOutput()
{
	m_nCurrentOutput = 0;
	m_bOutputStable = false;
	m_pTimer->stop();
	return;
}

void BugleCompensationWithSection::pauseOutput()
{
	m_bOutputStable = false;
	return;
}

void BugleCompensationWithSection::waitOutputStable()
{
	//延时;(延时周期在本文件开头配置,单位ms)
	TimeHelper::mDelay(OUTPUT_STABLE_PERIOD_MSEC);
	m_bOutputStable = true;
}

void BugleCompensationWithSection::bugleSignal()
{
	m_bBugleSignal = true;
	//按表格更新梯度?
	m_nTimeoutIndex = 0;
	m_pTimer->stop();

	if(isOutputStable())//证明正在运行;
		m_pTimer->start(m_timeoutList.at(m_nTimeoutIndex));
}


void BugleCompensationWithSection::bugleTimeOut()
{
	quint32 output = m_nCurrentOutput*m_factorList.at(m_nTimeoutIndex);
	emit(outputUpdate(output));
	//qDebug()<<m_timeoutList.at(m_nTimeoutIndex);
	m_nTimeoutIndex++;
	m_nTimeoutIndex %= m_timeoutList.count();
	if(m_nTimeoutIndex < m_timeoutList.count())
	{
		m_pTimer->start(m_timeoutList.at(m_nTimeoutIndex));
	}
}

void BugleCompensationWithSection::addTimeoutAndFactor(int timeout, double factor)
{
	m_timeoutList.append(timeout);
	m_factorList.append(factor);
}

bool BugleCompensationWithSection::isOutputStable()
{
	return m_bOutputStable;
}

//addTimeoutAndFactor(0, 1);
//addTimeoutAndFactor(2080, 0.9);
//addTimeoutAndFactor(2310, 0.8);
//addTimeoutAndFactor(2600, 0.9);
//addTimeoutAndFactor(2310, 1);
//addTimeoutAndFactor(2080, 1.1);
//addTimeoutAndFactor(1890, 1.2);
//addTimeoutAndFactor(1730, 1.1);
//addTimeoutAndFactor(1890, 1);
void BugleCompensationWithSection::updateTimeoutList(double flow)
{
	m_timeoutList.clear();
	m_timeoutList.append(0);

	QList<double>::const_iterator it;
	for (it = m_factorList.begin(); it!= m_factorList.end(); it++)
	{
		double dFactor = *it;
		double temp = 10000.0/(8.0*0.6*flow*dFactor);
		temp*=1000;
		m_timeoutList.append(temp);
	}
	m_timeoutList.removeLast();
	//qDebug()<<m_factorList;
	//qDebug()<<m_timeoutList;
}
