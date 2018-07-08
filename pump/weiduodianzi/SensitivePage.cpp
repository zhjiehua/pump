#include "SensitivePage.h"


SensitivePage::SensitivePage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Sensitive"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = tr("Sensitive");
}


SensitivePage::~SensitivePage(void)
{
}

void SensitivePage::gotoSetupPage()
{

}

CBasePage* SensitivePage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new SensitivePage(parent, index, previndex, add);
}
