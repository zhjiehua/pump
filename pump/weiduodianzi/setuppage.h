#ifndef SETUPPAGE_H
#define SETUPPAGE_H

#include <BasePage.h>
#include "ui_SetupPage.h"


class SetupPage : public CBasePage
{
	Q_OBJECT

public:
	SetupPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~SetupPage();

	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 prevIndex, quint32 add);

protected:
	virtual void initFocusList();
	//bool eventFilter(QObject *o, QEvent *e);

private:
	Ui::SetupPageClass ui;
	

private slots:
	void on_globalBtn_clicked();
	void on_calBtn_clicked();
	void on_permitBtn_clicked();
	void on_timeBtn_clicked();
	void on_msgBtn_clicked();
    void on_netBtn_clicked();
    void on_gridBtn_clicked();
    void on_glpBtn_clicked();
};

#endif // SETUPPAGE_H
