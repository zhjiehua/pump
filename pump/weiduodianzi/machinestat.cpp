#include "machinestat.h"
#include <QTimer>
#include <QList>
#include <QtAlgorithms>
#include "lineuint.h"
#include "Communication/LogicThread.h"
#include "iomodule.h"
#include "buglecompensation.h"


#define _USE_MATH_DEFINES
#include <math.h>

//��λ��PC�ݶȿ��Ƴ�ʱʱ��5s����2s
#define  GRADIENTPCTIMEOUT 2

//��λ��ʱ��ͬ����λ0.01min = 0.6s;
#define SYNC_TIME_UNIT		0.6

//ѹ�����ݳش�С;
#define PRESS_POLL_SIZE		10

#define SERIAL_MAX		9999999999
#define SERIAL_NUM_KEY	19900208
#define SERIAL_NUM_VAL	345192069

//�Ƿ���ʼ��ֵ;
#define FLOW_INVALID	99999999

#define SERIAL_MAX		9999999999
#define SERIAL_NUM_KEY	19900208
#define SERIAL_NUM_VAL	345192069
#define ACTIVE_CODE_TRY	9000000000
#define ACTIVE_CODE_MAX 999999999
#define ACTIVE_MASK		0x12345678


#include "baseMainPage.h"
extern BaseMainPage *g_pMainWindow;

//����;
bool bWarn=false;
double overFlow;
double overPress;
bool bOverPress = false;//ѹ��������־

bool bSyncOverPress = false; //�Žܻ����@2016-06-23��ͬ��ʱѹ������
bool bSyncFlag = false;//��ʾ����ͬ��

#ifdef __cplusplus

extern "C" {
#endif
extern uint32 g_MachineCode;
#ifdef __cplusplus
}
#endif
/////////////////////////////////////////////////////////
//
//���ܣ�ʮ����תBCD��
//
//���룺int Dec                      ��ת����ʮ��������
//      int length                   BCD�����ݳ���
//
//�����unsigned char *Bcd           ת�����BCD��
//
//���أ�0  success
//
//˼·��ԭ��ͬBCD��תʮ����
//
//////////////////////////////////////////////////////////
int DectoBCD(int Dec,int length)
{
	int ret = 0;
	for(int i = 0; i < length/2; i++)
	{
		int temp = Dec % 100;
		temp = ((temp/10)<<4) | ((temp%10)&0xff);
		temp = temp << i*8;
		ret |= temp;
		Dec/=100;
	}
	return ret;
}


MachineStat::MachineStat(QObject *parent)
	: QObject(parent)
	, m_dCurrentflowValInPc(0)//�Žܻ��޸�@2016-06-25����-1�ĳ�0
	, m_dFlowPercent(0)
	, m_nCurrentPressRawCode(0)
	, m_sCurrentPressVal("0")
	, m_bPcGradientCtrl(false)
	, m_nPcGradientCtrlFlag(0)
	, m_bBugleFlag(false)
	, m_nFlowCtrlWord(FLOW_INVALID)
	, m_nCurrentPercent(100000)
	,m_bCanClearOverPress(false)  //�Žܻ����@2016-06-23
	,m_bPumpTypeChange(false)
	,pwdOK(false)
	,pwdNeed(false)
	,noRTCBattery(false)
{
	pDb = DataBase::getInstance();
	initIO();
	initTimeoutList();
	initTimer();
	initCommunication();
	initMachineStat();
	serialNumberGenerate();

	initMACAddr();

	initIPAddr();

	//test
	//checkPressOverloaded(1000);
}

MachineStat::~MachineStat()
{
	releaseTimeoutList();
}

MachineStat * MachineStat::getInstance()
{
	static MachineStat *pStat = new MachineStat();
	return pStat;
}

#define ISNOT_MACHINE_STAT(stat)	(stat<STOP||stat>FIX)	
void MachineStat::setMachineStat(MachineStatment stat, bool recover)
{
	if(ISNOT_MACHINE_STAT(stat))
		return;

	if(!recover)//���������֮ǰ״̬;
	{
		if(stat == m_machineStat.machineStat)			//���״̬�Ƿ����仯;
			return;

		storeOldStat(stat);//������ǰ״̬;
		emit(machineStatChanged(stat));
	}
	
	if(stat == STOP)//ֹͣ���ת��;
	{
		if(bOverPress)
		{
			bOverPress = false;
			sysError(OVERPRESS_ERR, false);//ȡ������;
		}

		stopMachine();
	}
	else if(stat == PAUSE)//��ͣ;
	{
		if(bOverPress)
		{
			bOverPress = false;
			sysError(OVERPRESS_ERR, false);//ȡ������;
		}

		pauseMachine();
	}
	else if(stat == RUNNING)//����;
	{
		if(bOverPress)
		{
			bOverPress = false;
			sysError(OVERPRESS_ERR, false);//ȡ������;
		}
		
		startMachine();
	}
	else if(stat == PUMP)//��Һ;
	{
		if(bOverPress)
		{
			bOverPress = false;
			sysError(OVERPRESS_ERR, false);//ȡ������;
		}

		pumpMachine();
	}
	else if(stat == PURGE)//��ϴ;
	{
		if(bOverPress)
		{
			bOverPress = false;
			sysError(OVERPRESS_ERR, false);//ȡ������;
		}

		purgeMachine();
	}
	else if(stat == PCCTRL)//��λ������;
	{
		if(bOverPress)
		{
			bOverPress = false;
			sysError(OVERPRESS_ERR, false);//ȡ������;
		}

		pcCtrlMachine();
	}
}

void MachineStat::timeoutFunc()
{
	//�����������С���Һ����ϴ״̬;
	for (QList<LPTimeOutStruct>::iterator it = m_timeoutList.begin(); it!=m_timeoutList.end(); it++)
	{
		LPTimeOutStruct pTimeout = *it;
		if(pTimeout->cnt++ >= pTimeout->timeout)
		{
			pTimeout->cnt = 0;
			(this->*pTimeout->timeoutFunc)();
		}

	}
}

