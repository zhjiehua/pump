#include "communicationcoupling.h"
#include "machinestat.h"
#include "timehelper.h"

#ifdef WIN32
#include <windows.h>
#else
#include <sys/unistd.h>
#endif

extern bool bSyncFlag;

extern QHostAddress remoteAddrTemp;
extern quint16 remotePortTemp;

extern QHostAddress remoteAddr;
extern quint16 remotePort;

CommunicationCoupling::CommunicationCoupling(MachineStat *parent)
	: QObject(parent)
	, m_pMachine(parent)
{
	initCommunication();
}



CommunicationCoupling::~CommunicationCoupling()
{

}




void CommunicationCoupling::initCommunication()
{
	qDebug()<<QThread::currentThread();
	m_pWorker = new Worker(this);
	connect(this, SIGNAL(cmdSend(quint8, quint32, quint32)), m_pWorker, SLOT(CmdSend(quint8, quint32, quint32)));
	connect(this, SIGNAL(mcuCmdSend(quint8, quint32, quint32)), m_pWorker, SLOT(CmdSend4Mcu(quint8, quint32, quint32)));
	connect(this, SIGNAL( cmdSendClarity(quint32, quint32, quint32) ), m_pWorker, SLOT( CmdSendClarity(quint32, quint32, quint32)) );
	connect(this, SIGNAL(ackClarity(quint8)), m_pWorker, SLOT(sendAckClarity(quint8)) );


	connect(m_pWorker, SIGNAL(process4Mcu( quint8, quint32, quint32, quint32)), this, SLOT(processCmd4Mcu(quint8, quint32, quint32, quint32)) );
	connect(m_pWorker, SIGNAL(process4Pc( quint8, quint32, quint32, quint32)), this, SLOT(processCmd4Pc(quint8, quint32, quint32, quint32)) );
	connect(m_pWorker, SIGNAL(process4PcClarity( quint8, quint32, quint32, quint32)), this, SLOT(processCmd4PcClarity(quint8, quint32, quint32, quint32)) );
	connect(m_pWorker, SIGNAL(communicationError(int)), this, SLOT(communicationError(int)) );

	connect(this, SIGNAL(s_setPcProtocol(int)), m_pWorker, SLOT(l_setPcProtocol(int)) );
	connect(this, SIGNAL(s_setConnectPort(int)), m_pWorker, SLOT(l_setConnectPort(int)) );
	connect(this, SIGNAL(s_remoteIPChange()), m_pWorker, SLOT(l_setRemoteIP()) );
	connect(this, SIGNAL(s_remotePortChange()), m_pWorker, SLOT(l_setRemotePort()) );
	connect(this, SIGNAL(s_localIPChange()), m_pWorker, SLOT(l_setLocalIP()) );
	connect(this, SIGNAL(s_localPortChange()), m_pWorker, SLOT(l_setLocalPort()) );

	setPcProtocol(DataBase::getInstance()->queryData("pcProtocol").toInt());
}




void CommunicationCoupling::sendClarityACK()
{
	emit( ackClarity(1) );
}

void CommunicationCoupling::sendClarityNAK()
{
	emit( ackClarity(0) );
}

void CommunicationCoupling::sendMcuCmd(quint8 cmd, quint32 arg, quint32 add)
{
	emit(mcuCmdSend(cmd, arg, add));
	qDebug() << "sendMcuCmd() cmd = " << arg << ";arg = " << add;//张杰华调试添加@2016-06-26
}



void CommunicationCoupling::sendCmd(quint8 cmd, quint32 arg, quint32 add)
{
	emit(cmdSend(cmd, arg, add));
}



void CommunicationCoupling::sendCmdClarity( quint32 hAI, quint32 hPFC, quint32 hVal )
{
	emit( cmdSendClarity(hAI, hPFC, hVal) );
}

