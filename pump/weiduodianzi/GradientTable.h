#ifndef GRADIENTTABLE_H
#define GRADIENTTABLE_H

#include "BasePage.h"
#include "ui_GradientTable.h"
#include "baseMainPage.h"

class GradientTable: public CBasePage
{
	Q_OBJECT
public:
	GradientTable(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~GradientTable(void);
	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	void initFocusList();
	bool eventFilter(QObject *obj, QEvent *event);

private:
	Ui::GradientTableClass ui;
	int m_nWhich;//which table;

	void initDisplay();

private slots:
	void getOutTableFocus(int dir);
	void on_saveBtn_clicked();
	void on_backBtn_clicked();

};

#endif // GRADIENTTABLE_H
