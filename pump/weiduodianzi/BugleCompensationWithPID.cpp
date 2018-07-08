#include "BugleCompensationWithPID.h"
#include "timehelper.h"
#include <QDebug>

#define OUTPUT_STABLE_PERIOD_MSEC	100
#define CONST_KP					0.5
#define CONST_KI					0.2
#define CONST_KD					0.5

#define PRESS_DELTA					20.0
#define FLOW_FACTOR					20615

BugleCompensationWithPID::BugleCompensationWithPID(QObject *parent)
	:BugleCompensation(parent)
	,m_bSetPointIsSet(false)
{

}


BugleCompensationWithPID::~BugleCompensationWithPID()
{
}

void BugleCompensationWithPID::updateOutput(quint32 output, double flow)
{
	//更新当前输出量;
	m_nCurrentOutput = output;
	//复位设定值被设标志位;
	m_bSetPointIsSet = false;
	//复位输出稳定;
	m_bOutputStable = false;
	//清空PID数据;
	initPIDData();
	//等待输出稳定;
	waitOutputStable();

}

void BugleCompensationWithPID::adjustOutputByInput(qint32 inputData)
{
	//如果输出目前未稳定，调节没有意义;
	if( !isOutputStable() )
		return;

	//什么条件时候将inputData选定为标准参考值;
	if(!m_bSetPointIsSet)
	{
		m_pidData.nSetPoint = inputData;
		m_bSetPointIsSet = true;
		return;
	}

	//根据PID数据与标准参考值进行计算输出;
	quint32 ret = pidRealize(inputData);

	//更新输出到MCU;
	emit(outputUpdate(ret));
}

bool BugleCompensationWithPID::isOutputStable()
{
	return m_bOutputStable;
}

void BugleCompensationWithPID::initPIDData()
{
	m_pidData.nInteError= 0;
	m_pidData.nLastError= 0;					//Error[k]
	m_pidData.nPrevError= 0;					//Error[k-1]
	m_pidData.dKp= CONST_KP;	//比例常数Proportional Const
	m_pidData.dKi= CONST_KI;		//积分常数IntegralConst
	m_pidData.dKd= CONST_KD;	//微分常数Derivative Const
	m_pidData.nSetPoint= 0;
}

double BugleCompensationWithPID::pidRealize(qint32 input)
{
	qint32 err;
	double ret;
	err = m_pidData.nSetPoint-input;     
	m_pidData.nInteError += err; 
	double percent = ((double)err)/PRESS_DELTA;
	ret = m_pidData.dKp*(percent*FLOW_FACTOR) + m_nCurrentOutput;//目前只做比例控制;
	//ret = m_pidData.dKp*err + m_pidData.dKi*m_pidData.nInteError+m_nCurrentOutput;// + m_pidData.dKd*(err-m_pidData.nLastError)+m_nCurrentOutput;      
	m_pidData.nLastError = err;      
	return ret;
}

void BugleCompensationWithPID::stopOutput()
{
	m_nCurrentOutput = 0;
	//复位设定值被设标志位;
	m_bSetPointIsSet = false;
	//复位输出稳定;
	m_bOutputStable = false;
	//复位PID数据;
	initPIDData();
}

void BugleCompensationWithPID::pauseOutput()
{
	//复位输出稳定;
	m_bOutputStable = false;
	//记录设定值;
	qint32 set = m_pidData.nSetPoint;
	//复位PID数据;
	initPIDData();
	//恢复设定值;
	m_pidData.nSetPoint = set;
}

void BugleCompensationWithPID::waitOutputStable()
{
	//延时;(延时周期在本文件开头配置,单位ms)
	TimeHelper::mDelay(OUTPUT_STABLE_PERIOD_MSEC);
	//置位稳定;
	m_bOutputStable = true;
}

void BugleCompensationWithPID::bugleSignal()
{
	return;
}
