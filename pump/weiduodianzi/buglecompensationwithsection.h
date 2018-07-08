#ifndef BUGLECOMPENSATIONWITHSECTION_H
#define BUGLECOMPENSATIONWITHSECTION_H

#include "buglecompensation.h"
#include <QTimer>


class BugleCompensationWithSection : public BugleCompensation
{
	Q_OBJECT

public:
	BugleCompensationWithSection(QObject *parent = 0);
	~BugleCompensationWithSection();

	void updateOutput(quint32 output, double flow);				//更新输出;
	void adjustOutputByInput(qint32 inputData);	//根据输入调节输出(此函数应该在采集输入时候被调用);
	void stopOutput();								//停止输出;
	void pauseOutput();								//暂停输出;	
	void waitOutputStable();						//等待输出稳定;
	void bugleSignal();								//出现了凸轮信号;

private:
	int m_nTimeoutIndex;
	QList<int>m_timeoutList;
	QList<double>m_factorList;
	QTimer *m_pTimer;
	bool isOutputStable();
	void addTimeoutAndFactor(int timeout, double factor);
	void updateTimeoutList(double flow);						//更新补偿表格;
	

private slots:
	void bugleTimeOut();
};

#endif // BUGLECOMPENSATIONWITHSECTION_H