void MachineStat::initIO()
{
	IoModule *pIo = IoModule::getInstance();
	connect(pIo, SIGNAL(bulge()), this, SLOT(dealBulge()));
	connect(pIo, SIGNAL(weeping(bool)), this, SLOT(weepingWarn(bool)));
	connect(pIo, SIGNAL(testPress(quint32)), this, SLOT(updatePresVal(quint32)) );
}


void MachineStat::initTimer()
{
	QTimer *pTimer = new QTimer(this);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(timeoutFunc()));
	pTimer->start(1000);

	/*********************************20160615***********************************/
	m_pUploadTimer = new QTimer(this);
	connect(m_pUploadTimer, SIGNAL(timeout()), this, SLOT(uploadPressTimeout_Clarity()) );
	//�����ǰ��clarityЭ�飬�����Զ�ѹ���ϴ���ʱ��;
	if(DataBase::getInstance()->queryData("pcProtocol").toInt() != 0)
		setUploadPressFreq_Clarity(1);
	/******************************************************************************/
}

void MachineStat::initMachineStat()
{
	m_machineStat.machineStat = STOP;											//��ʼ��ֹͣ����ϴ;
	m_machineStat.oldMachineStat = INISTAT;									//�ɵĻ���״̬;
	m_machineStat.startupTime = 0;											//����ʱ��
	//m_machineStat.sysUsedTime = pDb->queryData("usedTime").toInt();			//ϵͳ�ۼ�ʱ��;
	m_machineStat.pumpStartupTime = pDb->queryData("pumpUsedTime").toInt();	//���ۼ�����ʱ��;
	m_machineStat.bugleCnt = pDb->queryData("bugleCnt").toInt();			//˨��ʹ�ô�����͹�ֹ�翪�أ�
	m_machineStat.bTryDone = false;

	m_machineStat.m_bUpdateFlowFromPc = false;

	m_machineStat.m_dCurrentFlow = 0;
	m_machineStat.m_bFixMode = false;

	//��ȡ���õĵ�������;
	//m_machineStat.m_firstTryDateTime = pDb->queryData("firstTryDateTime").toUInt();//��¼��ʼ���õĵ�������ʱ��;
	//if(m_machineStat.m_firstTryDateTime == 0)
	//{
	//	clearUsedTime();
	//}
	m_machineStat.m_nSysUsedTime = pDb->queryData("usedTime").toInt();			//ϵͳ�ۼ�ʱ��;

	pumpTypeChanged();

	//͹�ֲ���;
	//connect(BugleCompensation::getInstance(), SIGNAL(outputUpdate(quint32)), this, SLOT(updateFlowCtrl(quint32)) );

#ifdef linux
	//! ��ȡrtcʱ��;
	time_t now = time(NULL);

	//! ��ȡ���ݿ��־;
	long last_time = DataBase::getInstance()->queryData("last_time").toLong();

	qDebug() << "!!!!!!now = " << now;
	qDebug() << "!!!!!!last_time = " << last_time;

	if(last_time == 0)
	{
		//д��last_time;
		//DataBase::getInstance()->updateDate("last_time", QString::number(now));

		qDebug() << "last_time == 0";
	}
	else
	{
		//! �Ƚ�;
		if( now <= last_time )
		{
			//while(1)
			qDebug() << "--------now <= last_time";
			//exit;
			//while(1);
			//system(QString("exit").toLatin1().data());
			noRTCBattery = true;
		}
		else
		{
			//while(1)
			qDebug() << "=======now > last_time";
			//DataBase::getInstance()->updateDate("last_time", QString::number(now));

			noRTCBattery = false;
		}
	}

	DataBase::getInstance()->updateDate("last_time", QString::number(now));
	system(QString("hwclock -s").toLatin1().data());
#endif

}

void MachineStat::clearTime(MachineTime time)
{
	if(time == SYSTIME)
		clearUsedTime();
		//clearStartupTime();
	else if(time == PUMPTIME)
		clearBugleCnt();
	else if(time == STARTUPTIME)
		clearStartupUsedTime();
}


void MachineStat::storeOldStat(MachineStatment stat)
{
	m_machineStat.oldMachineStat = m_machineStat.machineStat;//��¼�ɵĻ���״̬;
	m_machineStat.machineStat = stat;
}


void MachineStat::stopMachine()
{
	disableUpdateFlow();
	
	m_pCommunicationCoupling->sendMcuCmd(1,MCU_WAVEADD_MOTOR,0);//�Žܻ���������@2016-06-26
	
	setStartupTime(0);
	BugleCompensation::getInstance()->stopOutput();

	m_nFlowCtrlWord = FLOW_INVALID;//��ֵһ���Ƿ������ٿ�����;

	//�ָ�PUMP��PURGE��������ʾ;
	emit(updateFlowDisplay(DataBase::getInstance()->queryData("pumpPurgeFlowVal")));

	m_machineStat.m_dCurrentFlow = 0;//�Žܻ����@2016-07-15
	m_dCurrentflowValInPc = 0; 
	m_nCurrentPercent = 100;
	emit(updatePercentDisplay("100")); 
}

void MachineStat::pauseMachine()
{
	//disableUpdateFlow();//�Žܻ�����@2016-06-25����ͣʱ�����Ժ㶨�ٶ�����
	BugleCompensation::getInstance()->pauseOutput();
}

void MachineStat::startMachine()
{
	enableUpdateFlow();
	int which = pDb->queryData("gradientTable").toInt();
	if(which != 12)
		pDb->readGradientTableIntoList(which);			//��ȡ�ݶȱ���ڴ�;
	clearStartupTime();

	if( m_machineStat.oldMachineStat == PAUSE )
		BugleCompensation::getInstance()->waitOutputStable();

	sysError(OVERPRESS_ERR, false);//ȡ������;
}

void MachineStat::pumpMachine()
{
	enableUpdateFlow();

	int which = pDb->queryData("gradientTable").toInt();
    double flow = getGradientTableFlow(which);

	//double flow = DataBase::getInstance()->queryData("pumpPurgeFlowVal").toDouble();
	updateFlow(flow, PUMP_PURGE_MODE);
	clearStartupTime();

	//�Žܻ�����@2016-06-28
	//if( m_machineStat.oldMachineStat == PAUSE )
	//	BugleCompensation::getInstance()->waitOutputStable();

	emit(updateFlowDisplay(QString::number(flow, 'f', 4)));
	
}

