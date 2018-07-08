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

#define NOWARN			 0//无报警
#define COMUNICATION_ERR 1//通讯出错;
#define OVERFLOW_ERR	 2//流速超限;
#define OVERPRESS_ERR	 3//压力超限;
#define WEEPING          4//漏液

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
		SYSTIME,		//系统累计时间,开机不清零;
		PUMPTIME,		//泵累计运行时间,开机不清零;
		STARTUPTIME,	//开机时间,开机清零;
	};

	enum MachineStatment
	{
		INISTAT = -1,	//初始状态,开机时候处于该状态;
		STOP = 0,		//停止状态;
		PAUSE,			//暂停状态;
		RUNNING,		//运行状态;
		PUMP,			//泵液状态;
		PURGE,			//清洗状态;
		PCCTRL,			//上位机控制状态;
		FIX,			//校正模式;
	};

	//流速控制模式;
	enum FlowCtrlMode
	{
		GRADIENT_MODE,	//梯度模式;
		PUMP_PURGE_MODE,//泵或清洗模式;
		PC_MODE,		//上位机模式;
		DEBUG_MODE,		//调试模式;
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
		uint m_firstTryDateTime;					//记录试用当日的日期时间，不清零;
		quint32 m_nSysUsedTime;						//! 系统使用时间
		quint32 pumpStartupTime;
		quint32 bugleCnt;
		bool bTryDone;								//试用完毕标志;

		quint32 m_nSerialId;
		int m_nPumpType;							//泵类型;

		bool m_bUpdateFlowFromPc;					//标志从上位机设置了流速和百分比;

		double m_dCurrentFlow;						//当前流速;

		bool m_bFixMode;
		
	};

	//张杰华添加@2016-06-23
	bool m_bCanClearOverPress;

	bool m_bPumpTypeChange;

	~MachineStat();
	static MachineStat *getInstance();
	void setFixMode(bool mode);										//进入校准模式;
	bool isFixMode();
	void clearPressWarn();									//清除压力报警;
	void sysError(int reSend, bool insert);					//插入报警;
	void setPcProtocol(int idx);
	void setConnectPort(int idx);

	int checkProbation();									//检查使用剩余时间;
	quint32 getTime(MachineTime time);						//获取时间;
	void clearTime(MachineTime time);						//清除时间;
	bool activeMachine(quint64 activeNum, bool bActive = true);						//通过激活码激活机器状态;	
	bool isTryDone(){return m_machineStat.bTryDone;}									//检查是否过了试用期;

	void enableUpdateFlow();																//更新流速电机使能;
	void disableUpdateFlow();																//更新流速电机除能;
	void updateFlow(double flow, MachineStat::FlowCtrlMode eFlowMod);						//更新流速(流速，流速控制模式);
	void updateFlowPercent(quint32 percent, MachineStat::FlowCtrlMode eFlowMod);			//流速更新;
	void updateFlowPercent(double percent);
	float GetWordFactor();									//读取配置流速;
	void clearPress();										//压力清零;
	void syncTime(quint32 time);							//上位机时间同步;
	
	void uploadPresVal();									//上传压力到上位机;
	/*********************************20160615*************************/
	void setUploadPressFreq_Clarity(int freq);				//设置上传压力频率;
	/*****************************************************************/
	//void updatePresVal(uint32 disp);						//更新压力值;
	
	MachineStatment getCurrentStat(){
		return m_machineStat.machineStat;
	}														//获取当前机器状态;
	void restoreOldStat(){
		m_machineStat.machineStat = m_machineStat.oldMachineStat;//张杰华添加@2016-06-25
		setMachineStat(m_machineStat.oldMachineStat);
	};	//恢复上一状态;

	void initMACAddr();
	void initIPAddr();//初始化ip地址;
	void setNetWorkConfig(MachineStat::E_IPConfig eConfig);//配置本地和远程IP/PORT
	quint32 getMaxFlow();//获取流速最大值;
	quint32 getMaxPress();//获取流速最大值;

	FlowCtrl *getFlowCtrl();

	void pumpTypeChanged(bool flag = false);//泵型发生改变;
	int getPumpType();	//获取当前泵类型;

	void updateSerialId(quint32 id);						//更新序列号的随机ID;

	void isUpdateFlowFromPC();								//判断是否刚从PC更新了流速,是则更新显示;

	/*0XXXXX 表示泵处在停止状态，且XXXXX为泵运行当前设定流速。
	1XXXXX 表示泵处在运行状态，且XXXXX为泵运行当前设定流速。
	流速数据格式为固定三位小数，单位为ml/min。*/
	quint32 pcGetMachineStat();								//获取状态1; 
	
	//返回设备码，不同泵型对应不同设备码;
	quint32 getMachineCode();
	//设置设备码;
	void updateMachineCode();

	void updateWarning();			//更新报警;//吴杰能添加@2016-06-18，将这个私有方法改为公有

	void syncFlowFromPc();			//将PC设置的流速同步到MCU;//wjf添加@2016-06-22,从私有改公有;

	//获取当前机器的运行状态;
	MachineStatment getMachineStat();//张杰华添加@2016-07-15,从私有改公有;

	bool pwdOK;
	bool pwdNeed;
	USRTYPE usrType;
	int whichPage;  //如果密码正确将跳到的页面

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
	struct MachineRunningStat m_machineStat;				//机器状态;
	DataBase *pDb;
	CommunicationCoupling *m_pCommunicationCoupling;		//通讯胶合层;
	FlowCtrl *m_pFlowCtrl;									//流速控制;
	quint32 m_nCurrentPressRawCode;							//记录最新从分控MCU中收到的压力字;
	QString m_sCurrentPressVal;								//记录最新的压力值，经过补偿各种处理;
	double m_dCurrentflowValInPc;							//记录PC设置的最新流速值;
	quint32 m_nCurrentPercent;								//记录PC设置的最新百分比;
	double m_dFlowPercent;									//流速百分比（单机）;
	//double m_dPumpOrPurgeFlowVal;							//记录用于PUMP和PURGE状态的流速值;
	QList<LPTimeOutStruct> m_timeoutList;					//定时任务列表;
	QList<int>warningList;									//报警队列;
