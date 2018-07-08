#ifndef SETUPLISTPAGE_H
#define SETUPLISTPAGE_H

#include "BasePage.h"
#include "ui_DebugMcuProtoPage.h"
#include "baseMainPage.h"
#include "mytablemodel.h"
#include <QTimer>
#include <QTime>
//#include <qwt_plot_curve.h>


class DebugMcuProtoPage: public CBasePage
{
	Q_OBJECT
public:
	DebugMcuProtoPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~DebugMcuProtoPage(void);

	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	void initFocusList();

private:
	Ui::DebugMcuProtoPageClass ui;
	bool m_bStartFlag;
	bool m_bCollectFlag;
	int m_nBulgeFlag;
	MyTableModel *gradient_model;
	QTime *m_pTime;
	double m_dPeriod;
	//QwtPlotCurve m_curve;//����
	QVector<QPointF>m_curveData;//��������;

	void initDisplay();
	int getBulge();

	//�������;
	void addDataToCurve(double x, double y);	//�����ݼ�������;
	void initPlotDisp();				//����ͼ��ʼ��;

private slots:
	void on_startBtn_clicked();
	void on_collectBtn_clicked();
	void on_clearBtn_clicked();
	void on_bugleBtn_clicked();
	void getOutTableFocus(int dir);
	void updatePressVal(QString, quint8);
	void dealBulge();//����͹���ź�;

	//ͼ�����;
	void on_plotBtn_clicked();
};

#endif // SETUPLISTPAGE_H