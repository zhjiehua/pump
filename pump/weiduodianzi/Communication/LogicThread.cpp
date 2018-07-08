#include "LogicThread.h"
#include "Communication/Protocol.h"
#include <QString>
#include "baseMainPage.h"
#include "timehelper.h"

#ifdef linux
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <asm-generic/ioctl.h>
#include <fcntl.h>


static int fd = -1;
static unsigned long temp_read = 0;
#endif

#define LOCAL_PORT	8080  //本地端口
#define REMOTE_PORT	8090  //远程端口
#define REMOTE_IPADDRESS  "192.168.19.254"	//远程IP

//#define COMMUNICATION_NETWORK//网络通讯;
#define COMMUNICATION_SERIAL//串口通讯;
#define COMMUNICATION_WITH_MCU//是否用串口和MCU通讯;

#ifdef WIN32
#define MAINCOMPORT		"COM6"//上位机通讯接口
#define MCUCOMPORT		"COM8"//单片机通讯接口
#elif linux
#define MAINCOMPORT		"/dev/ttySAC2"//上位机通讯接口
#define MCUCOMPORT		"/dev/ttySAC1"//单片机通讯接口
#endif


Worker *g_pWorker;

QHostAddress remoteAddrTemp;
quint16 remotePortTemp;

QHostAddress remoteAddr;
quint16 remotePort;

#ifdef WIN32
Win_QextSerialPort *myCom = 0;
#ifdef COMMUNICATION_WITH_MCU
Win_QextSerialPort *mcuCom = 0;//与底板MCU通讯;
#endif
#elif linux
Posix_QextSerialPort *myCom = 0;
#ifdef COMMUNICATION_WITH_MCU
Posix_QextSerialPort *mcuCom = 0;//与底板MCU通讯;
#endif
#endif


//通讯准备;
#define PC_COMMUNICATION_PORT_UART  0
#define PC_COMMUNICATION_PORT_NET   1


qint16 writeFunc( quint8* data, uint16 sz )
{
	qint16 ret = -1;
	if(g_pWorker->getConnectPortType() == PC_COMMUNICATION_PORT_UART)
	{
		if(myCom)
		{
			ret = myCom->write((const char*)data, sz);//张杰华修改@2016-06-23
		}
	}
	else
	{
		//QByteArray data;  
		//data.append(text.toUtf8());
		if(g_pWorker->udpSocket)
		{
			ret = g_pWorker->udpSocket->writeDatagram((const char*)data, sz, QHostAddress(g_pWorker->getRemoteIP()), g_pWorker->getRemotePort());

			//if(g_pWorker->getPcProtocol() == 0)
			//{
			//	ret = g_pWorker->udpSocket->writeDatagram((const char*)data, sz, QHostAddress(g_pWorker->getRemoteIP()), g_pWorker->getRemotePort());
			//}
			//else
			//{
			//	ret = g_pWorker->udpSocket->writeDatagram((const char*)data, sz, remoteAddr, remotePort);
			//}
			
		}

	}

	return ret;
}

void processFunc( mbyte type, uint32 cmd, uint32 arg, quint32 add)
{
	g_pWorker->processCmd4Pc( type, cmd, arg, add);
}


void processFunc4Clarity(  mbyte hID, mbyte hAI, mbyte hPFC, uint32 nVal )
{
	g_pWorker->processCmd4Clarity( hID, hAI, hPFC, nVal);
}


//*****************MCU通讯********************************/
qint16 writeFunc4Mcu( quint8* data, uint16 sz )
{
	qint16 ret = -1;
#ifdef COMMUNICATION_WITH_MCU
	if(mcuCom)
		ret = mcuCom->write((const char*)data, sz);
#endif
	return ret;
}

void processFunc4Mcu( mbyte type, uint32 cmd, uint32 arg, quint32 add )
{
	g_pWorker->processCmd4Mcu( type, cmd, arg, add);
}

void checkFunc4Mcu(uint32 cmd)
{
	g_pWorker->check4Mcu(cmd);
}



/*************************************************Worker*********************************************************/

