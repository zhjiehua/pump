#ifndef EDITCTRL_H
#define EDITCTRL_H

#include <QLineEdit>

class EditCtrl : public QLineEdit
{
	Q_OBJECT

public:
	EditCtrl(QWidget *parent);
	~EditCtrl();

	//�������ݿ�������ƣ������������Ч��Χ����Сֵ�����ֵ��С����λ��
	void setValRange(QString var, double min, double max, quint8 nDecimal = 0, int nWidth= -1, bool bReadOnly = false);
	bool isEditMode(){return !isReadOnly();}
	void disableAutoSave(){m_bAutoSave = false;}
	void restoreData();						//�ָ�֮ǰ������;
	void saveData(QString data);
	void setTextMode(bool mode = true);		//�����ı�����ģʽ������������ֵ��Χ���������ơ�


protected:
	void keyPressEvent(QKeyEvent *event);

private:
	bool m_bAutoSave;//�Զ�����;
	double m_minVal;//��Сֵ,Ĭ��0;
	double m_maxVal;//���ֵ��Ĭ��100;
	quint8 m_nDecimal;//С����λ����Ĭ��0;
	bool   m_bReadOnly;//ֻ������������;
	int  m_nWidth;//����λ����,������λ����;
	QString tempData;//��ʱ����;
	QString m_varName;//��������;
	bool m_bTextMode;//�ı�����ģʽ;

	int len;//�������ݵĳ���;
	int bei;//����;
	QStringList data;//�������ݵĻ���;

	void checkValidity(QString val);//����������ݵ���Ч��;

private slots:
	void dateChanged(QString var, QString val);

signals:
	void dataChanging(QString currentData);
	
};

#endif // EDITCTRL_H
