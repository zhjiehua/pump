#ifndef LOGICTHREAD_H
#define LOGICTHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QUdpSocket>
#include "Protocol.h"


#define TIMEOUT_PERIOD 50


#ifdef WIN32
#include "Communication/win_qextserialport.h"
#elif linux
#include "Communication//posix_qextserialport.h"
#endif

#if 0
//以下为返回16字节完整数据的命令;
#define PFC_READ_PRODUCT_ID			0x01//读取产品ID;
#define PFC_READ_LICENSE_H			0x02//读取序列号高位;
#define PFC_READ_LICENSE_L			0x03//读取序列号低位;
#define PFC_READ_STATUS_1			0x04//读取检测器状态1(灯状态与当前波长);
#define PFC_READ_STATUS_2			0x05//读取检测器状态2（时间常数与输出范围);
#define PFC_READ_VERSION			0x06//读取检测器软件版本号;
#define PFC_READ_REF				0x07//读取ref*;
#define PFC_READ_SIG				0x08//读取sig*;

//以下为返回ACK，NACK的命令;
#define PFC_WAVE_LENGTH				0x10//波长;
#define PFC_TIME_CONST				0x11//时间常数;
#define PFC_OUTPUT_EXTENT			0x12//输出范围;
#define PFC_SYNC_TIME				0X13//设定同步时间;
#define PFC_LIGHT_TURN_ON			0x14//开灯;
#define PFC_LIGHT_TURN_OFF			0x15//关灯;
#define PFC_WAVE_LEN_INI			0x16//波长初始化;
#define PFC_AUTO_RESET				0x17//自动清零命令;
#define PFC_SET_AU_FREQ				0x18//设定Au值发送频率命令;
#define PFC_SET_OUTPUT_EVENT		0x19//设定输出事件;
#define PFC_LIGHT_SEL				0x20//氘灯与钨灯选择;

//以下为下位机主动发送的命令;
#define PFC_READ_AU					0x90//读Au值命令;
#define PFC_INPUT_EVENT				0x91//发送输入事件;
#define PFC_SYS_ERR					0x92//发送系统故障;
#define PFC_STAT_CHANGE_1			0x93//仪器键盘输入导致检测器检测器状态变化1(灯状态与当前波长);
#define PFC_STAT_CHANGE_2			0x94//仪器键盘输入导致检测器检测器状态变化2(时间常数与输出范围);
#endif

#define PFCC_READ_PRODUCT_ID		0x01//读取产品ID;
#define PFCC_READ_LICENSE_H			0x02//读取序列号高位;
#define PFCC_READ_LICENSE_L			0x03//读取序列号低位;
#define PFCC_READ_PUMPSTATUS		0x04//读取泵运行状态;
#define PFCC_READ_PUMPFIX			0x05//读取泵流量校正参数;
#define PFCC_READ_PUMPTIME			0x06//读取泵运行总时间;


//以下为返回ACK，NACK的命令;
#define PFCC_SET_FLOW				0x10//设定泵流量;
#define PFCC_SET_FLOWPERCENT		0x11//设定泵流量百分比;
#define PFCC_SYNCTIME				0x12//设定同步时间;
#define PFCC_MAX_PRESS				0X13//设定最大压力;
#define PFCC_MIN_PRESS				0x14//设定最小压力;
#define PFCC_PUMPSTART				0x15//泵启动命令;
#define PFCC_PUMPSTOP				0x16//泵停止命令;
#define PFCC_PRESSCLEAR				0x17//压力自动清零命令;
#define PFCC_READ_PRESS				0x18//读取压力并设定压力发送频率命令
#define PFCC_SET_MODE				0x19//设定泵模式;
#define PFCC_SET_FLOWFIX			0x40//设定泵校流量正参数;

//以下为下位机主动发送的命令;
#define PFCC_SEND_PRESS					0x90//读Au值命令;
#define PFCC_INPUT_EVENT				0x91//发送输入事件;
#define PFCC_SYS_ERR					0x92//发送系统故障;







#define PFC_SET_FLOW1				0x08//设置冲洗流速; 
#define PFC_SET_MAXPRESS			0x09//设置最大压力;
#define PFC_SET_MINPRESS			0x0A//设置最小压力;
#define PFC_SET_PUMPMOD				0x0C//设置泵工作模式;
#define PFC_PRESS_FIX				0x0D//压力校正;
#define PFC_TIME_SYNC				0x0E//时间同步命令;
#define PFC_PRESS_FIXW				0x0F//压力校准写入;
#define PFC_FLOW_FIXW				0x0B//流量校准参数吸入;

