#ifndef MACHINESTAT_H
#define MACHINESTAT_H

#include <QObject>
#include "database.h"
#include <QList>
#include <QTime>
#include "lineuint.h"
#include "communicationcoupling.h"
#include "flowctrl.h"
/*********************************20160615*************************/
#include <QTimer>
/*****************************************************************/
int DectoBCD(int Dec,int length);

#define NOWARN			 0//�ޱ���
#define COMUNICATION_ERR 1//ͨѶ����;
#define OVERFLOW_ERR	 2//���ٳ���;
#define OVERPRESS_ERR	 3//ѹ������;
#define WEEPING          4//©Һ

class MachineStat;
typedef void (MachineStat::*pTimeoutFunc)();


class MachineStat : public QObject
{
	Q_OBJECT

public:

	enum USRTYPE
	{
		ADMIN,
		USER,
	};

	enum MachineTime
	{
		SYSTIME,		//ϵͳ�ۼ�ʱ��,����������;
		PUMPTIME,		//���ۼ�����ʱ��,����������;
		STARTUPTIME,	//����ʱ��,��������;
	};

	enum MachineStatment
	{
		INISTAT = -1,	//��ʼ״̬,����ʱ���ڸ�״̬;
		STOP = 0,		//ֹͣ״̬;
		PAUSE,			//��ͣ״̬;
		RUNNING,		//����״̬;
		PUMP,			//��Һ״̬;
		PURGE,			//��ϴ״̬;
		PCCTRL,			//��λ������״̬;
		FIX,			//У��ģʽ;
	};

	//���ٿ���ģʽ;
	enum FlowCtrlMode
	{
		GRADIENT_MODE,	//�ݶ�ģʽ;
		PUMP_PURGE_MODE,//�û���ϴģʽ;
		PC_MODE,		//��λ��ģʽ;
		DEBUG_MODE,		//����ģʽ;
	};

	enum E_IPConfig
	{
		E_LOCALIP,
		E_LOCALPORT,
		E_REMOTEIP,
		E_REMOTEPORT,
	};

	struct MachineRunningStat{
		MachineStatment machineStat;
		MachineStatment oldMachineStat;
		quint32 startupTime;
		uint m_firstTryDateTime;					//��¼���õ��յ�����ʱ�䣬������;
		quint32 m_nSysUsedTime;						//! ϵͳʹ��ʱ��
		quint32 pumpStartupTime;
		quint32 bugleCnt;
		bool bTryDone;								//������ϱ�־;

		quint32 m_nSerialId;
		int m_nPumpType;							//������;

		bool m_bUpdateFlowFromPc;					//��־����λ�����������ٺͰٷֱ�;

		double m_dCurrentFlow;						//��ǰ����;

		bool m_bFixMode;
		
	};

	//�Žܻ����@2016-06-23
	bool m_bCanClearOverPress;

	bool m_bPumpTypeChange;

	~MachineStat();
	static MachineStat *getInstance();
	void setFixMode(bool mode);										//����У׼ģʽ;
	bool isFixMode();
	void clearPressWarn();									//���ѹ������;
	void sysError(int reSend, bool insert);					//���뱨��;
	void setPcProtocol(int idx);
	void setConnectPort(int idx);

	int checkProbation();									//���ʹ��ʣ��ʱ��;
	quint32 getTime(MachineTime time);						//��ȡʱ��;
	void clearTime(MachineTime time);						//���ʱ��;
	bool activeMachine(quint64 activeNum, bool bActive = true);						//ͨ�������뼤�����״̬;	
	bool isTryDone(){return m_machineStat.bTryDone;}									//����Ƿ����������;

	void enableUpdateFlow();																//�������ٵ��ʹ��;
	void disableUpdateFlow();																//�������ٵ������;
	void updateFlow(double flow, MachineStat::FlowCtrlMode eFlowMod);						//��������(���٣����ٿ���ģʽ);
	void updateFlowPercent(quint32 percent, MachineStat::FlowCtrlMode eFlowMod);			//���ٸ���;
	void updateFlowPercent(double percent);
	float GetWordFactor();									//��ȡ��������;
	void clearPress();										//ѹ������;
	void syncTime(quint32 time);							//��λ��ʱ��ͬ��;
	
	void uploadPresVal();									//�ϴ�ѹ������λ��;
	/*********************************20160615*************************/
	void setUploadPressFreq_Clarity(int freq);				//�����ϴ�ѹ��Ƶ��;
	/*****************************************************************/
	//void updatePresVal(uint32 disp);						//����ѹ��ֵ;
	
