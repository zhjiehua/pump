#ifndef MSGPAGE_H
#define MSGPAGE_H

#include <BasePage.h>
#include "ui_MsgPage.h"

class MsgPage : public CBasePage
{
	Q_OBJECT

public:
	MsgPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~MsgPage();
	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	virtual void initFocusList();

private:
	Ui::MsgPageClass ui;
	
private slots:
		void on_updateBtn_clicked();
		void on_saveDataBtn_clicked();
		void on_updateDataBtn_clicked();
};

#endif // MSGPAGE_H
