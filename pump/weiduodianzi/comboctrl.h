#ifndef COMBOCTRL_H
#define COMBOCTRL_H

#include <QComboBox>
#include <QString>

class ComboCtrl : public QComboBox
{
	Q_OBJECT

public:
	ComboCtrl(QWidget *parent);
	~ComboCtrl();

	void setVar(QString name);

protected:
	void keyPressEvent(QKeyEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);

private:
	QString m_varName;

private slots:
	void saveData(int nIndex);	
	void dateChanged(QString var, QString val);
};

#endif // COMBOCTRL_H
