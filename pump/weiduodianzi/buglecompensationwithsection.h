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

	void updateOutput(quint32 output, double flow);				//�������;
	void adjustOutputByInput(qint32 inputData);	//��������������(�˺���Ӧ���ڲɼ�����ʱ�򱻵���);
	void stopOutput();								//ֹͣ���;
	void pauseOutput();								//��ͣ���;	
	void waitOutputStable();						//�ȴ�����ȶ�;
	void bugleSignal();								//������͹���ź�;

private:
	int m_nTimeoutIndex;
	QList<int>m_timeoutList;
	QList<double>m_factorList;
	QTimer *m_pTimer;
	bool isOutputStable();
	void addTimeoutAndFactor(int timeout, double factor);
	void updateTimeoutList(double flow);						//���²������;
	

private slots:
	void bugleTimeOut();
};

#endif // BUGLECOMPENSATIONWITHSECTION_H