	MachineStatment getCurrentStat(){
		return m_machineStat.machineStat;
	}														//��ȡ��ǰ����״̬;
	void restoreOldStat(){
		m_machineStat.machineStat = m_machineStat.oldMachineStat;//�Žܻ����@2016-06-25
		setMachineStat(m_machineStat.oldMachineStat);
	};	//�ָ���һ״̬;

	void initMACAddr();
	void initIPAddr();//��ʼ��ip��ַ;
	void setNetWorkConfig(MachineStat::E_IPConfig eConfig);//���ñ��غ�Զ��IP/PORT
	quint32 getMaxFlow();//��ȡ�������ֵ;
	quint32 getMaxPress();//��ȡ�������ֵ;

	FlowCtrl *getFlowCtrl();

	void pumpTypeChanged(bool flag = false);//���ͷ����ı�;
	int getPumpType();	//��ȡ��ǰ������;

	void updateSerialId(quint32 id);						//�������кŵ����ID;

	void isUpdateFlowFromPC();								//�ж��Ƿ�մ�PC����������,���������ʾ;

	/*0XXXXX ��ʾ�ô���ֹͣ״̬����XXXXXΪ�����е�ǰ�趨���١�
	1XXXXX ��ʾ�ô�������״̬����XXXXXΪ�����е�ǰ�趨���١�
	�������ݸ�ʽΪ�̶���λС������λΪml/min��*/
	quint32 pcGetMachineStat();								//��ȡ״̬1; 
	
	//�����豸�룬��ͬ���Ͷ�Ӧ��ͬ�豸��;
	quint32 getMachineCode();
	//�����豸��;
	void updateMachineCode();

	void updateWarning();			//���±���;//��������@2016-06-18�������˽�з�����Ϊ����

	void syncFlowFromPc();			//��PC���õ�����ͬ����MCU;//wjf���@2016-06-22,��˽�иĹ���;

	//��ȡ��ǰ����������״̬;
	MachineStatment getMachineStat();//�Žܻ����@2016-07-15,��˽�иĹ���;

	bool pwdOK;
	bool pwdNeed;
	USRTYPE usrType;
	int whichPage;  //���������ȷ��������ҳ��

	bool noRTCBattery;
private:
	typedef struct tagTimeOutStruct
	{
		quint32 timeout;
		quint32 cnt;
		pTimeoutFunc timeoutFunc;
	}TimeOutStruct,*LPTimeOutStruct;


	MachineStat(QObject *parent = 0);
	/*********************************20160615*************************/
	QTimer *m_pUploadTimer;
	/*********************************************************************/
	struct MachineRunningStat m_machineStat;				//����״̬;
	DataBase *pDb;
	CommunicationCoupling *m_pCommunicationCoupling;		//ͨѶ���ϲ�;
	FlowCtrl *m_pFlowCtrl;									//���ٿ���;
	quint32 m_nCurrentPressRawCode;							//��¼���´ӷֿ�MCU���յ���ѹ����;
	QString m_sCurrentPressVal;								//��¼���µ�ѹ��ֵ�������������ִ���;
	double m_dCurrentflowValInPc;							//��¼PC���õ���������ֵ;
	quint32 m_nCurrentPercent;								//��¼PC���õ����°ٷֱ�;
	double m_dFlowPercent;									//���ٰٷֱȣ�������;
	//double m_dPumpOrPurgeFlowVal;							//��¼����PUMP��PURGE״̬������ֵ;
	QList<LPTimeOutStruct> m_timeoutList;					//��ʱ�����б�;
	QList<int>warningList;									//��������;
//�������;
	//�ݶȱ��У���������;
	bool m_bPcGradientCtrl;									//��λ���ݶȿ��Ʊ�־λ;
	quint8 m_nPcGradientCtrlFlag;							//��λ���ݶȿ��Ƽ�ʱ�������ʱû���յ���λ����ͬ��ʱ��������ֹͣ����;
	quint32 m_nFlowCtrlWord;								//���ٿ�����;
	
	//͹�ֲ���;
	QTime bugleTime;
	bool m_bBugleFlag;