void CommunicationCoupling::processCmd4Pc(quint8 type, quint32 cmd, quint32 arg, quint32 add)
{
	//TimeHelper::getComSem()->acquire();
	QString strDisp("");

	static quint8 pumpState = 0;

	remoteAddr = remoteAddrTemp;
	remotePort = remotePortTemp;

	if(cmd != 3 && cmd != 8)
		int i = 1;
	//命令处理;
	switch( cmd )
	{
		//以下需要回复数据;
	case PFC_SET_FLOW1:
		{
			//张杰华调试添加@2016-06-26
			static quint32 last_arg = 0;
			if(last_arg != arg)
			{
				last_arg = arg;
				//qDebug() << "processCmd4Pc() arg =" << arg; 
			}

			int pumpType = m_pMachine->getPumpType();
			//arg = arg & 0x3fff;				//张杰华删除@2016-06-14
			double val=0;
			switch(pumpType)
			{
			case 0:
			case 1:
				val = (double)arg/1000.0;
			break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
				val = (double)arg/10.0;
				break;
			default:
				return;
			}

			strDisp = QString("set flow1");
			m_pMachine->updateFlowPercent(add, MachineStat::PC_MODE);
			m_pMachine->updateFlow(val, MachineStat::PC_MODE);

			//张杰华添加@2016-07-26
			//if(m_pMachine->getMachineStat() == MachineStat::PCCTRL || m_pMachine->getMachineStat() == MachineStat::PURGE)
			if(pumpState == 1)
			{
				m_pMachine->syncFlowFromPc();
			}

			qDebug() << "CommunicationCoupling::processCmd4Pc()" << strDisp;
		}
		break;
	case PFC_SET_MAXPRESS:
		{
			strDisp = QString("set maxpress");
			QString data = QString::number((double)arg/100.0);
			DataBase::getInstance()->updateDate("maxpress", data);
		}
		break;
	case PFC_SET_MINPRESS:
		{
			strDisp = QString("set minpress");
			QString data = QString::number((double)arg/100.0);
			DataBase::getInstance()->updateDate("minpress", data);
		}
		break;
	case PFC_TIME_SYNC:
		{
			bSyncFlag = true; //张杰华添加@2016-06-22

			strDisp = QString("time sync");
			m_pMachine->syncTime(arg);
		}
		break;
	case PFC_START:
		{
			strDisp = QString("start");

			pumpState = 1;

			if(m_pMachine->getMachineStat() != MachineStat::PCCTRL)
			{
				m_pMachine->setMachineStat(MachineStat::PURGE); //张杰华修改@2016-06-25
#ifdef WIN32
				Sleep(5);
#else
				usleep(5000);//加一点延时，防止MCU收不到下面的流速指令
#endif
				m_pMachine->syncFlowFromPc();//modified by wjf@2016-06-22;  //其实这里会不太严谨，上一句已经向MCU发送数据库的流速，这句是发PC发过来的流速给MCU
			}
		}
		qDebug() << "CommunicationCoupling::processCmd4Pc()" << strDisp;
		break;
	case PFC_STOP:
		{
			strDisp = QString("stop");

			pumpState = 0;

			m_pMachine->setMachineStat(MachineStat::STOP);
		}
		break;
	case PFC_READ_PRESS:
		{
			strDisp = QString("read press");
			m_pMachine->uploadPresVal();
		}
		break;
	default:
		strDisp = QString("cmd not found;");
		break;
	}
	//TimeHelper::getComSem()->release();
	//qDebug() << "CommunicationCoupling::processCmd4Pc()" << strDisp;
}


