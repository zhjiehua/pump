#ifndef SENSITIVEPAGE_H
#define SENSITIVEPAGE_H

#include "BasePage.h"
#include "ui_sensitivePage.h"

class SensitivePage: public CBasePage
{
	Q_OBJECT
public:
	SensitivePage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~SensitivePage(void);
	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

private:
	Ui::SensitivePageClass ui;

	private slots:
		void gotoSetupPage();
};

#endif // SENSITIVEPAGE_H