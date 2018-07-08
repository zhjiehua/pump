#ifndef GLPINFOPAGE_H
#define GLPINFOPAGE_H

#include <BasePage.h>
#include "ui_glpinfopage.h"

class GlpInfoPage : public CBasePage
{
	Q_OBJECT

public:
	GlpInfoPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~GlpInfoPage();
	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

	bool eventFilter(QObject *obj, QEvent *event);

protected:
	virtual void initFocusList();

private:
	Ui::GlpInfoPageClass ui;
	
private slots:
    void updateBugleDisp(quint32 cnt);//更新栓塞次数
	void pumpChange(int);
	void updateUsedTime();
};

#endif // GLPINFOPAGE_H