void MachineStat::clearStartupTime()
{
	if(m_machineStat.oldMachineStat != PAUSE)//���֮ǰ������ͣ״̬������;
		setStartupTime(0);
}

void MachineStat::purgeMachine()
{
	enableUpdateFlow();
	//DataBase::getInstance()->readGradientTableIntoList(11);
	double flow = DataBase::getInstance()->queryData("pumpPurgeFlowVal").toDouble();
	if(!m_machineStat.m_bUpdateFlowFromPc)//�Žܻ����@2016-06-25
	{
		updateFlow(flow, PUMP_PURGE_MODE);

		//�ָ�PUMP��PURGE��������ʾ;
		emit(updateFlowDisplay(DataBase::getInstance()->queryData("pumpPurgeFlowVal")));
	}
	clearStartupTime();

	//if( m_machineStat.oldMachineStat == PAUSE ) //�Žܻ�����@2016-06-25
	//	BugleCompensation::getInstance()->waitOutputStable();
}

//! modified by wjf@2016-06-22 �򻯽���PCģʽ�Ĺ���
void MachineStat::pcCtrlMachine()
{
#if 0
	//������λ��ʱ��ͬ��
	//�ж�����ͨ���ٿ��ƻ�����λ���ݶȿ���;

	m_nPcGradientCtrlFlag = 0;
	if(m_dCurrentflowValInPc == 0)
	{
		m_bPcGradientCtrl = true;//PC����;
	}
	else//��ͨ���ٿ���;
		syncFlowFromPc();

	syncFlowFromPc();

	pDb->updateDate("gradientTable", "10");
	clearStartupTime();
#endif

	m_nPcGradientCtrlFlag = 0;
	pDb->updateDate("gradientTable", "10");
	clearStartupTime();

	////�Žܻ����@2016-06-29
	//if(DataBase::getInstance()->queryData("pcProtocol").toInt() != 0)
	//	syncFlowFromPc();
}

#define CONTROL_WORD_FACTOR 6*(pow((double)2,32))/125000000/10


float MachineStat::GetWordFactor()
{
	int selectindex = DataBase::getInstance()->queryData("pumptype").toInt(); 
	//float pumpMaxFlowArray[]={6,1.5,0.245,0.11,0.05,0.025,0.008333};//�Žܻ��޸�@2016-06-14����50ml����ϵ��3.3�ĳ�1.5
	float pumpMaxFlowArray[]={6,1.5,0.245,
		0.11,
		0.11,
		0.05,
		0.05,
		0.025,
		0.025,
		0.008333,
		0.008333};//�Žܻ��޸�@2016-06-14����50ml����ϵ��3.3�ĳ�1.5
	if(selectindex>10)
	{
		selectindex=0;
	}
	return pumpMaxFlowArray[selectindex]*(pow((double)2,32))/125000000/10;

}

void MachineStat::updateFlow(double flow, MachineStat::FlowCtrlMode eFlowMod)
{
	//���ٷ�Χ��⣬�������ֵͣ������;
	switch(eFlowMod)
	{
		case GRADIENT_MODE:
			updateFlowInGradientMode(flow);
		break;
		case PUMP_PURGE_MODE:
			updateFlowInPumpPurgeMode(flow);
		break;
		case PC_MODE:
			updateFlowInPcMode(flow);
		break;
		case DEBUG_MODE:
			updateFlowInDebugMode(flow);
		break;
		default:
			return;
	}

	m_machineStat.m_dCurrentFlow = flow;
}


double MachineStat::compensationForFlowSecondary( double flow )
{
	quint32 elapse = 0;
	if(m_bBugleFlag)
	{
		elapse = bugleTime.elapsed();
		m_bBugleFlag = false;
	}

	double temp = (elapse/1000.0)*flow/1.666;
	temp = temp*2.0*M_PI;
	temp = sin(temp);
	temp = 0.1*temp + 1;
	temp *=flow;
	return temp;
}


quint32 MachineStat::compensationForFlow(double flow)
{ 
	QList<LineUint> &flowCompensationList = pDb->getFlowCompensationList();
	for (int i = 0; i<flowCompensationList.count(); i++)
	{
		const LineUint &line = flowCompensationList.at(i);
		if(line.inRange(flow))
		{
			double percent = line.getValueByXAndK(flow);//���ݹ�ʽ���ض�Ӧ������ֵ;
			percent = flow/percent;
			//percent = compensationForFlowSecondary(percent);
			return (percent/0.0001);
		}
	}
	return (flow/0.0001);
}


int MachineStat::checkProbation()
{
	quint32 probationDay = pDb->queryData("tryDay").toInt();			//��������;
	//qDebug()<<pDb->queryData("usedTime").toULong();
	//quint32 usedDay = pDb->queryData("usedTime").toULong()/60/60/24;	//�Ѿ�����ʱ��;
	quint32 usedDay = getTime(SYSTIME)/60/60/24;

	qint32 temp = probationDay - usedDay;
	
	//temp = 3;//������;

	if(temp <= 0)
	{
		m_machineStat.bTryDone = true;
		temp = 0;
	}
	else
		m_machineStat.bTryDone = false;

	return temp;
}

void MachineStat::saveUsedTime()
{
	//�����ۼ�ʹ��ʱ���¼;
	quint32 usedTime = m_machineStat.m_nSysUsedTime;
	QString temp = QString::number(usedTime);
	pDb->updateDate("usedTime", temp);

	usedTime = m_machineStat.pumpStartupTime;
	temp = QString::number(usedTime);
	pDb->updateDate("pumpUsedTime",temp);
}

//! modified by wjf@2016-06-22
void MachineStat::syncTime(quint32 time)
{
	//! �������߶�ʮһ���յ���λ��ͬ���ͽ���PCģʽ;
	if(m_machineStat.machineStat != PCCTRL && !bSyncOverPress)
		setMachineStat(PCCTRL);

	//qDebug() << "time = " << time;

	setStartupTime( time*SYNC_TIME_UNIT );

	////�ж��Ƿ���PC����ģʽ;
	//if( !m_bPcGradientCtrl )
	//{
	//	m_bPcGradientCtrl = true;
	//	setMachineStat(PCCTRL);
	//}

	m_nPcGradientCtrlFlag = 0;

	if(bOverPress)//�Žܻ����@2016-07-26
		return;

	m_machineStat.m_bUpdateFlowFromPc = false;//�Žܻ����@2016-06-25�����pc��ͬ������ʱ����ʾ����

	//�Žܻ����@2016-06-20�����ѹ����������ͬ������
	//if(!bSyncOverPress)
	//{
		//syncFlowFromPc();
	//}
}

