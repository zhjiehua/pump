#ifndef FLOWCTRL_H
#define FLOWCTRL_H

//代表流速控制，从机器模块中独立出来,这个模块暂时不工作！！！！！！！！！！！！！！！！！！！！！！


#include <QObject>

class FlowCtrl : public QObject
{
	Q_OBJECT

public:
	~FlowCtrl();

	static FlowCtrl *getInstance();

	//百分比更新;
	void updateFlowPercentInPcMode(quint32 percent);				//百分比更新显示(PC模式);
	void updateFlowPercent(quint32 percent);						//梯度模式下的流速更新;

	//不同模式下的流速控制;
	void updateFlowInPcMode(double flow);							//PC模式下的流速控制;
	void updateFlowInGradientMode(double flow);						//单机梯度模式下的流速控制;
	void updateFlowInPumpPurgeMode(double flow);					//单机泵、清洗模式下的流速控制;
	void updateFlowInDebugMode(double flow);						//调试模式下的流速控制;

	void enableUpdateFlow();										//更新流速电机使能;
	void disableUpdateFlow();										//更新流速电机除能;

private:
	FlowCtrl(QObject *parent = 0);
	double m_dFlowPercent;

signals:
	void updatePercentDisplay(QString);						//更新百分比显示信号;
	
};

#endif // FLOWCTRL_H
