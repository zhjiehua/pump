#include "timehelper.h"
#include <QTime>
#include <QTime>


TimeHelper::TimeHelper(QObject *parent)
	: QObject(parent)
{

}

TimeHelper::~TimeHelper()
{

}

void TimeHelper::mDelay(const quint32 mSec)
{
	QTime n=QTime::currentTime();
	QTime now;
	do{
		now=QTime::currentTime();
	}   while (n.msecsTo(now)<=mSec);
}

QSemaphore* TimeHelper::getComSem()
{
	static QSemaphore *comSemaphore = NULL;
	if(!comSemaphore)
	{
		comSemaphore = new QSemaphore(1);
	}
	return comSemaphore;
}

quint32 TimeHelper::getRandom()
{
	QDateTime currentTime = QDateTime::currentDateTime();
	qsrand(currentTime.time().msec());
	int randNum = qrand()%99999999;
	return randNum;
}