quint32 MachineStat::updateFlowByGradientList(quint32 time)
{
	time = time % pDb->getGradientEndTime();				//ʱ��ת��;
	QList<LineUint> &gradientList = pDb->getGradientList();//�������;
	for (int i = 0; i<gradientList.count(); i++)
	{
		const LineUint &line = gradientList.at(i);
		if(line.inStep(time))											//�ж��Ƿ�ﵽ�ݶ�ת����ʱ��;
		{
			double flow = line.getStepValueByX(time);					//���ض�Ӧʱ��������ֵ;
			updateFlowPercent(line.getPercentVal(), GRADIENT_MODE);		//�������ٰٷֱ�;
			updateFlow(flow, GRADIENT_MODE);							//��������;
			return flow;
		}
	}
	return 0;
}


double MachineStat::compensationForPress(uint32 arg)
{
	double v0 = pDb->queryData("pressRawV0").toDouble();
	//double val = ((double)arg-v0)*0.01;//40MPa����
	double val = ((double)arg-v0)*0.0128; //51MPa����
	//double val = ((double)arg-v0)*0.015; //60MPa����
	QList<LineUint> &pressCompensationList = pDb->getPressCompensationList();
	for (int i = 0; i < pressCompensationList.count(); i++)
	{
		if( pressCompensationList.at(i).inRange(val) )
		{
			double fix = pressCompensationList.at(i).getValueByXAndK(val);
			val = val * fix;
			break;
		}
	}
	return val;
}


void MachineStat::checkPressOverloaded(double val)
{
	double maxVal = pDb->queryData("maxpress").toDouble();
	double minVal = pDb->queryData("minpress").toDouble();
	quint8 warning = 0;
	if(getTime(STARTUPTIME) > 60)
	{
		if(val > maxVal)
			warning = 2;
		else if(val < minVal)
			warning = 1;
	}
	else//С��һ���Ӳ�����Сѹ�����;
	{
		if(val > maxVal)
			warning = 2;
	}

	
	if(warning != 0)
	{
		if(isFixMode())
		{
			stopMachine();
			setFixMode(false);
		}
		else
		{
			setMachineStat(STOP, false);//ͣ��;
		}
		//setMachineStat(STOP, true);//ͣ��;
		overPress = val;
		bOverPress = true;//��λѹ��������־;
		sysError(OVERPRESS_ERR, true);//����;

		//�Žܻ����@2016-06-23
		if(bSyncFlag)
			bSyncOverPress = true;
	}
	else
	{
		//sysError(OVERPRESS_ERR, false);//ȡ������;
	}

	m_sCurrentPressVal = QString::number(val, 'f', 2);
	emit(updatePressDisplay(m_sCurrentPressVal, warning));
}

void MachineStat::updatePresVal(quint32 arg)
{
	BugleCompensation::getInstance()->adjustOutputByInput(arg);
	m_nCurrentPressRawCode = arg;
	double val = compensationForPress(arg);
	if(val < 0)
		val = 0;

	val = getAverageOfPress(val);//���ݳ��˲�;
	checkPressOverloaded(val);
}


void MachineStat::clearPress()
{
	pDb->updateDate("pressRawV0", QString::number(m_nCurrentPressRawCode));
}

/********************************20160615*********************************************/
void MachineStat::uploadPresVal()
{
	double val = m_sCurrentPressVal.toDouble();
	val = val * 100;
	//��Э��;
	if(DataBase::getInstance()->queryData("pcProtocol").toInt() == 0 )
	{
		int pumpType = DataBase::getInstance()->queryData("pumptype").toInt();
		if(pumpType > 1 )
			val = val / 10;
		m_pCommunicationCoupling->sendCmd(1, PFC_READ_PRESS, val);
	}
	else//clarityЭ��;
		m_pCommunicationCoupling->sendCmdClarity(0, PFCC_SEND_PRESS, DectoBCD(val, 4));
}

void MachineStat::setUploadPressFreq_Clarity( int freq )
{
	if(freq == 0)
		m_pUploadTimer->stop();
	else
	{
		if(m_pUploadTimer->isActive())
			m_pUploadTimer->stop();
		m_pUploadTimer->start(freq*100);//50*freq(ms)
	}
}

/**************************************************************************************/

void MachineStat::initMACAddr()
{
	QString sMAC = DataBase::getInstance()->queryData("serial");
	if(sMAC.count() < 10)
		sMAC = QString("1234567890");

	sMAC = "ifconfig eth0 hw ether " + QString("AA") + sMAC;//��һ���ֽڵĵ�һλ������1��1��ʾ�㲥
	//qDebug() << "sMAC = " << sMAC;

#ifdef linux
	system(QString("ifconfig eth0 down").toLatin1().data());
	system(sMAC.toLatin1().data());   //�Žܻ�ɾ��@2016-06-15�������ã���ʱҪ��ӻ���
	system(QString("ifconfig eth0 up").toLatin1().data());
#endif
}

void MachineStat::initIPAddr()
{
	QString strCig = QString("ifconfig eth0 %1.%2.%3.%4").arg(DataBase::getInstance()->queryData("ip1")).arg(DataBase::getInstance()->queryData("ip2")).arg(DataBase::getInstance()->queryData("ip3")).arg(DataBase::getInstance()->queryData("ip4"));
#ifdef linux
	system(strCig.toLatin1().data());  //�Žܻ�ɾ��@2016-06-15�������ã���ʱҪ��ӻ���
#endif
}

void MachineStat::initDb()
{
	// = DataBase::getInstance();

}

quint32 MachineStat::getTime(MachineTime time)
{
	if(time == SYSTIME)
		//return getUsedTime();
		return m_machineStat.m_nSysUsedTime;
	else if(time == STARTUPTIME)
		return m_machineStat.startupTime;
	else if(time == PUMPTIME)
		return m_machineStat.pumpStartupTime;
	else
		return 0;
}

