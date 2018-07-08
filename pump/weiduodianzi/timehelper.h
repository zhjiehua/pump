#ifndef TIMEHELPER_H
#define TIMEHELPER_H

#include <QObject>
#include <QSemaphore>

class TimeHelper : public QObject
{
	Q_OBJECT

public:
	TimeHelper(QObject *parent = 0);
	~TimeHelper();

	static void mDelay(const quint32 mSec);
	static QSemaphore* getComSem();

	static quint32 getRandom();					//获取随机数;
};

#endif // TIMEHELPER_H
