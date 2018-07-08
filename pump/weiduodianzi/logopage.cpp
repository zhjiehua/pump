#include "logopage.h"
#include <QDebug>
#include <QPainter>


LogoPage::LogoPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("UV3000U"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = "UV3000U";
}

LogoPage::~LogoPage()
{

}


void LogoPage::gotoSetupPage()
{

}

CBasePage* LogoPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new LogoPage(parent, index, previndex, add);
}



