#include "pwdpage.h"
#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>
#include "baseMainPage.h"
#include "Common.h"



PwdPage::PwdPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Password"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = tr("Password");
	ui.loginBtn->setStyleSheet(BTNGLOBALSTYLE);
}

PwdPage::~PwdPage()
{

}

void PwdPage::initFocusList()
{
	if(MachineStat::getInstance()->usrType == MachineStat::ADMIN)
		ui.usrEdit->setText("Admin");
	else
		ui.usrEdit->setText("User");
	ui.usrEdit->setEnabled(false);
	ui.loginBtn->installEventFilter(this);
	ui.pwdEdit->installEventFilter(this);
	ui.pwdEdit->setEchoMode(QLineEdit::Password);
	ui.pwdEdit->setValRange(QString(""),0,999999, 0);
	ui.pwdEdit->clear();
	//ui.pwdEdit->setFocus();

	xList.append(ui.pwdEdit);
	xList.append(ui.loginBtn);

	yList.append(ui.pwdEdit);
	yList.append(ui.loginBtn);
	
}

void PwdPage::loginAdmin()
{
	qDebug()<<"fff";
}

bool PwdPage::eventFilter(QObject *obj, QEvent *event)
{
	if(obj == ui.loginBtn)
	{
		if(event->type() == QEvent::KeyPress)
		{
			QKeyEvent *ke = static_cast<QKeyEvent *>(event);
			if(ke->key() == Qt::Key_Return)
			{
				QString pwd = ui.pwdEdit->text();
				QString p;

				if(MachineStat::getInstance()->usrType == MachineStat::ADMIN)
					p = DataBase::getInstance()->queryData("pwd");
				else
					p = DataBase::getInstance()->queryData("usrpwd");

				if(pwd == p)
				{
					MachineStat::getInstance()->pwdOK = true;
					g_pMainWindow->changePage(MachineStat::getInstance()->whichPage);
				}
				else
				{
					QMessageBox::warning(g_pMainWindow, tr("Warning"), tr("Pwd error!!!"));
					ui.pwdEdit->clear();
				}
				return true;
			}
			else 
				return CBasePage::eventFilter(obj, event);
		}
		else
			return CBasePage::eventFilter(obj, event);
	}
	else
		return CBasePage::eventFilter(obj, event);
}

CBasePage* PwdPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new PwdPage(parent, index, previndex, add);
}

