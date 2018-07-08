#ifndef PRESSFIXPAGE_H
#define PRESSFIXPAGE_H

#include <QTimer>
#include "BasePage.h"
#include "ui_pressFixPage.h"

class PressFixPage: public CBasePage
{
	Q_OBJECT
public:
	PressFixPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~PressFixPage(void);
	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	void initFocusList();

private:
	Ui::PressFixPageClass ui;
	bool flag;
	QTimer *pTimer;

	void initDisplay();



private slots:
	void getOutTableFocus(int dir);
	void on_saveBtn_clicked();
	void on_backBtn_clicked();
	void on_startBtn_clicked();
	void on_clearBtn_clicked();

	void updatePresVal(QString disp, quint8 warning);
	void updatePressList();

	void updateFlow();//´¦ÀíÍ¹ÂÖÐÅºÅ;
	
};

#endif // PRESSFIXPAGE_H