Worker::Worker(QObject *parent /*= 0*/)
{
	setupCommunication();
	m_pTimer = new QTimer(this);
	QObject::connect(m_pTimer, SIGNAL(timeout()), this, SLOT(timeoutFunc()) );//定时读取数据;
	startTimer();
	//connect(mcuCom, SIGNAL(readyRead()), this, SLOT(timeoutFunc()));
	moveToThread(&m_workerThread);
	g_pWorker = this;
	m_nResendTimeout = 0;
	m_nResendCnt = 0;
	m_workerThread.start();
	//m_nConnectPort = 0;
}



Worker::~Worker()
{
	
}



void Worker::timeoutFunc()
{
	static bool errorFlag = false;
#if 1
	if(sendList4Mcu.count() > 0)
	{
		if(++m_nResendTimeout >3)
		{
			m_nResendTimeout = 0;
			QList<quint32>list = sendList4Mcu.at(0);
			API_McuCmdSend(list.at(0), list.at(1),list.at(2));

			m_nResendCnt++;
			if(m_nResendCnt > 5)
			{
				errorFlag = true;
				emit(communicationError(1));
			}

		}
	}
	else
	{
		m_nResendTimeout = 0;
		m_nResendCnt = 0;
		if(errorFlag)
		{
			errorFlag = false;
			emit(communicationError(0));
		}
	}
#endif



	//发送读取压力值命令;
	static int nReadPressPeriod = 0;
	if(nReadPressPeriod++ > 7 )
	{
		nReadPressPeriod = 0;
		API_McuCmdSend(PROTOCL_CMD, MCU_READ_AU_VAL,0);
	}
	


	QByteArray data;
	QByteArray data2;

	if(getConnectPortType() == PC_COMMUNICATION_PORT_UART)
	{
		if(myCom)
			data  = myCom->readAll();
	}

#ifdef COMMUNICATION_WITH_MCU
	if(mcuCom)
		data2 = mcuCom->readAll();
#endif



#if 0
	{
		qDebug()<<"recv:"<<data.size();
		QString temp;
		for(int i = 0; i < data.size(); i++)
			temp+=QString(" %1").arg( (quint8)data.at(i) );
		qDebug()<<temp;
	}
#endif
#if 0
	{
		if(data2.size())
		{
			qDebug()<<"mcu port recv:"<<data2.size();
			QString temp;
			for(int i = 0; i < data2.size(); i++)
			{
				quint8 d = (quint8)(data2.at(i));
				//temp+=QString(" %1").arg(d);
				temp += QString::number(data2.at(i),16).right(2)+"-";
			}
			qDebug()<<temp;
		}
		
	}
#endif

#if 1
	//解析协议;
	if(data.size() > 0)
	{
		if(m_nPcProtocol == 0)
			API_Protocol((mbyte *)data.data(), data.size());			//解析旧协议;
		else
			API_ClarityProtocol((mbyte *)data.data(), data.size());	//解析新协议;
	}
#endif

#if 1
	if(data2.size() > 0)
		API_McuProtocol((mbyte *)data2.data(), data2.size());//MCU协议解释;
#endif

}



void Worker::processCmd4Pc( mbyte type, uint32 cmd, uint32 arg, uint32 add)
{
	//TimeHelper::getComSem()->acquire();
	emit(process4Pc(type, cmd, arg, add));
	//TimeHelper::getComSem()->release();
}




void Worker::processCmd4Clarity(mbyte hID, mbyte hAI, mbyte hPFC, uint32 nVal)
{
	emit( process4PcClarity(hID, hAI, hPFC, nVal) );
}



void Worker::processCmd4Mcu(mbyte type, uint32 cmd, uint32 arg,uint32 add)
{
	emit(process4Mcu( type, cmd, arg, add));
}


void Worker::CmdSend(quint8 type, quint32 cmd, quint32 arg)
{
	if(type == PROTOCL_LOCAL_USE_CHANGE_PUMPTYPE)
	{
		//改变泵型,从而改变上位机流速命令字节长度的识别;
		API_SetPumpType(cmd);
	}
	else
		API_CmdSend(type, cmd, arg);
}





