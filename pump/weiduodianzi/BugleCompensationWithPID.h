#pragma once
#include "buglecompensation.h"
class BugleCompensationWithPID :
	public BugleCompensation
{
	typedef struct PID
	{
		qint32 nSetPoint;		//设定目标
		qint32 nInteError;		//误差累计(积分)
		double dKp;				//比例常数Proportional Const
		double dKi;				//积分常数 IntegralConst
		double dKd;				//微分常数Derivative Const
		qint32 nLastError;		//E[k]
		qint32 nPrevError;		//E[k-1]
	} PID;

public:
	BugleCompensationWithPID(QObject *parent = 0);
	~BugleCompensationWithPID();

public:
	void updateOutput(quint32 output, double flow);				//更新输出;
	void adjustOutputByInput(qint32 inputData);	//根据输入调节输出(此函数应该在采集输入时候被调用);
	void stopOutput();								//停止输出;
	void pauseOutput();								//暂停输出;	
	void waitOutputStable();						//等待输出稳定;
	void bugleSignal();								//出现了凸轮信号;
	double pidRealize(qint32 input);

private:
	bool m_bSetPointIsSet;
	PID m_pidData;									//PID数据;
	bool isOutputStable();							//判断输出是否已经稳定;
	void initPIDData();								//初始化PID数据;
};

