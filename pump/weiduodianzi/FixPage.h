#ifndef FIXPAGE_H
#define FIXPAGE_H

#include "BasePage.h"
#include "ui_fixPage.h"
#include "baseMainPage.h"

class FixPage: public CBasePage
{
	Q_OBJECT
public:
	FixPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~FixPage(void);

	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	virtual void initFocusList();

private:
	Ui::FixPageClass ui;

private slots:
	void on_pressCalBtn_clicked();
	void on_flowCalBtn_clicked();

};

#endif // WAVEFIXPAGE_H