#define PFC_PUMP					0x0//Pump;
#define PFC_PURGE					0x01//Purge
#define PFC_START					0x0//Start
#define PFC_HOLD					0x05//Hold
#define PFC_STOP					0x02//Stop
#define PFC_READ_PRESS				0x03//读取压力命名2;
#define PFC_SET_FLOW2				0x06//设置冲洗流量2;
#define PFC_DOWNLOAD_PROGRAM		0x07//下载程序;



/******************************MUC通讯命令识别**********************************/
#define MCU_SET_PARAM				0x01//设置参数;
#define MCU_MOTOR_INI				0x02//马达初始化;
#define MCU_WAVEADD_MOTOR			0x04//波长增加方向转动马达;
#define MCU_WAVEDEC_MOTOR			0x05//波长减小方向转动马达;
#define MCU_READ_PARAM				0x09//读取参数;
#define MCU_READ_VERSION			0x0a//读取分控版本号;
#define MCU_READ_AU_VAL				0x0b//读取AU值;
#define MCU_READ_AU_VALB			0x0c//读取AU值;



struct SendMcuData{
	mbyte type;
	uint32 cmd; 
	uint32 arg;
};



/*负责后台逻辑处理;*/
class Worker : public QObject
{

	Q_OBJECT
public:
	Worker(QObject *parent = 0);
	~Worker();

	void startTimer();
	void setPcProtocol(const int &nPro){ m_nPcProtocol = nPro; };
	int getPcProtocol(){return m_nPcProtocol;};

	//! 网络相关接口;
	//! 获取PC通讯端口类型(串口或网口);
	quint32 getConnectPortType(){return m_nConnectPort;}
	//! 获取远端IP;
	QString getRemoteIP(){return m_strRemoteIp;}
	//! 获取远端端口;
	quint32 getRemotePort(){return m_nRemotePort;}

	//tcp
	QTcpServer *tcpSocketServer;				//张杰华添加@2016-06-22，设置网口通信
	QTcpSocket *tcpSocketClientConnection;
	void setupNetworkCommunication();
	//udp
	QUdpSocket *udpSocket;
	void setupMCUCommunication();

private:
	int m_nPcProtocol;							//上位机通讯协议选择(0 旧协议， 1新协议);
	QThread m_workerThread;
	QTimer *m_pTimer;								//用于定时读取通讯数据的定时器;
	quint8 abc;
	QList< QList<quint32> >sendList4Mcu;			//发送列表;
	QList< QList<quint32> >sendList4Pc;				//发送列表;
	quint32 m_nResendTimeout;						//重发计时;
	quint32 m_nResendCnt;							//重发计数;

	//! 上位机的通讯接口(串口、网口);
	quint32 m_nConnectPort;

	//! 本地网络参数;
	quint32 m_nLocalPort;

	//! 远程网络参数;
	quint32 m_nRemotePort;
	QString m_strRemoteIp;


	void setupCommunication();						//设置通讯，可以是串口或者网口;
	void setupSerialCommunication();				//设置串口通讯;

signals:
	void process4Mcu( quint8 type, quint32 cmd, quint32 arg, quint32 add);
	void process4Pc( quint8 type, quint32 cmd, quint32 arg, quint32 add);
	void process4PcClarity( quint8 hID, quint32 hAI, quint32 hPFC, quint32 nVal);
	void communicationError(int reSend);

	

private slots:
	void CmdSend(quint8 type, quint32 cmd, quint32 arg);
	void CmdSend4Mcu(quint8 type, quint32 cmd, quint32 arg);
	void CmdSendClarity(quint32 hAI, quint32 hPFC, quint32 hVal);

	void sendAckClarity(quint8 ack);

public slots:
	void timeoutFunc();
	void processCmd4Pc( mbyte type, uint32 cmd, uint32 arg, uint32 add);				//对接收到的数据进行处理;
	void processCmd4Clarity(mbyte hID, mbyte hAI, mbyte hPFC, uint32 nVal);		//对接收到的数据进行处理，基于Clarity协议;
	void processCmd4Mcu( mbyte type, uint32 cmd, uint32 arg, uint32 add);			//对接收到的数据进行处理;
	void check4Mcu(uint32 cmd);														//校验;

	//张杰华添加@2016-06-22，添加socket通信
	//tcp
	void acceptConnection();
	void readClient();
	//udp
	void processPendingDatagrams();

	void l_setConnectPort(int nCon);
	void l_setPcProtocol(int nPro);
	void l_setRemoteIP();
	void l_setRemotePort();
	void l_setLocalIP();
	void l_setLocalPort();
};



#endif // LOGICTHREAD_H
