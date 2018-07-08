#ifndef COMMUNICATIONCOUPLING_H
#define COMMUNICATIONCOUPLING_H

#include <QObject>
#include "Communication/Device.h"
#include "Communication/LogicThread.h"

class MachineStat;

class CommunicationCoupling : public QObject
{
	Q_OBJECT

public:
	CommunicationCoupling(MachineStat *parent);
	~CommunicationCoupling();

	void sendMcuCmd(quint8, quint32, quint32);				//发送MCU命令;
	void sendCmd(quint8, quint32, quint32);					//发送上位机命令;

	void sendCmdClarity(quint32 hAI, quint32 hPFC, quint32 hVal);		//发送上位机命令(Clarity);

	void setPcProtocol(int idx);							//设置上位机的通讯协议;
	void setConnectPort(int idx);							//设置上位机的通讯协议;
	void remoteIPChange();
	void remotePortChange();
	void localIPChange();
	void localPortChange();

	void setPumpTypeForPcFlowCtrl(int nPump);						//设置泵型，泵型不同，上位机控制流速控制字长度不同;

private:
	Worker *m_pWorker;										//后台逻辑处理;
	MachineStat *m_pMachine;								//机器状态;

	void initCommunication();								//通讯初始化;

	void sendClarityACK();									//发送回应信号;
	void sendClarityNAK();									//发送错误信号;

private slots:
	void processCmd4Mcu(quint8 type, quint32 cmd, quint32 arg,quint32 add);
	void processCmd4Pc( quint8 type, quint32 cmd, quint32 arg,quint32 add);
	void processCmd4PcClarity( quint8 hID, quint32 hAI, quint32 hPFC,quint32 hVal);
	void communicationError(int reSend);
	
signals:
	void mcuCmdSend(quint8, quint32, quint32);
	void cmdSend(quint8, quint32, quint32);
	//ack
	void ackClarity(quint8 ack);

	void cmdSendClarity(quint32, quint32, quint32);

	void s_setPcProtocol(int idx);							//设置上位机的通讯协议;
	void s_setConnectPort(int idx);							//设置上位机的通讯协议;
	void s_remoteIPChange();
	void s_remotePortChange();
	void s_localIPChange();
	void s_localPortChange();
};

#endif // COMMUNICATIONCOUPLING_H