	//ѹ�����;
	QList<double>pressList;									//ѹ�����ݳ�;

//methods:
//��ʼ��;
	void initDb();	
	void initIO();
	void initTimer();
	void initMachineStat();
	void initFlowCtrl();
	void initLogicThread();									//��ʼ����̨ͨѶ�����߳�;
	void initTimeoutList();									//��ʼ����ʱ���б�;
	void initCommunication();								//ͨѶ��ʼ��;
//�ͷ�;
	void releaseTimeoutList();								//�ͷŶ�ʱ���б�;
//����״̬����;
	void storeOldStat(MachineStatment stat);
	void stopMachine();			//ֹͣ����;
	void pauseMachine();		//��ͣ����;
	void startMachine();		//��ʼ����;
	void pumpMachine();			//��Һ;
	void purgeMachine();		//��ϴ;
	void pcCtrlMachine();		//��λ������;
//ʱ�����;
	void setStartupTime(quint32 time);
	inline void clearStartupTime();	//״̬�л�ʱ���������ʱ��;
	void clearUsedTime();
	void clearBugleCnt();//��˨��ʹ�ô���
	void clearStartupUsedTime(){m_machineStat.startupTime = 0;}
	uint getUsedTime();											//��ȡϵͳʹ��ʱ��;
	void saveUsedTime();			//ʹ��ʱ���¼����;
	void registerTimeoutFunc(const quint32 time, pTimeoutFunc pFunc);
	void saveDataBase(){pDb->saveDb();}						//�������ݿ�;
	void checkTryOut();										//����������Ƿ�ʱ;
	void checkFlowCtrlByPc();								//�����λ���ݶȿ���;
	void updateStartupTime();								//��������ʱ��;

	quint64 generateActiveCode(quint64 sertialNum, quint8 which);					//���ɼ�����;

//���ٿ���;
	bool checkFlowOverLoaded(double flow);							//��������Ƿ����ֵ������ͣ��������;
	void syncFlowToMachine();							//�����ݶ����ٵ��ֿ�;
	quint32 updateFlowByGradientList(quint32 time);		//���ݶ�Ӧ��ʱ��ֵ�����������;
	quint32 compensationForFlow(double flow);			//�������������������������;
	double compensationForFlowSecondary(double flow);	//͹�ֲ�������;

    //��ȡ�ݶȱ�ĵ�n������;
    double getGradientTableFlow(quint32 nWhich, quint32 nRow = 0);

	//���ٰٷֱȵĸ���;
	void updateFlowPercentInPcMode(quint32 add, bool manual = false);					//PCģʽ�µİٷֱȸ���;
	void updateFlowPercentLocal(quint32 add);							//��PCģʽ�µİٷֱȸ���;

	//��ͬģʽ�µ����ٿ���;
	void updateFlowInPcMode(double flow);							//PCģʽ�µ����ٿ���;
	void updateFlowInGradientMode(double flow);						//�����ݶ�ģʽ�µ����ٿ���;
	void updateFlowInPumpPurgeMode(double flow);					//�����á���ϴģʽ�µ����ٿ���;
	void updateFlowInDebugMode(double flow);						//����ģʽ�µ����ٿ���;

	//void syncFlowFromPc();											//��PC���õ�����ͬ����MCU;
	void syncFlowToMcu(double flow);								//������ת�ɳ�MCU�����֣�ͨ�����ڷ��͸�MCU;
	
	
//ѹ�����;
	double compensationForPress(uint32 arg);			//ѹ������;
	void checkPressOverloaded(double press);			//���ѹ���Ƿ��ޣ�����Ӧ����Ӧ������ʩ;
	void initPressPoll();								//ѹ�����ݳس�ʼ��;
	double getAverageOfPress(double au);				//ȡѹ��ƽ��ֵ;

	//Ȩ�����;
	void serialNumberGenerate();								//����������к�;
	quint32 getTryDayFromActiveCode( quint64 activeNum, quint32 serialId );
	//void updateWarning();									//���±���;

	////��ȡ��ǰ����������״̬;
	//MachineStatment getMachineStat();

public slots:
	void setMachineStat(MachineStatment stat,  bool recover= false);//�л�����״̬;
	void updatePresVal(quint32 disp);						//����ѹ��ֵ;
	void dealBulge();									//����͹���ź�;
	void weepingWarn(bool flag);

private slots:
	void timeoutFunc();
	void updateFlowCtrl(quint32 data);					//�������ٿ�����;

	void shutDownWarn();
	
	/*********************************20160615*************************/
	//��ʱ�ϴ�ѹ����ʱ���ж�;
	void uploadPressTimeout_Clarity();
	/******************************************************************/

signals:
	void machineStatChanged(MachineStat::MachineStatment stat);		//����״̬�仯;
	void updatePressDisplay(QString, quint8);						//����ѹ����ʾ,�ڶ�������Ϊ���Ӳ���������ָʾ�Ƿ񱨾�
	void updateFlowDisplay(QString );								//����������ʾ;
	void updateStartupTimeDisplay(quint32);							//��������ʱ����ʾ;	

	void updateBugleCnt(quint32 cnt);								//˨��ʹ�ô�������;

	void systemError(int num, QString str);

	void updatePercentDisplay(QString);						//���°ٷֱ���ʾ�ź�;
};

#endif // MACHINESTAT_H