void CommunicationCoupling::processCmd4PcClarity( quint8 hID, quint32 hAI, quint32 hPFC,quint32 hVal )
{
	//0x01是读取产品ID，因此不需要匹配ID;
	if(hPFC != PFCC_READ_PRODUCT_ID && hID != m_pMachine->getMachineCode())
		return;

	QString strDisp;

	static quint8 pumpState = 0;

	remoteAddr = remoteAddrTemp;
	remotePort = remotePortTemp;

	//命令处理;
	switch( hPFC )
	{
		//以下需要回复数据;
	case PFCC_READ_PRODUCT_ID:
		{
			strDisp = QString("read ID");
			sendCmdClarity(0,PFCC_READ_PRODUCT_ID,m_pMachine->getMachineCode());
		}
		break;
	case PFCC_READ_LICENSE_H:
		{
			//quint64 license = DataBase::getInstance()->queryData("license").toULong();
			quint64 license = DataBase::getInstance()->queryData("serial").toULongLong();
			license/=1000000;
			license = DectoBCD(license,6);
			sendCmdClarity(0,PFCC_READ_LICENSE_H,license);
		}
		break;
	case PFCC_READ_LICENSE_L:
		{
			//quint64 license = DataBase::getInstance()->queryData("license").toULong();
			quint64 license = DataBase::getInstance()->queryData("serial").toULongLong();
			license%=1000000;
			license = DectoBCD(license,6);
			sendCmdClarity(0,PFCC_READ_LICENSE_L,license);
		}
		break;
	case PFCC_READ_PUMPSTATUS:
		{
			strDisp = QString("read pump stat");
			quint32 val = m_pMachine->pcGetMachineStat();
			sendCmdClarity(0,PFCC_READ_PUMPSTATUS,val);
		}
		break;
	case PFCC_READ_PUMPFIX:
		{
			strDisp = QString("read pump fix");
		}
		break;
	case PFCC_READ_PUMPTIME:
		{
			strDisp = QString("read pump time");
		}
		break;
		//以下需要回复ACK或者NACK
	case PFCC_SET_FLOW:
		{
			strDisp = QString("set flow");

			int pumpType = m_pMachine->getPumpType();
			//arg = arg & 0x3fff;				//张杰华删除@2016-06-14
			double val=0;
			switch(pumpType)
			{
			case 0:
			case 1:
				val = (double)hVal/1000.0;
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				val = (double)hVal/100.0;
				break;
			case 7:
			case 8:
			case 9:
			case 10:
				val = (double)hVal/10.0;
				break;
			default:
				return;
			}

			m_pMachine->updateFlow(val, MachineStat::PC_MODE);

			//张杰华添加@2016-07-26
			//if(m_pMachine->getMachineStat() == MachineStat::PCCTRL || m_pMachine->getMachineStat() == MachineStat::PURGE)
			if(pumpState == 1)
			{
				m_pMachine->syncFlowFromPc();
			}
			sendClarityACK();
		}
		break;
	case PFCC_SET_FLOWPERCENT:
		{
			strDisp = QString("set flow percent");
			double percent = hVal/10.0;
			m_pMachine->updateFlowPercent(percent);//, MachineStat::PC_MODE
			sendClarityACK();
		}
		break;

	case PFCC_SYNCTIME:
		{
			bSyncFlag = true; //张杰华添加@2016-06-22

			strDisp = QString("time sync;");
			m_pMachine->syncTime(hVal);
			sendClarityACK();
		}
		break;
	case PFCC_MAX_PRESS:
		{
			strDisp = QString("max press;");
			QString data = QString::number((double)hVal/100.0);
			DataBase::getInstance()->updateDate("maxpress", data);
			sendClarityACK();
		}
		break;
	case PFCC_MIN_PRESS:
		{
			strDisp = QString("min press;");
			QString data = QString::number((double)hVal/100.0);
			DataBase::getInstance()->updateDate("minpress", data);
			sendClarityACK();
		}
		break;
	case PFCC_PUMPSTART:
		{
			strDisp = QString("pump start;");

			pumpState = 1;

			////m_pMachine->setMachineStat(MachineStat::PCCTRL);
			//m_pMachine->setMachineStat(MachineStat::PURGE);//张杰华修改@2016-07-01
			//m_pMachine->syncFlowFromPc();//张杰华修改@2016-07-01
			
			if(m_pMachine->getMachineStat() != MachineStat::PCCTRL)
			{
				m_pMachine->setMachineStat(MachineStat::PURGE); //张杰华修改@2016-06-25
#ifdef WIN32
				Sleep(5);
#else
				usleep(5000);//加一点延时，防止MCU收不到下面的流速指令
#endif
				m_pMachine->syncFlowFromPc();//modified by wjf@2016-06-22;  //其实这里会不太严谨，上一句已经向MCU发送数据库的流速，这句是发PC发过来的流速给MCU
			}

			sendClarityACK();
		}
		break;
	case PFCC_PUMPSTOP:
		{
			pumpState = 0;

			m_pMachine->setMachineStat(MachineStat::STOP);
			sendClarityACK();
		}
		break;
	case PFCC_PRESSCLEAR:
		{
			strDisp = QString("press clear;");
			m_pMachine->clearPress();
			sendClarityACK();
		}
		break;
	case PFCC_READ_PRESS:
		{
			strDisp = QString("read press;");
			/********************************20160615********************************/
			m_pMachine->setUploadPressFreq_Clarity(hVal);
			sendClarityACK();
			/***********************************************************************/
		}
		break;
	case PFCC_SET_MODE:
		{

			strDisp = QString("set mode;");
		}
		break;
	case PFCC_SET_FLOWFIX:
		{

			strDisp = QString("set flow fix;");
			sendClarityACK();
		}
		break;
		//主动发送;
	case PFCC_SEND_PRESS:
		{
			strDisp = QString("send press;");
		}
		break;
	case PFCC_INPUT_EVENT:
		{

			strDisp = QString("input event;");
		}
		break;
	case PFCC_SYS_ERR:
		{

			strDisp = QString("sys error;");
		}
		break;

	default:
		strDisp = QString("cmd can not distinguish!!!;");
		//SendNAK();
		break;
	}
	qDebug()<<strDisp;
}

