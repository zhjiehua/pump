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

	virtual void updateOutput(quint32 output, double flow)=0;				//�������;
	virtual void adjustOutputByInput(qint32 inputData)=0;		//��������������;
	virtual void stopOutput()=0;								//ֹͣ���;
	virtual void pauseOutput()=0;								//��ͣ���;
	virtual void waitOutputStable()=0;							//�ȴ�����ȶ�;

	virtual void bugleSignal()=0;								//������͹���ź�;

protected:
	bool m_bBugleSignal;										//͹���źű�־;
	bool m_bOutputStable;										//����ȶ���־;
	quint32 m_nCurrentOutput;									//��ǰ�����;
	virtual bool isOutputStable()=0;							//�ж�����Ƿ��Ѿ��ȶ�;

signals:
	void outputUpdate(quint32 data);							//�������;

private:
	
};

#endif // BUGLECOMPENSATION_H
