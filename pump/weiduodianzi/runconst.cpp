#include "runconst.h"
#include <QDebug>

RunConstPage::RunConstPage(QWidget *parent /*= 0*/)
	: CBasePage(tr("Debug IO"), ADMINPAGE_INDEX, 0,0, parent)
{
	ui.setupUi(this);
	m_strTitle = tr("Debug IO");
}

RunConstPage::~RunConstPage()
{

}


void RunConstPage::gotoRunConstPage()
{

}