void CommunicationCoupling::communicationError(int error)
{
	m_pMachine->sysError(COMUNICATION_ERR, error);
}

void CommunicationCoupling::processCmd4Mcu(quint8 type, quint32 cmd, quint32 arg,quint32 add)
{
	QString strDisp;
	switch( cmd )
	{
	case MCU_SET_PARAM:
		{
			strDisp = QString("mcu set param;");
		}
		break;
	case MCU_MOTOR_INI:
		{
			strDisp = QString("mcu motor ini;");
		}
		break;
	case MCU_WAVEADD_MOTOR:
		{

		}
		break;
	case MCU_WAVEDEC_MOTOR:
		{

		}
		break;
	case MCU_READ_PARAM:
		{
			strDisp = QString("mcu read param;");
		}
		break;
	case MCU_READ_VERSION:
		{
			strDisp = QString("mcu read version;");
		}
		break;
	case MCU_READ_AU_VAL:
		{
			strDisp = QString("mcu read au val;");
			m_pMachine->updatePresVal(arg);
			//qDebug() << "PRESS = " << arg;
		}
		break;
	default:
		strDisp = QString("mcu command not found;");
		break;
	}
}

void CommunicationCoupling::setPcProtocol(int idx)
{
	//m_pWorker->setPcProtocol(idx);
	emit(s_setPcProtocol(idx));
}

void CommunicationCoupling::setConnectPort(int idx)
{
	//m_pWorker->setConnectPort(idx);
	emit(s_setConnectPort(idx));
}

void CommunicationCoupling::remoteIPChange()
{
	//m_pWorker->setRemoteIP();
	emit(s_remoteIPChange());
}

void CommunicationCoupling::remotePortChange()
{
	//m_pWorker->setRemotePort();
	emit(s_remotePortChange());
}

void CommunicationCoupling::localIPChange()
{
	//m_pWorker->setLocalIP();
	emit(s_localIPChange());
}

void CommunicationCoupling::localPortChange()
{
	//m_pWorker->setLocalPort();
	emit(s_localPortChange());
}

void CommunicationCoupling::setPumpTypeForPcFlowCtrl( int nPump )
{
	sendCmd(PROTOCL_LOCAL_USE_CHANGE_PUMPTYPE, nPump, 0);
}
