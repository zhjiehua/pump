#include "FixPage.h"
#include <QKeyEvent>
#include "baseMainPage.h"

FixPage::FixPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Calibration"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = tr("Calibration");
	setStyleSheet( BTNGLOBALSTYLE);
}


FixPage::~FixPage(void)
{
}



void FixPage::initFocusList()
{
	xList.append(ui.pressCalBtn);
	xList.append(ui.flowCalBtn);

	yList.append(ui.pressCalBtn);
	yList.append(ui.flowCalBtn);

	//ui.pressCalBtn->setFocus();

	MachineStat::getInstance()->pwdNeed = true;
}


CBasePage* FixPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new FixPage(parent, index, previndex, add);
}

void FixPage::on_pressCalBtn_clicked()
{
	//MachineStat::getInstance()->whichPage = WAVEFIXPAGE_INDEX;
	//MachineStat::getInstance()->usrType = MachineStat::USER;
	//g_pMainWindow->changePage(PWDPAGE_INDEX);

	g_pMainWindow->changePage(WAVEFIXPAGE_INDEX);
}

void FixPage::on_flowCalBtn_clicked()
{
	//MachineStat::getInstance()->whichPage = FLOWFIXPAGE_INDEX;
	//MachineStat::getInstance()->usrType = MachineStat::USER;
	//g_pMainWindow->changePage(PWDPAGE_INDEX);

	g_pMainWindow->changePage(FLOWFIXPAGE_INDEX);
}