void MachineStat::initTimeoutList()
{
	m_timeoutList.clear();
	registerTimeoutFunc(0, &MachineStat::syncFlowToMachine);
	registerTimeoutFunc(0, &MachineStat::checkFlowCtrlByPc);
	registerTimeoutFunc(2, &MachineStat::saveUsedTime);
	registerTimeoutFunc(2, &MachineStat::checkTryOut);
	registerTimeoutFunc(60, &MachineStat::saveDataBase);
	registerTimeoutFunc(0, &MachineStat::updateStartupTime);
}

void MachineStat::releaseTimeoutList()
{
	qDeleteAll(m_timeoutList.begin(), m_timeoutList.end() );
	m_timeoutList.clear();
}

void MachineStat::registerTimeoutFunc(const quint32 time, pTimeoutFunc pFunc)
{
	LPTimeOutStruct pTimeout= new TimeOutStruct;
	pTimeout->timeout = time;
	pTimeout->timeoutFunc = pFunc;
	pTimeout->cnt = 0;
	m_timeoutList.append(pTimeout);
}

void MachineStat::syncFlowToMachine()
{
	if(m_machineStat.machineStat == RUNNING)
	{
		//if(m_machineStat.machineStat!=PCCTRL)
		//{
			updateFlowByGradientList(m_machineStat.startupTime);		//���ݶȸ�������;
		//}
	}
}

void MachineStat::checkTryOut()
{
	int active = pDb->queryData("bActive").toInt();
	if(active == 0)//���δ�����Ҫ��ʱ�������ʱ��;
	{
		if( checkProbation() == 0)
		{
			//g_pMainWindow->changePage(PWDPAGE_INDEX);
			m_machineStat.bTryDone = true;
			g_pMainWindow->changePage(PERMITPAGE_INDEX);
		}
		else//������δ��;
		{
			m_machineStat.bTryDone = false;
		}
	}
	else
		m_machineStat.bTryDone = false;
}

//! modified by wjf@2016-06-22
void MachineStat::checkFlowCtrlByPc()
{
	//������λ��ʱ��ͬ��;
	//return;

	//if(m_machineStat.machineStat!= PCCTRL)//�Žܻ�����@2016-06-25
	//	return;

	if(!bSyncFlag)//�Žܻ����@2016-06-25
		return;

	//if(!m_bPcGradientCtrl)
	//	return;

	//Clarity Э�鲻��Ҫ���²���;
	if(DataBase::getInstance()->queryData("pcProtocol").toInt() == 1)
		return;

	m_nPcGradientCtrlFlag++;
	if(m_nPcGradientCtrlFlag >= GRADIENTPCTIMEOUT)
	{
		m_bPcGradientCtrl = false;
		m_nPcGradientCtrlFlag = 0;

		bSyncFlag = 0;
		bSyncOverPress = 0;

		setMachineStat(PAUSE); //ֹͣ�ɼ���Ҫά�ֺ㶨����
		//setMachineStat(STOP); //�Žܻ��޸�@2016-06-21
	}
}

void MachineStat::updateStartupTime()
{
	if(m_machineStat.machineStat >= RUNNING)
	{
		//������λ��ʱ��ͬ��;
		/**/if(!m_bPcGradientCtrl && m_machineStat.machineStat != PCCTRL)
			setStartupTime(++m_machineStat.startupTime);
		//setStartupTime(++m_machineStat.startupTime);
		m_machineStat.pumpStartupTime++;

	}

	m_machineStat.m_nSysUsedTime++;//ϵͳʹ��ʱ��;

	//static int cnt = 0;
	//emit(updateBugleCnt(cnt++));
}

quint64 MachineStat::generateActiveCode( quint64 sertialNum, quint8 which )
{
	quint64 ret=0;
	quint64 date=m_machineStat.m_nSerialId;
	/*QDate today = QDate::currentDate();
	date=today.year()*10000;
	date+=today.month()*100;
	date+=today.day();*/
	if(which == 0)//���ü���;
	{
		ret = (sertialNum+date)%ACTIVE_CODE_MAX;
		ret ^=ACTIVE_MASK;
		ret %=ACTIVE_CODE_MAX;
	}
	else
	{
		ret = (sertialNum+date)%ACTIVE_CODE_MAX;
		ret ^=ACTIVE_MASK;
		ret %=ACTIVE_CODE_MAX;
		ret /=1000;
		ret *=1000;
		ret +=ACTIVE_CODE_TRY;
	}
	return ret;
}

void MachineStat::setStartupTime(quint32 time)
{
	static quint32 ttime = 0;

	if(ttime == time)
		return;
	else
		ttime = time;

	m_machineStat.startupTime = time;
	emit( updateStartupTimeDisplay( time) );
}

void MachineStat::initCommunication()
{
	m_pCommunicationCoupling = new CommunicationCoupling(this);
}

void MachineStat::enableUpdateFlow()
{
	IoModule::getInstance()->logic_set_io(IO_MOTORSTOP_MASK, false);
}

void MachineStat::disableUpdateFlow()
{
	IoModule::getInstance()->logic_set_io(IO_MOTORSTOP_MASK, true);
}

void MachineStat::dealBulge()
{
	/*m_bBugleFlag = true;
	bugleTime = QTime::currentTime();
	bugleTime.restart();*/
	//��֪͹�ֲ���;
	BugleCompensation::getInstance()->bugleSignal();

	m_machineStat.bugleCnt++;
	DataBase::getInstance()->updateDate("bugleCnt", QString::number(m_machineStat.bugleCnt) );

	emit(updateBugleCnt(m_machineStat.bugleCnt));
}

void MachineStat::weepingWarn(bool flag)
{
	sysError(WEEPING, flag);//����
}

void MachineStat::serialNumberGenerate()
{
	if(DataBase::getInstance()->queryData("serial").toULongLong() == 0 )
	{
		QDateTime currentTime = QDateTime::currentDateTime();
		qsrand(currentTime.time().msec());
		int randNum = qrand()%999999;

		quint64 serialNum = currentTime.date().year()*pow(10.0, 13) + currentTime.date().month()*pow(10.0, 11) + currentTime.date().day()*pow(10.0, 9);
		serialNum += currentTime.time().hour()*pow(10.0, 7) + currentTime.time().minute()*pow(10.0, 5) + currentTime.time().second()*pow(10.0, 3);
		serialNum += randNum;

		serialNum %=SERIAL_MAX;
		serialNum ^= SERIAL_NUM_KEY;

		DataBase::getInstance()->updateDate("serial", QString::number(serialNum));
		qDebug()<<"............"<<serialNum;
	}
}

