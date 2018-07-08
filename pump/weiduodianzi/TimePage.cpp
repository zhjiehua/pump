#include "TimePage.h"
#include <QDate>
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#include <QEvent>
#include <baseMainPage.h>
#ifdef linux
#include <stdlib.h>
#endif

//#define TIME_UNIT_BASE 1//单位是s，测试用;
#define TIME_UNIT_BASE 3600//单位小时，真正用;


TimePage::TimePage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Time"), index, previndex, add, parent )
	, m_bModify(false)
{
	ui.setupUi(this);
	m_strTitle = tr("Time");
	initDisplay();
}


TimePage::~TimePage(void)
{
}


void TimePage::initFocusList()
{
	xList.append(ui.yearEdit);
	xList.append(ui.monthEdit);
	xList.append(ui.dayEdit);
	xList.append(ui.hourEdit);
	xList.append(ui.minEdit);
	xList.append(ui.secEdit);

	yList.append(ui.yearEdit);
	yList.append(ui.hourEdit);
	yList.append(ui.monthEdit);
	yList.append(ui.minEdit);
	yList.append(ui.dayEdit);
	yList.append(ui.secEdit);
	//ui.yearEdit->setFocus();

}

bool TimePage::eventFilter(QObject *o, QEvent *e)
{
	if(e->type() == QEvent::KeyPress)
	{
		QKeyEvent *ke = static_cast<QKeyEvent *>(e);
		if(ke->key() == Qt::Key_Return)
		{
			if(o == ui.yearEdit ||
				o == ui.monthEdit ||
				o ==  ui.dayEdit ||
				o == ui.hourEdit ||
				o == ui.minEdit ||
				o ==  ui.secEdit)
			{
				EditCtrl *pEdit = qobject_cast<EditCtrl *>(o);
				//if(pEdit->isEditMode())
				if(m_bModify)
				{	
					m_bModify = false;
					//saveDateTime();
#if 1//屏蔽，不修改RTC时钟;
					QString temp = QString("%1.").arg(ui.yearEdit->text().toInt(), 4, 10, QChar('0'));
					temp += QString("%1.").arg(ui.monthEdit->text().toInt(), 2, 10, QChar('0'));
					temp += QString("%1-").arg(ui.dayEdit->text().toInt(), 2, 10, QChar('0'));
					temp += QString("%1:").arg(ui.hourEdit->text().toInt(), 2, 10, QChar('0'));
					temp += QString("%1:").arg(ui.minEdit->text().toInt(), 2, 10, QChar('0'));
					temp += QString("%1").arg(ui.secEdit->text().toInt(), 2, 10, QChar('0'));
					qDebug()<<"saving datetime:"<<temp;
					QString cmd = QString("date -s ");
					cmd +=temp;
#ifdef linux
					system(cmd.toLatin1().data());
					system("hwclock -w");

					time_t now = time(NULL);
					DataBase::getInstance()->updateDate("last_time", QString::number(now));
#endif
#endif
					return false;
				}
				else
				{
					m_bModify = true;
					
					return false;
				}
			}
		}
		else if(ke->key() == Qt::Key_Backspace)
		{
			if(m_bModify)
				m_bModify = false;
			return CBasePage::eventFilter(o,e);
		}
		else
			return false;
	}
	else
		return false;
}

void TimePage::updateTime()
{
	if(m_bModify)
		return;

	QDateTime ttime = QDateTime::currentDateTime();//获取系统现在的时间;
	int offset = DataBase::getInstance()->queryData("datetimeoffset").toInt();
	uint temp = ttime.toTime_t();
	temp +=offset;
	QDateTime time = QDateTime::fromTime_t( temp );
	QDate date = ttime.date();
	QTime realTime = ttime.time();
	ui.yearEdit->setText(QString::number(date.year()));
	ui.monthEdit->setText(QString::number(date.month()));
	ui.dayEdit->setText(QString::number(date.day()));
	ui.hourEdit->setText(QString::number(realTime.hour()));
	ui.minEdit->setText(QString::number(realTime.minute()));
	ui.secEdit->setText(QString::number(realTime.second()));
}

CBasePage* TimePage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new TimePage(parent, index, previndex, add);
}



void TimePage::initDisplay()
{
	ui.yearEdit->installEventFilter(this);
	ui.dayEdit->installEventFilter(this);
	ui.monthEdit->installEventFilter(this);
	ui.hourEdit->installEventFilter(this);
	ui.minEdit->installEventFilter(this);
	ui.secEdit->installEventFilter(this);

	ui.yearEdit->setAlignment(Qt::AlignCenter);
	ui.monthEdit->setAlignment(Qt::AlignCenter);
	ui.dayEdit->setAlignment(Qt::AlignCenter);
	ui.hourEdit->setAlignment(Qt::AlignCenter);
	ui.minEdit->setAlignment(Qt::AlignCenter);
	ui.secEdit->setAlignment(Qt::AlignCenter);


	ui.yearEdit->setValRange(QString("year"),2015,2050,0);
	ui.monthEdit->setValRange(QString("month"),1,12,0);
	ui.dayEdit->setValRange(QString("day"),1,31,0);

	ui.hourEdit->setValRange(QString("hour"), 0, 23, 0);
	ui.minEdit->setValRange(QString("minute"), 0, 59, 0);
	ui.secEdit->setValRange(QString("second"), 0, 59, 0);

	updateTime();

	//定时更新时间;
	QTimer *pTimer = new QTimer(this);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(updateTime()));
	pTimer->start(1000);
}

void TimePage::saveDateTime()
{
	int year = ui.yearEdit->text().toInt();
	int month = ui.monthEdit->text().toInt();
	int day = ui.dayEdit->text().toInt();
	int hour = ui.hourEdit->text().toInt();
	int min = ui.minEdit->text().toInt();
	int sec = ui.secEdit->text().toInt();

	QDate date(year, month, day);
	QTime time(hour, min, sec);
	QDateTime dt(date, time);
	
	uint newDateTime = dt.toTime_t();
	uint currentDateTime = QDateTime::currentDateTime().toTime_t();
	int offset = newDateTime - currentDateTime;

	DataBase::getInstance()->updateDate("datetimeoffset", QString::number(offset) );
}