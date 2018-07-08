#ifndef TABLECTRL_H
#define TABLECTRL_H

#include <QTableView>

class TableCtrl : public QTableView
{
	Q_OBJECT

public:
	TableCtrl(QWidget *parent);
	~TableCtrl();

	void initIndex();

protected:
	void keyPressEvent(QKeyEvent *event);
	void focusInEvent(QFocusEvent * event);
	void focusOutEvent(QFocusEvent * event);


private:
	QModelIndex m_currentIndex;

signals:
	void getOutFocus(int dir);//向下失去焦点;//0上//1下//2左//3右;
	
};

#endif // TABLECTRL_H
