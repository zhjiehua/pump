#ifndef LINEUINT_H
#define LINEUINT_H

#include <QObject>

//���Ե�Ԫ
class LineUint : public QObject
{
	Q_OBJECT

public:
	LineUint(QObject *parent=0);
	explicit LineUint(const LineUint &line);
	~LineUint();

	void setRange(double x1, double y1, double x2, double y2, QString disp = QString("") );
	bool inStep(double x) const;
	bool inRange(double x) const;
	void printOut() const;
	double getStepValueByX(double x)const;
	double getValueByXAndK(double x)const;
	QString getFlowStr()const;

	//�ݶ�ʱ���õ��ٷֱ�;
	quint32 getPercentVal()const;
	void setPercentVal(quint32 per);

private:
	double x1;
	double y1;
	double x2;
	double y2;
	double k;//б��;
	QString str;
	quint32 percent;//�ٷֱȣ��ݶ�ʱ����;
	
};

#endif // LINEUINT_H