quint32 MachineStat::getTryDayFromActiveCode( quint64 activeNum, quint32 serialId )
{
	quint32 tryday = activeNum%1000;
	serialId %=1000;
	tryday ^=serialId;
	tryday = 999-tryday;
	return tryday;
}


quint64 getActiveNum(quint64 sertialNum)
{
	quint64 serialNum = DataBase::getInstance()->queryData("serial").toULongLong();
	serialNum ^= SERIAL_NUM_KEY;
	return serialNum - SERIAL_NUM_VAL;
}


bool MachineStat::activeMachine(quint64 activeNum, bool bActive/* = true*/)
{
	bool ret = true;
	if( bActive )
	{
		quint64 serialNum = DataBase::getInstance()->queryData("serial").toULongLong();
		if( activeNum >= ACTIVE_CODE_TRY )//���ü���;
		{
			//�ж��Ƿ����;
			quint64 tActiveNum = generateActiveCode(serialNum, 1);
			quint64 temp = activeNum/1000*1000;
			if(tActiveNum == temp)
			{
				quint64 tryday = getTryDayFromActiveCode(activeNum, m_machineStat.m_nSerialId);
				//������к������;
				m_machineStat.m_nSerialId = 0;
				
				if(tryday == 0)//ȡ������;
				{
					//�����������;
					pDb->updateDate("tryDay", QString::number(0));
					clearUsedTime();
				}
				else
				{
					//�ۼ���������;
					tryday+=pDb->queryData("tryDay").toUInt();
					pDb->updateDate("tryDay", QString::number(tryday));
					/*qDebug()<<pDb->queryData("tryDay");*/

				}
				DataBase::getInstance()->updateDate("bActive", "0");

			}
			else
				ret = false;
		}
		else//���ü���;
		{
			//�ж��Ƿ����;
			quint64 tActiveNum = generateActiveCode(serialNum, 0);
			if(tActiveNum == activeNum)
			{
				//��¼�����־;
				DataBase::getInstance()->updateDate("bActive", "1");
				//������к������;
				m_machineStat.m_nSerialId = 0;

				//�����������;
				pDb->updateDate("tryDay", QString::number(0));
				clearUsedTime();
			}
			else
				ret = false;
		}

	}
	else
	{
		//ȡ�������־;
		DataBase::getInstance()->updateDate("bActive", "0");
	}
	return ret;
}

void MachineStat::clearBugleCnt()
{
	m_machineStat.bugleCnt = 0;
	DataBase::getInstance()->updateDate("bugleCnt", QString::number(m_machineStat.bugleCnt));
}

void MachineStat::clearUsedTime()
{
	//m_machineStat.m_firstTryDateTime = QDateTime::currentDateTime().toTime_t();//��¼��ǰ����ʱ��;
	//pDb->updateDate("firstTryDateTime", QString::number(m_machineStat.m_firstTryDateTime));
	m_machineStat.m_nSysUsedTime = 0;
	QString temp = QString::number(m_machineStat.m_nSysUsedTime);
	pDb->updateDate("usedTime", temp);
}

void MachineStat::setPcProtocol(int idx)
{
	m_pCommunicationCoupling->setPcProtocol(idx);
}

void MachineStat::setConnectPort(int idx)
{
	m_pCommunicationCoupling->setConnectPort(idx);
}

void MachineStat::updateFlowCtrl(quint32 data)
{
	m_pCommunicationCoupling->sendMcuCmd(1, MCU_WAVEADD_MOTOR, data);
}

void MachineStat::shutDownWarn()
{
	if(bWarn)
		bWarn = false;

	IoModule::getInstance()->doWarn(false);
}

/********************************************20160615*************************************/
void MachineStat::uploadPressTimeout_Clarity()
{
	//�����ǰ�Ǿ�Э�飬��ֹͣ��ʱ��������;
	if( DataBase::getInstance()->queryData("pcProtocol").toInt() == 0 )
	{
		setUploadPressFreq_Clarity(0);
		return;
	}

	uploadPresVal();
}
/**********************************************************************************************/

void MachineStat::initPressPoll()
{
	//��ʼ��au���У�����ƽ��auֵ��ƽ��;
	pressList.clear();
	for (int i = 0; i < PRESS_POLL_SIZE; i++)
	{
		pressList.append(0.0);
	}
}

double MachineStat::getAverageOfPress( double au )
{
	//auֵƽ������;
	double auAverage = 0;
	int maxLen = PRESS_POLL_SIZE;

	if( pressList.count() > maxLen && maxLen != 0 )
	{
		pressList.removeFirst();
		pressList<<au;
		for (int i = 0; i < maxLen; i++)
		{
			auAverage += pressList.at(i);
		}
		auAverage /= (double)maxLen;
	}
	else
	{
		pressList<<au;
		auAverage = au;
	}
	return auAverage;
}

bool MachineStat::checkFlowOverLoaded(double flow)
{
	double maxFlow = getMaxFlow();
	if(flow > maxFlow)
	{
		setMachineStat(STOP, true);
		return true;
	}
	else
		return false;
}

quint32 MachineStat::getMaxFlow()
{
	//quint32 pumpMaxFlowArray[]={10,50,100,250,500,1000,3000};
	quint32 pumpMaxFlowArray[]={10,50,100,150,250,300,500,800,1000,2000,3000};
	quint32 index = DataBase::getInstance()->queryData("pumptype").toInt();
	if(index > 10)
		return 0;
	return pumpMaxFlowArray[index];
	
}

quint32 MachineStat::getMaxPress()
{
	quint32 pumpMaxPressArray[]={42, 25 , 20, 20, 20, 15, 15, 10, 10, 10, 10};
	quint32 index = DataBase::getInstance()->queryData("pumptype").toInt();
	if(index > 10)
		return 0;
	return pumpMaxPressArray[index];

}


