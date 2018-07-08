/*
	DATE			VERSION		DESCRIPTION								AUTHER
	2016-08-11		V0.90		许可证控件不能保存首位是0的数字			zhjiehua
	2016-08-12		V0.91		修改GLP页面（生产日期、安装日期等）		zhjiehua
	2016-08-17		V0.92		流量校正页面加30min定时					zhjiehua
	2016-08-17		V0.93		压力量程从40MPa改为60MPa；
								电池检测改为每次开机都更新last_time		zhjiehua
	2016-09-12		V0.94		添加4个泵型号；
								修改总吸液量的bug						zhjiehua
	2016-10-17		V0.95		压力量程改回40MPa						zhjiehua
	2016-10-17		V0.96		压力量程改为51MPa						zhjiehua
	2016-11-08		V0.97		执行首行时出现运行purge的流速，而不是上位机发的流速，在设置清洗状态和同步流速到MCU命令间加延时；
								停所有泵按钮会在停止和启动间切换，应该是时间同步命令问题，暂将同步时间命令中的同步流速到MCU命令去掉
																		zhjiehua
	2017-03-17		V0.98		将保存数据和更新数据按钮从信息页面移到管理员页面
																		zhjiehua
	2017-08-15		V0.99		将超级管理员的密码由111111改成173895；
								将Clarity协议的上传频率有50Hz步进改成10Hz步进					
																		zhjiehua
	2017-09-15		V1.00		修复运行时间程序时运行页面的时间显示跳动bug；
								修复500ml以上泵类型不能与上位机软件通讯bug					
																		zhjiehua
	2017-10-16		V1.01		修复掰阀启动工作站时泵停止的bug			zhjiehua
	2017-10-23		V1.02		修复泵的压力值与工作站上不一致的bug		zhjiehua
	2018-01-06		V1.03		修复超级页面保存数据文件不存在的bug		zhjiehua
	2018-02-02		V1.04		修复1000ml泵反控不正常的bug				zhjiehua
	2018-04-09		V1.05		clarity协议制备泵100ml~500ml改成2个小数点精度；
												800ml~3000ml改成1个小数点精度；
																		zhjiehua
*/

#include "baseMainPage.h"
#include <QtGui/QApplication>
#include <QFont>
#include <QTextCodec>
#include "Common.h"
#include "QDesktopWidget"


int g_nActScreenX;
int g_nActScreenY;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QDesktopWidget* desktopWidget = QApplication::desktop();
	//获取可用桌面大小
	//QRect deskRect = desktopWidget->availableGeometry();
	//获取设备屏幕大小
	QRect screenRect = desktopWidget->screenGeometry();
	g_nActScreenX = screenRect.width();
	g_nActScreenY = screenRect.height();

	qDebug() << "m_nActScreenX = " << g_nActScreenX;
	qDebug() << "m_nActScreenY = " << g_nActScreenY;

	QTextCodec::setCodecForTr(QTextCodec::codecForName("gb18030"));
	QFont font;
	if(g_nActScreenX < 420)
		font.setPointSize(8);
	else
	{
#ifdef linux
		font.setPointSize(12);
#else
		font.setPointSize(8);
#endif
	}
	//font.setFamily(("wenquanyi"));
	a.setFont(font);
	BaseMainPage w("LC3000U");
	w.resize(SCREEN_WIDTH, SCREEN_HEIGH);

#ifdef linux
	QApplication::setOverrideCursor(Qt::BlankCursor);//隐藏鼠标
	w.setWindowFlags(Qt::FramelessWindowHint);
	w.showFullScreen ();
#else
	//w.setWindowFlags(Qt::FramelessWindowHint);
	w.show();
#endif
	return a.exec();
}
