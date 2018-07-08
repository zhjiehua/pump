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
	//���µ�ǰ�����;
	m_nCurrentOutput = output;
	//��λ�趨ֵ�����־λ;
	m_bSetPointIsSet = false;
	//��λ����ȶ�;
	m_bOutputStable = false;
	//���PID����;
	initPIDData();
	//�ȴ�����ȶ�;
	waitOutputStable();

}

void BugleCompensationWithPID::adjustOutputByInput(qint32 inputData)
{
	//������Ŀǰδ�ȶ�������û������;
	if( !isOutputStable() )
		return;

	//ʲô����ʱ��inputDataѡ��Ϊ��׼�ο�ֵ;
	if(!m_bSetPointIsSet)
	{
		m_pidData.nSetPoint = inputData;
		m_bSetPointIsSet = true;
		return;
	}

	//����PID�������׼�ο�ֵ���м������;
	quint32 ret = pidRealize(inputData);

	//���������MCU;
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
	m_pidData.dKp= CONST_KP;	//��������Proportional Const
	m_pidData.dKi= CONST_KI;		//���ֳ���IntegralConst
	m_pidData.dKd= CONST_KD;	//΢�ֳ���Derivative Const
	m_pidData.nSetPoint= 0;
}

double BugleCompensationWithPID::pidRealize(qint32 input)
{
	qint32 err;
	double ret;
	err = m_pidData.nSetPoint-input;     
	m_pidData.nInteError += err; 
	double percent = ((double)err)/PRESS_DELTA;
	ret = m_pidData.dKp*(percent*FLOW_FACTOR) + m_nCurrentOutput;//Ŀǰֻ����������;
	//ret = m_pidData.dKp*err + m_pidData.dKi*m_pidData.nInteError+m_nCurrentOutput;// + m_pidData.dKd*(err-m_pidData.nLastError)+m_nCurrentOutput;      
	m_pidData.nLastError = err;      
	return ret;
}

void BugleCompensationWithPID::stopOutput()
{
	m_nCurrentOutput = 0;
	//��λ�趨ֵ�����־λ;
	m_bSetPointIsSet = false;
	//��λ����ȶ�;
	m_bOutputStable = false;
	//��λPID����;
	initPIDData();
}

void BugleCompensationWithPID::pauseOutput()
{
	//��λ����ȶ�;
	m_bOutputStable = false;
	//��¼�趨ֵ;
	qint32 set = m_pidData.nSetPoint;
	//��λPID����;
	initPIDData();
	//�ָ��趨ֵ;
	m_pidData.nSetPoint = set;
}

void BugleCompensationWithPID::waitOutputStable()
{
	//��ʱ;(��ʱ�����ڱ��ļ���ͷ����,��λms)
	TimeHelper::mDelay(OUTPUT_STABLE_PERIOD_MSEC);
	//��λ�ȶ�;
	m_bOutputStable = true;
}

void BugleCompensationWithPID::bugleSignal()
{
	return;
}