//流速相关;
	//梯度表和校正表的生成;
	bool m_bPcGradientCtrl;									//上位机梯度控制标志位;
	quint8 m_nPcGradientCtrlFlag;							//上位机梯度控制计时，如果超时没有收到上位机的同步时间命令则停止运行;
	quint32 m_nFlowCtrlWord;								//流速控制字;
	
	//凸轮补偿;
	QTime bugleTime;
	bool m_bBugleFlag;

	//压力相关;
	QList<double>pressList;									//压力数据池;

//methods:
//初始化;
	void initDb();	
	void initIO();
	void initTimer();
	void initMachineStat();
	void initFlowCtrl();
	void initLogicThread();									//初始化后台通讯处理线程;
	void initTimeoutList();									//初始化定时器列表;
	void initCommunication();								//通讯初始化;
//释放;
	void releaseTimeoutList();								//释放定时器列表;
//机器状态控制;
	void storeOldStat(MachineStatment stat);
	void stopMachine();			//停止机器;
	void pauseMachine();		//暂停机器;
	void startMachine();		//开始运行;
	void pumpMachine();			//泵液;
	void purgeMachine();		//清洗;
	void pcCtrlMachine();		//上位机控制;
//时间相关;
	void setStartupTime(quint32 time);
	inline void clearStartupTime();	//状态切换时候清除运行时间;
	void clearUsedTime();
	void clearBugleCnt();//清栓塞使用次数
	void clearStartupUsedTime(){m_machineStat.startupTime = 0;}
	uint getUsedTime();											//获取系统使用时间;
	void saveUsedTime();			//使用时间记录保存;
	void registerTimeoutFunc(const quint32 time, pTimeoutFunc pFunc);
	void saveDataBase(){pDb->saveDb();}						//保存数据库;
	void checkTryOut();										//检查试用期是否超时;
	void checkFlowCtrlByPc();								//检查上位机梯度控制;
	void updateStartupTime();								//更新启动时间;

	quint64 generateActiveCode(quint64 sertialNum, quint8 which);					//生成激活码;

//流速控制;
	bool checkFlowOverLoaded(double flow);							//检测流速是否超最大值，超则停机并警告;
	void syncFlowToMachine();							//更新梯度流速到分控;
	quint32 updateFlowByGradientList(quint32 time);		//根据对应的时间值，查表获得流量;
	quint32 compensationForFlow(double flow);			//根据流量补偿表进行流量补偿;
	double compensationForFlowSecondary(double flow);	//凸轮补偿流速;

    //获取梯度表的底n行流速;
    double getGradientTableFlow(quint32 nWhich, quint32 nRow = 0);

	//流速百分比的更新;
	void updateFlowPercentInPcMode(quint32 add, bool manual = false);					//PC模式下的百分比更新;
	void updateFlowPercentLocal(quint32 add);							//非PC模式下的百分比更新;

	//不同模式下的流速控制;
	void updateFlowInPcMode(double flow);							//PC模式下的流速控制;
	void updateFlowInGradientMode(double flow);						//单机梯度模式下的流速控制;
	void updateFlowInPumpPurgeMode(double flow);					//单机泵、清洗模式下的流速控制;
	void updateFlowInDebugMode(double flow);						//调试模式下的流速控制;

	//void syncFlowFromPc();											//将PC设置的流速同步到MCU;
	void syncFlowToMcu(double flow);								//将流速转成成MCU控制字，通过串口发送给MCU;
	
	
//压力相关;
	double compensationForPress(uint32 arg);			//压力补偿;
	void checkPressOverloaded(double press);			//检查压力是否超限，超限应做相应保护措施;
	void initPressPoll();								//压力数据池初始化;
	double getAverageOfPress(double au);				//取压力平均值;

	//权限相关;
	void serialNumberGenerate();								//随机生成序列号;
	quint32 getTryDayFromActiveCode( quint64 activeNum, quint32 serialId );
	//void updateWarning();									//更新报警;

	////获取当前机器的运行状态;
	//MachineStatment getMachineStat();

public slots:
	void setMachineStat(MachineStatment stat,  bool recover= false);//切换机器状态;
	void updatePresVal(quint32 disp);						//更新压力值;
	void dealBulge();									//处理凸轮信号;
	void weepingWarn(bool flag);

private slots:
	void timeoutFunc();
	void updateFlowCtrl(quint32 data);					//更新流速控制字;

	void shutDownWarn();
	
	/*********************************20160615*************************/
	//定时上传压力定时器中断;
	void uploadPressTimeout_Clarity();
	/******************************************************************/

signals:
	void machineStatChanged(MachineStat::MachineStatment stat);		//机器状态变化;
	void updatePressDisplay(QString, quint8);						//更新压力显示,第二个参数为附加参数，可以指示是否报警
	void updateFlowDisplay(QString );								//更新流速显示;
	void updateStartupTimeDisplay(quint32);							//更新运行时间显示;	

	void updateBugleCnt(quint32 cnt);								//栓塞使用次数更新;

	void systemError(int num, QString str);

	void updatePercentDisplay(QString);						//更新百分比显示信号;
};

#endif // MACHINESTAT_H