//加入发送队列;
void Worker::CmdSend4Mcu(quint8 type, quint32 cmd, quint32 arg)
{
	QList<quint32>list;
	list.append(type);
	list.append(cmd);
	list.append(arg);
	sendList4Mcu.append(list);
}

void Worker::CmdSendClarity( quint32 hAI, quint32 hPFC, quint32 hVal )
{
	API_CmdSendClarity(hAI, hPFC, hVal);
}



void Worker::sendAckClarity( quint8 ack )
{
	ack ? SendACK() : SendNAK();
}

void Worker::check4Mcu( uint32 cmd )
{
	//qDebug()<<QThread::currentThread();
	//qDebug()<<sendList4Mcu.count();
	if(sendList4Mcu.count() > 0)
	{
		if((cmd&0x0f) == sendList4Mcu.at(0).at(1))
			sendList4Mcu.removeAt(0);
	}
}

//通讯准备;
void Worker::setupCommunication()
{
	m_nConnectPort = DataBase::getInstance()->queryData("connect_port").toUInt();
	//! 判断使用串口还是网口和PC通讯;
	if( m_nConnectPort == PC_COMMUNICATION_PORT_UART )
		setupSerialCommunication();
	else
		setupNetworkCommunication();//张杰华添加@2016-06-23

	setupMCUCommunication();

	//注册通讯函数;
	ProtocolConf proConf;
	proConf.write = writeFunc;								//实现此函数!!!!!!!!!!!!!!!!!!!!!;
	proConf.process = processFunc;							//实现此函数!!!!!!!!!!!!!!!!!进行命令处理;
	proConf.processClarity = processFunc4Clarity;			//基于新协议进行数据处理;
	SetProtocolConf( &proConf, PROTOCOL_CONF_WRITEFUN|PROTOCOL_CONF_PROCESSFUN|PROTOCOL_CONF_PROCESSFUNCLARITY);

#ifdef COMMUNICATION_WITH_MCU
	//注册MCU通讯函数;
	Protocol4McuConf mcuProConf;
	mcuProConf.write = writeFunc4Mcu;
	mcuProConf.process = processFunc4Mcu;
	mcuProConf.checkCompare = checkFunc4Mcu;
	SetMcuProtocolConf(&mcuProConf, PROTOCOL_CONF_PROCESSFUN|PROTOCOL_CONF_WRITEFUN|PROTOCOL_CONF_CHECKFUN);
#endif

}



//张杰华添加@2016-06-22，添加socket通信
void Worker::acceptConnection()
{
	tcpSocketClientConnection = tcpSocketServer->nextPendingConnection();
	connect(tcpSocketClientConnection, SIGNAL(readyRead()), this, SLOT(readClient()));
}

void Worker::readClient()
{
	QString str = tcpSocketClientConnection->readAll();
	//或者
	char buf[1024];
	tcpSocketClientConnection->read(buf,1024);
}


void Worker::processPendingDatagrams()  
{   

	while(udpSocket -> hasPendingDatagrams())  
	{   
		QByteArray datagram;  
		datagram.resize(udpSocket->pendingDatagramSize());   
		udpSocket->readDatagram(datagram.data(), datagram.size(), &remoteAddrTemp, &remotePortTemp);   
		//QString messages = QString::fromUtf8(datagram); 

		if(m_nPcProtocol == 0)
			API_Protocol((mbyte *)datagram.data(), datagram.size());			//解析旧协议;
		else
			API_ClarityProtocol((mbyte *)datagram.data(), datagram.size());		//解析新的协议;
	}  
} 

//张杰华添加@2016-06-22，添加socket通信
void Worker::setupNetworkCommunication()
{
	//tcpSocketServer = new QTcpServer();
	//tcpSocketServer->listen(QHostAddress::Any, 6665);
	//connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));

	//! 读取网络参数;
	//! 本地端口号;
	m_nLocalPort = DataBase::getInstance()->queryData("port").toUInt();
	//! 远程端口和IP;
	m_nRemotePort = DataBase::getInstance()->queryData("remote_port").toUInt();
	m_strRemoteIp =  QString("%1.%2.%3.%4").arg(DataBase::getInstance()->queryData("remote_ip1")).arg(DataBase::getInstance()->queryData("remote_ip2")).arg(DataBase::getInstance()->queryData("remote_ip3")).arg(DataBase::getInstance()->queryData("remote_ip4"));
	
	if(udpSocket != NULL)
		udpSocket->deleteLater();

	udpSocket = new QUdpSocket(this);  
	//udpSocket->bind (QHostAddress("192.168.19.254"), LOCAL_PORT, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);  
	udpSocket->bind (m_nLocalPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);   //注意：bind的是本地IP和本地端口，发送的是远端IP和远端端口
	connect (udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));  
}

