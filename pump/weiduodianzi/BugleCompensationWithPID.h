#pragma once
#include "buglecompensation.h"
class BugleCompensationWithPID :
	public BugleCompensation
{
	typedef struct PID
	{
		qint32 nSetPoint;		//�趨Ŀ��
		qint32 nInteError;		//����ۼ�(����)
		double dKp;				//��������Proportional Const
		double dKi;				//���ֳ��� IntegralConst
		double dKd;				//΢�ֳ���Derivative Const
		qint32 nLastError;		//E[k]
		qint32 nPrevError;		//E[k-1]
	} PID;

public:
	BugleCompensationWithPID(QObject *parent = 0);
	~BugleCompensationWithPID();

public:
	void updateOutput(quint32 output, double flow);				//�������;
	void adjustOutputByInput(qint32 inputData);	//��������������(�˺���Ӧ���ڲɼ�����ʱ�򱻵���);
	void stopOutput();								//ֹͣ���;
	void pauseOutput();								//��ͣ���;	
	void waitOutputStable();						//�ȴ�����ȶ�;
	void bugleSignal();								//������͹���ź�;
	double pidRealize(qint32 input);

private:
	bool m_bSetPointIsSet;
	PID m_pidData;									//PID����;
	bool isOutputStable();							//�ж�����Ƿ��Ѿ��ȶ�;
	void initPIDData();								//��ʼ��PID����;
};

