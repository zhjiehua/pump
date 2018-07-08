#ifndef LANGUAGEPAGE_H
#define LANGUAGEPAGE_H

#include <BasePage.h>
#include "ui_LanguagePage.h"

class LanguagePage : public CBasePage
{
	Q_OBJECT

public:
	LanguagePage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~LanguagePage();
	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	virtual void initFocusList();
	bool eventFilter(QObject *o, QEvent *e);

private:
	Ui::LanguagePageClass ui;
	

private slots:
	void gotoLanguagePage();
	void changeLanguageToChinese();
	void changeLanguageToEnglish();
};

#endif // LANGUAGEPAGE_H