void MachineStat::sysError( int number, bool insert )
{
	if(warningList.indexOf(number) != -1 )
	{
		if(!insert)
			warningList.removeOne(number);
	}
	else
	{
		if(insert)
			warningList.append(number);
	}
	updateWarning();
}


void MachineStat::updateWarning()
{
	if(warningList.count() == 0)
		emit(systemError(0, tr("No Warn")));
	else
	{
		int warningId = warningList.at(0);
		switch(warningId)
		{
		case COMUNICATION_ERR:
			if(!bOverPress)
				emit(systemError(warningId, tr("Com error!")));
			break;
		//case OVERFLOW_ERR:
		//	emit(systemError(warningId, tr("Flow over:")+QString::number(overFlow)));
		//	//��������������;
		//	if(!bWarn)
		//	{
		//		bWarn = true;
		//		IoModule::getInstance()->doWarn(true);
		//		QTimer::singleShot(3000, this, SLOT(shutDownWarn()));
		//	}
		//	break;
		case OVERPRESS_ERR:
			{
				emit(systemError(warningId, tr("Press over:")+QString::number(overPress)));
				//��������������;
				if(!bWarn)
				{
					bWarn = true;
					IoModule::getInstance()->doWarn(true);
					QTimer::singleShot(3000, this, SLOT(shutDownWarn()));
				}
			}
			break;
		case WEEPING:
			{
				emit(systemError(warningId, tr("Weeping:")));
				//��������������;
				if(!bWarn)
				{
					bWarn = true;
					IoModule::getInstance()->doWarn(true);
					QTimer::singleShot(3000, this, SLOT(shutDownWarn()));
				}
			}
			break;
		default:
			break;
		}
	}
}

MachineStat::MachineStatment MachineStat::getMachineStat()
{
	return m_machineStat.machineStat;
}

void MachineStat::initFlowCtrl()
{
	FlowCtrl::getInstance();
}

FlowCtrl * MachineStat::getFlowCtrl()
{
	return m_pFlowCtrl;
}

void MachineStat::pumpTypeChanged(bool flag)
{
	//��ȡ�õ����͸ı���λ����Э�����ٿ��������;
	int index = DataBase::getInstance()->queryData("pumptype").toInt();
	m_pCommunicationCoupling->setPumpTypeForPcFlowCtrl(index);
	m_machineStat.m_nPumpType = index;

	//����Э��������
	m_pCommunicationCoupling->sendCmd(PROTOCL_LOCAL_USE_CHANGE_PUMPTYPE, index, 0);

	//���µ�ǰ���豸��;
	updateMachineCode();

	if(flag == true)
	{
		//! �ָ���������;
		DataBase::getInstance()->restoreCompensationTable();//! added by wjf@2016-06-22
	}

}

int MachineStat::getPumpType()
{
	return m_machineStat.m_nPumpType;
}

void MachineStat::updateSerialId( quint32 id )
{
	m_machineStat.m_nSerialId = id;
}

void MachineStat::isUpdateFlowFromPC()
{
	if(!m_machineStat.m_bUpdateFlowFromPc)
		return;

	m_machineStat.m_bUpdateFlowFromPc = false;

	updateFlowPercentInPcMode(m_nCurrentPercent, true);
	emit(updateFlowDisplay(QString::number(m_dCurrentflowValInPc, 'f', 4)));			//����������ʾ;
}

quint32 MachineStat::pcGetMachineStat()
{
	quint32 ret = 0;

	//��ȡ����״̬;
	MachineStatment st = getMachineStat();
	if( st != STOP )
		ret |= 1<<20;

	//��ȡ��ǰ����;
	//m_machineStat.m_dCurrentFlow = 5.000;//�����ã�ģ�⵱ǰ����5ml������clarityЭ����;
	quint32 flow = m_machineStat.m_dCurrentFlow*1000;
	quint32 hFlow = DectoBCD(flow, 5);
	//qDebug() << "flow = " << flow;
	//qDebug() << "hFlow = " << hFlow;
	ret |= hFlow;

	return ret;
}

quint32 MachineStat::getMachineCode()
{
	return g_MachineCode;
}

void MachineStat::updateMachineCode()
{
	quint32 nPumpType = DataBase::getInstance()->queryData("pumptype").toInt();
	//quint32 nCode[7] = {0x12, 0x13, 0x20, 0x21, 0x22, 0x23, 0x24};
	quint32 nCode[11] = {0x12, 0x13, 0x20, 0x25, 0x21, 0x26, 0x22, 0x27, 0x23, 0x28, 0x24};
	nPumpType %= 11;
	g_MachineCode = nCode[nPumpType];
}

void MachineStat::updateFlowPercent( quint32 percent, FlowCtrlMode eFlowMod )
{
	switch (eFlowMod)
	{
		case PC_MODE:
			updateFlowPercentInPcMode(percent);
			break;
		case GRADIENT_MODE:
			updateFlowPercentLocal(percent);
			break;
		default:
			break;
	}
}

void MachineStat::updateFlowPercent( double percent )
{
	emit(updatePercentDisplay(QString::number(percent)));
}

void MachineStat::updateFlowPercentLocal( quint32 percent )
{
	static quint32 tpercent = 100000;

	if(tpercent == percent)
		return;
	else
		tpercent = percent;

	QString temp = QString("%1").arg(percent);
	m_dFlowPercent = (double)(percent)/100.0;

	emit(updatePercentDisplay(temp));
}

void MachineStat::updateFlowPercentInPcMode( quint32 percent , bool manual)
{
	

	if(m_nCurrentPercent == percent && manual == false)
		return;
	else
		m_nCurrentPercent = percent;

	QString temp;
	if(percent == 0)
	{
		temp = "100";
	}
	else
	{
		temp = QString("%1").arg(percent -1);
	}
	emit(updatePercentDisplay(temp));
}

void MachineStat::updateFlowInPcMode( double flow )
{
	if( m_dCurrentflowValInPc == flow)
	{
		if( m_machineStat.machineStat == PCCTRL )
			return;
	}

	m_dCurrentflowValInPc = flow;
	m_machineStat.m_bUpdateFlowFromPc = true;

	//qDebug() << "updateFlowInPcMode() flow = " << flow; //�Žܻ��������@2016-06-25

	emit(updateFlowDisplay(QString::number(flow, 'f', 4)));			//����������ʾ;
	//syncFlowFromPc();                     //�Žܻ�ɾ��@2016-06-15��ȥ��PC�˸������ٺ��豸���������Ĺ���
}

