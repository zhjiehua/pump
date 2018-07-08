#ifndef EDITCTRL_H
#define EDITCTRL_H

#include <QLineEdit>

class EditCtrl : public QLineEdit
{
	Q_OBJECT

public:
	EditCtrl(QWidget *parent);
	~EditCtrl();

	//设置数据库变量名称，可以输入的有效范围，最小值、最大值和小数点位数
	void setValRange(QString var, double min, double max, quint8 nDecimal = 0, int nWidth= -1, bool bReadOnly = false);
	bool isEditMode(){return !isReadOnly();}
	void disableAutoSave(){m_bAutoSave = false;}
	void restoreData();						//恢复之前的数据;
	void saveData(QString data);
	void setTextMode(bool mode = true);		//进入文本输入模式，即不进行数值范围的输入限制。


protected:
	void keyPressEvent(QKeyEvent *event);

private:
	bool m_bAutoSave;//自动保存;
	double m_minVal;//最小值,默认0;
	double m_maxVal;//最大值，默认100;
	quint8 m_nDecimal;//小数点位数，默认0;
	bool   m_bReadOnly;//只读，不作输入;
	int  m_nWidth;//多少位对齐,不够高位补零;
	QString tempData;//临时数据;
	QString m_varName;//变量名称;
	bool m_bTextMode;//文本输入模式;

	int len;//输入内容的长度;
	int bei;//倍数;
	QStringList data;//输入数据的缓存;

	void checkValidity(QString val);//检查输入数据的有效性;

private slots:
	void dateChanged(QString var, QString val);

signals:
	void dataChanging(QString currentData);
	
};

#endif // EDITCTRL_H
