#ifndef BUGLECOMPENSATION_H
#define BUGLECOMPENSATION_H

#include <QObject>

class BugleCompensation : public QObject
{
	Q_OBJECT

public:
	BugleCompensation(QObject *parent = 0);
	~BugleCompensation();
	static BugleCompensation *getInstance();

	virtual void updateOutput(quint32 output, double flow)=0;				//更新输出;
	virtual void adjustOutputByInput(qint32 inputData)=0;		//根据输入调节输出;
	virtual void stopOutput()=0;								//停止输出;
	virtual void pauseOutput()=0;								//暂停输出;
	virtual void waitOutputStable()=0;							//等待输出稳定;

	virtual void bugleSignal()=0;								//出现了凸轮信号;

protected:
	bool m_bBugleSignal;										//凸轮信号标志;
	bool m_bOutputStable;										//输出稳定标志;
	quint32 m_nCurrentOutput;									//当前输出量;
	virtual bool isOutputStable()=0;							//判断输出是否已经稳定;

signals:
	void outputUpdate(quint32 data);							//更新输出;

private:
	
};

#endif // BUGLECOMPENSATION_H