void MachineStat::updateFlowInGradientMode( double flow )
{
	//����x�ٷֱ�;
	flow = flow * m_dFlowPercent;

	//������ʾ;
	emit(updateFlowDisplay(QString::number(flow, 'f', 4)));			//����������ʾ;

	syncFlowToMcu(flow);

}

void MachineStat::updateFlowInPumpPurgeMode( double flow )
{
	syncFlowToMcu(flow);
}

void MachineStat::updateFlowInDebugMode( double flow )
{
	syncFlowToMcu(flow);
}

void MachineStat::syncFlowFromPc()
{
	static double last_flow = 0;//�Žܻ��������@2016-06-25
	if(last_flow != m_dCurrentflowValInPc)
	{
		last_flow = m_dCurrentflowValInPc;

		//emit(updateFlowDisplay(QString::number(m_dCurrentflowValInPc, 'f', 4)));			//����������ʾ;

		//qDebug() << "syncFlowFromPc() flow = " << m_dCurrentflowValInPc;
	}

	if(bOverPress)//�Žܻ����@2016-07-26
		return;

	m_machineStat.m_bUpdateFlowFromPc = false;//�Žܻ����@2016-06-25�����pc��ͬ������ʱ����ʾ����
	syncFlowToMcu(m_dCurrentflowValInPc);
}

void MachineStat::syncFlowToMcu( double flow )
{
	static double last_flow = 0;//�Žܻ��������@2016-06-25
	if(last_flow != flow)
	{
		last_flow = flow;
		//qDebug() << "syncFlowToMcu() flow = " << flow;
	}

	//�������;
	if( checkFlowOverLoaded(flow) )
	{
		overFlow = flow;
		sysError(OVERFLOW_ERR, 1);
		return;
	}
	else
		sysError(OVERFLOW_ERR, 0);

	/***************************���ٲ���*********************************/
	flow = compensationForFlow(flow);


	/********************ת����mcu������,д��MCU*************************/
	//quint32 ret = flow*CONTROL_WORD_FACTOR;
	quint32 ret = flow*GetWordFactor();
	
	static qint8 reSendMcuCmdFlag = 0;

	if(m_nFlowCtrlWord != ret )
	{
		m_nFlowCtrlWord = ret;

		reSendMcuCmdFlag = 0;

		//���¿����ֵ�MCU;
		//m_pCommunicationCoupling->sendMcuCmd(1, MCU_WAVEADD_MOTOR, ret);

		//͹�ֲ���ͬ��;
		//BugleCompensation::getInstance()->updateOutput(ret, flow);
		
		//qDebug() << "sendMcuCmd" << ret;//�Žܻ��������@2016-06-25
	}

	if(reSendMcuCmdFlag <= 3)
	{
		reSendMcuCmdFlag++;

		//���¿����ֵ�MCU;
		//if(reSendMcuCmdFlag%2 == 1)
			m_pCommunicationCoupling->sendMcuCmd(1, MCU_WAVEADD_MOTOR, ret);
	}
	
	//ʹ�����ٸ���;
	MachineStat::getInstance()->enableUpdateFlow();
}

uint MachineStat::getUsedTime()
{
	QDateTime startTime = QDateTime::fromTime_t(m_machineStat.m_firstTryDateTime);
	QDateTime currentTime = QDateTime::currentDateTime();

	return (currentTime.toTime_t() - startTime.toTime_t() );
}

void MachineStat::setFixMode(bool mode)
{
	m_machineStat.m_bFixMode = mode;
}

bool MachineStat::isFixMode()
{
	return m_machineStat.m_bFixMode;
}

void MachineStat::clearPressWarn()
{
	if(bOverPress)
	{
		bOverPress = false;
		sysError(OVERPRESS_ERR, false);//ȡ������;
	}
}

double MachineStat::getGradientTableFlow(quint32 nWhich, quint32 nRow /*= 0*/)
{   
    if(nWhich != 12)
        pDb->readGradientTableIntoList(nWhich);			        //��ȡ�ݶȱ���ڴ�;
    QList<LineUint> &gradientList = pDb->getGradientList();     //�������;

    if(gradientList.count() <= 0 )
        return 0;

    return gradientList.at(0).getFlowStr().toDouble();
}

void MachineStat::setNetWorkConfig(MachineStat::E_IPConfig eConfig)
{
	switch(eConfig)
	{
	case E_LOCALIP:
		{
			initIPAddr();
		}
		break;
	case E_LOCALPORT:
		{
			qDebug()<<"change port:"<<DataBase::getInstance()->queryData("port");

			quint32 connectPort = DataBase::getInstance()->queryData("connect_port").toUInt();
			if(connectPort == 1)//PC_COMMUNICATION_PORT_NET
			{
				m_pCommunicationCoupling->localPortChange();
			}
		}
		break;
	case E_REMOTEIP:
		{
			QString strCig = QString("%1.%2.%3.%4").arg(DataBase::getInstance()->queryData("remote_ip1")).arg(DataBase::getInstance()->queryData("remote_ip2")).arg(DataBase::getInstance()->queryData("remote_ip3")).arg(DataBase::getInstance()->queryData("remote_ip4"));
			qDebug()<<"!!!!!!!!!!!!!!!!remote ip change"<<"  "<<strCig;

			quint32 connectPort = DataBase::getInstance()->queryData("connect_port").toUInt();
			if(connectPort == 1)//PC_COMMUNICATION_PORT_NET
			{
				m_pCommunicationCoupling->remoteIPChange();
			}
		}
		break;
	case E_REMOTEPORT:
		{
			qDebug()<<"change remoteport:"<<DataBase::getInstance()->queryData("remote_port");

			quint32 connectPort = DataBase::getInstance()->queryData("connect_port").toUInt();
			if(connectPort == 1)//PC_COMMUNICATION_PORT_NET
			{
				m_pCommunicationCoupling->remotePortChange();
			}
		}
		break;
	}
}

