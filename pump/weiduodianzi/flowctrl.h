#ifndef FLOWCTRL_H
#define FLOWCTRL_H

//�������ٿ��ƣ��ӻ���ģ���ж�������,���ģ����ʱ��������������������������������������������������


#include <QObject>

class FlowCtrl : public QObject
{
	Q_OBJECT

public:
	~FlowCtrl();

	static FlowCtrl *getInstance();

	//�ٷֱȸ���;
	void updateFlowPercentInPcMode(quint32 percent);				//�ٷֱȸ�����ʾ(PCģʽ);
	void updateFlowPercent(quint32 percent);						//�ݶ�ģʽ�µ����ٸ���;

	//��ͬģʽ�µ����ٿ���;
	void updateFlowInPcMode(double flow);							//PCģʽ�µ����ٿ���;
	void updateFlowInGradientMode(double flow);						//�����ݶ�ģʽ�µ����ٿ���;
	void updateFlowInPumpPurgeMode(double flow);					//�����á���ϴģʽ�µ����ٿ���;
	void updateFlowInDebugMode(double flow);						//����ģʽ�µ����ٿ���;

	void enableUpdateFlow();										//�������ٵ��ʹ��;
	void disableUpdateFlow();										//�������ٵ������;

private:
	FlowCtrl(QObject *parent = 0);
	double m_dFlowPercent;

signals:
	void updatePercentDisplay(QString);						//���°ٷֱ���ʾ�ź�;
	
};

#endif // FLOWCTRL_H
