#ifndef GRADIENTPAGE_H
#define GRADIENTPAGE_H

#include "BasePage.h"
#include "ui_GradientPage.h"
#include "baseMainPage.h"

class GradientPage: public CBasePage
{
	Q_OBJECT
public:
	GradientPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~GradientPage(void);
	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	void initFocusList();

private:
	Ui::GradientPageClass ui;

private slots:
	void on_RGBtn_clicked();
	void on_RGBtn2_clicked();
	void on_RGBtn3_clicked();
	void on_RGBtn4_clicked();
	void on_RGBtn5_clicked();
	void on_RGBtn6_clicked();
	void on_RGBtn7_clicked();
	void on_RGBtn8_clicked();
	void on_RGBtn9_clicked();
	void on_RGBtn10_clicked();
	void on_FGBtn_clicked();
	void on_CGBtn_clicked();

};

#endif // GRADIENTPAGE_H
