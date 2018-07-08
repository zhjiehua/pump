#ifndef IOMODULE_H
#define IOMODULE_H

#include <QObject>
#include <QTimer>

#ifdef linux
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <asm-generic/ioctl.h>
#include <fcntl.h>

//IOCTL的命令
#define IOC_MAGIC  'A'
#define PWM_IOCTL_WHICH_EDGE	_IO(IOC_MAGIC, 10)//设置是检测下降沿还是上升沿;
#define PWM_IOCTL_SET_FALLING	_IO(IOC_MAGIC, 9)//设置下降沿;
#define PWM_IOCTL_GET_EDG		_IO(IOC_MAGIC, 8)
#define PWM_IOCTL_GET_IO		_IO(IOC_MAGIC, 7)
#define PWM_IOCTL_CLS_IO		_IO(IOC_MAGIC, 6)
#define PWM_IOCTL_SET_IO		_IO(IOC_MAGIC, 5)
#define PWM_IOCTL_GET_STA		_IO(IOC_MAGIC, 4)
#define PWM_IOCTL_SET_CNT		_IO(IOC_MAGIC, 3)
#define PWM_IOCTL_SET_FREQ		_IO(IOC_MAGIC, 2)
#define PWM_IOCTL_START			_IO(IOC_MAGIC, 1)
#define PWM_IOCTL_STOP			_IO(IOC_MAGIC, 0)
#endif

//IO掩码
#define IO_PUL_MASK				((unsigned long)1<<0)
#define IO_DIR_MASK				((unsigned long)1<<1)
#define IO_RST_MASK				((unsigned long)1<<2)
#define IO_COIL_MASK			((unsigned long)1<<3)
#define IO_CTL0_MASK			((unsigned long)1<<4)
#define IO_CTL1_MASK			((unsigned long)1<<5)

#define IO_DIN1_MASK			((unsigned long)1<<6)
#define IO_DIN2_MASK			((unsigned long)1<<7)
#define IO_DIN3_MASK			((unsigned long)1<<8)
#define IO_DIN4_MASK			((unsigned long)1<<9)
#define IO_DIN5_MASK			((unsigned long)1<<10)
#define IO_DIN6_MASK			((unsigned long)1<<11)

#define IO_MOTORSTOP_MASK		((unsigned long)1<<12)
#define IO_BULGE_MASK			((unsigned long)1<<13)

//凸轮补偿用;
#define IO_BUGLE_COMPEN_MASK	((unsigned long)1<<14)
#define IO_BUGLE_COMPRESV_MASK	((unsigned long)1<<15)
#define IO_BUGLE_COMPLVL_MASK	((unsigned long)1<<16)


class IoModule : public QObject
{
	Q_OBJECT

public:
	~IoModule();
	static IoModule *getInstance();

	void logic_set_io(quint32 io, bool val);
	bool logic_get_io(quint32 io);

//检测器用
	void logic_init_lamp_operation();

	void doWarn(bool on);//蜂鸣器响3s警告;

private:
	QTimer *m_pTimer;
	QTimer *m_pReadIoTimer;


	IoModule(QObject *parent = 0);
	void initIO();

signals:
	void initLampSuccess();
	void bulge();//凸轮信号出现;
	void weeping(bool flag);//漏液
	void testPress(quint32);

private slots:
	void init_s1();//CTL1输出0电平、CTL0输出1电平
	void init_s2();//CTL0输出0电平;
	void init_s3();//CTL1输出1电平，此时氘灯应点亮;
	void init_s4();//DIN4为0电平则氘灯正常；如为1电平则氘灯错误
	void operation_s();//操作模式;

	void readingIO();
	
};

#endif // IOMODULE_H
