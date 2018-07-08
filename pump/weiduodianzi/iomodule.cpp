#include "iomodule.h"
#include <QStateMachine>
#include <QSignalTransition>
#include <QDebug>

#define ENABLE_AD_PRESS_SIMULATION	0
#define ENABLE_IO_BUGLE_SIMULATION	0


#ifdef linux
#include <sys/ioctl.h>
#include "qkbdmy_qws.h"
static int fd = -1;
static unsigned long temp_read = 0;
#endif




IoModule::IoModule(QObject *parent)
	: QObject(parent)
{
	initIO();
	//logic_init_lamp_operation();
}

IoModule::~IoModule()
{

}

IoModule * IoModule::getInstance()
{
	static IoModule *pIo = new IoModule();
	return pIo;
}


void IoModule::initIO()
{
#ifdef linux
	fd = open("/dev/pwm", 0);
	if (fd < 0) {
		perror("open pwm device");
		exit(1);
	}
	ioctl(fd, PWM_IOCTL_WHICH_EDGE, 0);//张杰华添加@2016-06-15，0表示下降沿检测
#endif

	m_pReadIoTimer = new QTimer(this);
	m_pReadIoTimer->start(100);
	connect(m_pReadIoTimer, SIGNAL(timeout()), this, SLOT(readingIO()) );

}


void IoModule::logic_set_io(quint32 io, bool val)
{
#ifdef linux
	if(val)
		ioctl(fd, PWM_IOCTL_SET_IO, io);
	else
		ioctl(fd, PWM_IOCTL_CLS_IO, io);
#endif
}


bool IoModule::logic_get_io(quint32 io)
{
	unsigned long ret = 1;
#ifdef linux
	temp_read = io;
	ioctl(fd, PWM_IOCTL_GET_IO, (unsigned long*)(&temp_read));
	ret = temp_read&io;
#endif
	if(ret)
		return true;
	else
		return false;
}

void IoModule::logic_init_lamp_operation()
{
	QStateMachine *pMachine = new QStateMachine(this);
	m_pTimer = new QTimer(this);
	m_pTimer->setSingleShot(true);
	QState *pInitS = new QState();
	QState *pInitS1 = new QState(pInitS);
	QState *pInitS2 = new QState(pInitS);
	QState *pInitS3 = new QState(pInitS);
	QState *pInitS4 = new QState(pInitS);
	pInitS->setInitialState(pInitS1);
	pInitS1->addTransition(m_pTimer, SIGNAL(timeout()), pInitS2);
	pInitS2->addTransition(m_pTimer, SIGNAL(timeout()), pInitS3);
	pInitS3->addTransition(m_pTimer, SIGNAL(timeout()), pInitS4);

	QState *pOperationS = new QState();
	pInitS->addTransition(this, SIGNAL(initLampSuccess()),pOperationS);
	pMachine->addState(pInitS);//初始化状态;
	pMachine->addState(pOperationS);//操作状态;
	pMachine->setInitialState(pInitS);

	connect(pInitS1, SIGNAL(entered()), this, SLOT(init_s1()));
	connect(pInitS2, SIGNAL(entered()), this, SLOT(init_s2()));
	connect(pInitS3, SIGNAL(entered()), this, SLOT(init_s3()));
	connect(pInitS4, SIGNAL(entered()), this, SLOT(init_s4()));
	connect(pOperationS, SIGNAL(entered()), this, SLOT(operation_s()));

	pMachine->start();

}


void IoModule::doWarn(bool on)
{
#ifdef linux
	MyKeyHandler::turnOnWarning(on);
#endif
}

void IoModule::init_s1()
{
	qDebug()<<"s11";
	m_pTimer->start(5000);
}

void IoModule::init_s2()
{
	qDebug()<<"s12";
	m_pTimer->start(1000);
}

void IoModule::init_s3()
{
	qDebug()<<"s13";
	m_pTimer->start(1000);
}

void IoModule::init_s4()
{
	qDebug()<<"s14";
	emit(initLampSuccess());
}

void IoModule::operation_s()
{
	qDebug()<<"operation mode";
}

void IoModule::readingIO()
{
	unsigned long ret = 0;
	unsigned long weepRet = 0;
	static unsigned long last_weepRet = 0;
#ifdef linux
	temp_read = IO_BULGE_MASK;
	ioctl(fd, PWM_IOCTL_GET_EDG, (unsigned long*)(&temp_read));
	ret = temp_read&IO_BULGE_MASK;

#else
#if ENABLE_IO_BUGLE_SIMULATION//模拟凸轮信号
	static quint32 cnt = 0;
	if(++cnt == 3)
	{
		ret = 1;
		cnt = 0;
	}
#endif
#endif
	if(ret)
	{
		emit(bulge());
	}
	
#if ENABLE_AD_PRESS_SIMULATION//用于模拟读取到AD信号,同时可以测试开始停止是否生效;
	//quint32 temp = qrand()%1000;
	quint32 temp = 100;
	emit(testPress(temp));
#endif

	//漏液检测
#ifdef linux
	temp_read = IO_DIN5_MASK;
	ioctl(fd, PWM_IOCTL_GET_IO, (unsigned long*)(&temp_read));
	weepRet = temp_read&IO_DIN5_MASK;
#else
	weepRet = 0;
#endif
	
	if(last_weepRet != weepRet)
	{
		last_weepRet = weepRet;
		emit(weeping(!!weepRet));
	}

}
