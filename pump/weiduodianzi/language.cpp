#include "language.h"
#include <QDebug>
#include "baseMainPage.h"
#include <QApplication>
#include <QKeyEvent>


LanguagePage::LanguagePage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Language"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = tr("Language");

	ui.globalBtn->setStyleSheet("QPushButton{border-image:url(:/weiduodianzi/ui/global.png);}");
	ui.EnglishBtn->setStyleSheet("QPushButton{border-image:url(:/weiduodianzi/ui/english.png);}""QPushButton:focus{border-image:url(:/weiduodianzi/ui/englishFocus.png);outline: 0px}");
	ui.ChineseBtn->setStyleSheet("QPushButton{border-image:url(:/weiduodianzi/ui/chinese.png);}""QPushButton:focus{border-image:url(:/weiduodianzi/ui/chineseFocus.png);outline: 0px}");
	
	connect(ui.ChineseBtn, SIGNAL(clicked()), this, SLOT(changeLanguageToChinese()) );
	connect(ui.EnglishBtn, SIGNAL(clicked()), this, SLOT(changeLanguageToEnglish()) );
}

LanguagePage::~LanguagePage()
{

}


void LanguagePage::gotoLanguagePage()
{

}

void LanguagePage::changeLanguageToChinese()
{
	g_pMainWindow->changeLanguage(CHINESE);
	ui.chineseLabel->setText(tr("Chinese"));
	ui.englishLabel->setText(tr("English"));
}

void LanguagePage::changeLanguageToEnglish()
{
	g_pMainWindow->changeLanguage(ENGLISH);
	ui.chineseLabel->setText(tr("Chinese"));
	ui.englishLabel->setText(tr("English"));
}

#if 1
bool LanguagePage::eventFilter(QObject *o, QEvent *e)
{
	if(e->type() == QEvent::KeyPress)
	{
		QKeyEvent *ke = static_cast<QKeyEvent *>(e);
		if(ke->key() == Qt::Key_Return)
		{
			if(o == ui.ChineseBtn)
			{
				changeLanguageToChinese();
				return true;
			}
			else if(o == ui.EnglishBtn)
			{
				changeLanguageToEnglish();
				return true;
			}
			else 
				return false;
		}
		else 
			return CBasePage::eventFilter(o, e);

	}
	else 
		return false;
}
#endif

void LanguagePage::initFocusList()
{
	xList.append(ui.ChineseBtn);
	xList.append(ui.EnglishBtn);

	yList.append(ui.ChineseBtn);
	yList.append(ui.EnglishBtn);

	ui.ChineseBtn->installEventFilter(this);
	ui.EnglishBtn->installEventFilter(this);
	//ui.ChineseBtn->setFocus();
}

CBasePage* LanguagePage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new LanguagePage(parent, index, previndex, add);
}