void Worker::l_setConnectPort(int nCon)
{ 
	m_nConnectPort = nCon; 
}

void Worker::l_setPcProtocol(int nPro)
{ 
	m_nPcProtocol = nPro; 
}

void Worker::l_setRemoteIP()
{
	m_strRemoteIp =  QString("%1.%2.%3.%4").arg(DataBase::getInstance()->queryData("remote_ip1")).arg(DataBase::getInstance()->queryData("remote_ip2")).arg(DataBase::getInstance()->queryData("remote_ip3")).arg(DataBase::getInstance()->queryData("remote_ip4"));
}

void Worker::l_setRemotePort()
{
	m_nRemotePort = DataBase::getInstance()->queryData("remote_port").toUInt();
}

void Worker::l_setLocalIP()
{

}

void Worker::l_setLocalPort()
{
	m_nLocalPort = DataBase::getInstance()->queryData("port").toUInt();

	if(udpSocket != NULL)
		udpSocket->deleteLater();

	udpSocket = new QUdpSocket(this); 
	udpSocket->bind (m_nLocalPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);   //注意：bind的是本地IP和本地端口，发送的是远端IP和远端端口
	connect (udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));  
}

void Worker::setupSerialCommunication()
{
	//定义一个结构体，用来存放串口各个参数;
	struct PortSettings myComSetting = {BAUD9600,DATA_8,PAR_NONE,STOP_1,FLOW_OFF,500};
	//定义串口对象，并传递参数，在构造函数里对其进行初始化;
#ifdef WIN32
#ifdef COMMUNICATION_SERIAL
	myCom = new Win_QextSerialPort(MAINCOMPORT);
#endif

#elif linux
#ifdef COMMUNICATION_SERIAL
	myCom = new Posix_QextSerialPort(MAINCOMPORT,myComSetting,QextSerialBase::Polling);//LINUX下串口无法使用事件驱动;
#endif
#endif

#ifdef COMMUNICATION_SERIAL
	//以可读写方式打开串口;
	myCom ->open(QIODevice::ReadWrite);	
	myCom->setTimeout(10);
	myCom->setBaudRate(BAUD9600);
	myCom->setDataBits(DATA_8);
	myCom->setStopBits(STOP_1);
	myCom->setParity(PAR_NONE);
	myCom->setFlowControl(FLOW_OFF);
#endif
}
void Worker::setupMCUCommunication()
{
	//定义一个结构体，用来存放串口各个参数;
	struct PortSettings myComSetting = {BAUD9600,DATA_8,PAR_NONE,STOP_1,FLOW_OFF,500};
	//定义串口对象，并传递参数，在构造函数里对其进行初始化;
#ifdef WIN32
#ifdef COMMUNICATION_WITH_MCU
	mcuCom = new Win_QextSerialPort(MCUCOMPORT);
#endif

#elif linux
#ifdef COMMUNICATION_WITH_MCU
	mcuCom = new Posix_QextSerialPort(MCUCOMPORT,myComSetting,QextSerialBase::Polling);
#endif
#endif

#ifdef COMMUNICATION_WITH_MCU
	mcuCom->open(QIODevice::ReadWrite);
	mcuCom->setTimeout(10);
	mcuCom->setBaudRate(BAUD9600);
	mcuCom->setDataBits(DATA_8);
	mcuCom->setStopBits(STOP_1);
	mcuCom->setParity(PAR_NONE);
	mcuCom->setFlowControl(FLOW_OFF);
#endif
}

void Worker::startTimer()
{
	m_pTimer->start(TIMEOUT_PERIOD);
}



