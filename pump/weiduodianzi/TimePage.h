#ifndef TIMEPAGE_H
#define TIMEPAGE_H

#include "BasePage.h"
#include "ui_timePage.h"


class TimePage: public CBasePage
{
	Q_OBJECT
public:
	TimePage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~TimePage(void);
	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	virtual void initFocusList();
	bool eventFilter(QObject *o, QEvent *e);

private:
	Ui::TimePageClass ui;
	bool m_bModify;

	void initDisplay();
	void saveDateTime();

private slots:
	void updateTime();

	
	
};

#endif // TIMEPAGE_H
