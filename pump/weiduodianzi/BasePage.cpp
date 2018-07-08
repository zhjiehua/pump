#include "BasePage.h"
#include "editctrl.h"
#include "tablectrl.h"
#include <QGridLayout>
#include "baseMainPage.h"


CBasePage::CBasePage(QWidget *parent /*= 0 */)
	:QWidget(parent)
	,m_pCurrentPage(0)
{

}

CBasePage::CBasePage(QString strTitle, quint8 nPageIndex, quint8 nPrevPageIndex, quint32 add, QWidget *parent/* = 0*/)
	: QWidget(parent),
	m_strTitle( strTitle ),
	m_nPageIndex( nPageIndex ),
	m_nPrevPageIndex(nPrevPageIndex),
	m_nAdd(add),
	m_pCurrentPage(0)
{
	
}




CBasePage::~CBasePage()
{

}

void CBasePage::initFocus()
{
	initFocusList();
	filterEventForChildren();
}

void CBasePage::filterEventForChildren()
{
	/*QObjectList list = children();
	foreach(QObject *obj, list)
	{
	QWidget *widget = qobject_cast<QWidget*>(obj);
	if(widget)
	widget->installEventFilter(this);
	}*/
}

bool CBasePage::eventFilter(QObject *obj, QEvent *event)
{
	if(event->type() == QEvent::KeyPress)
	{
		QKeyEvent *ke = static_cast<QKeyEvent *>(event);
		if(ke->key() == Qt::Key_Backspace)
		{
			QWidget *ctrl = qobject_cast<QWidget *>(obj);
			if(ctrl)
			{
				EditCtrl *edit = qobject_cast<EditCtrl *>(obj);
				TableCtrl *tblCtrl = qobject_cast<TableCtrl *>(obj);
				if(edit)
				{
					if(!edit->isEditMode())
					{
						g_pMainWindow->backToPage();
						return true;
					}
					else
						return false;
				}
				else
				{
					if(tblCtrl)
						return false;

					g_pMainWindow->backToPage();
					return true;
				}
			}
			return false;
		}
		else
			return false;
	}
	else
		return false;
}

CBasePage* CBasePage::changePage(int index, quint32 addData /*= 0*/)
{
	if(index >=0 & index <pageMethod.size())
	{
		const StructPageMethod &method = pageMethod.at(index);
		CBasePage *pPage = method.getInstance(this, index, method.nPrevIndex, addData);
		if(m_pCurrentPage)
			delete m_pCurrentPage;
		m_pCurrentPage = pPage;
		QLayout *oldLayout = this->layout();
		if(oldLayout)
		{
			delete oldLayout;
		}
		QGridLayout *pLayout = new QGridLayout(this);
		pLayout->addWidget(pPage);
		m_pCurrentPage->show();
		m_pCurrentPage->initFocus();
		return m_pCurrentPage;
	}
	else
		return NULL;
}

void CBasePage::addPage(getPageInstance pageInstanceMethod, quint8 prevIndex)
{
	StructPageMethod method;
	method.getInstance = pageInstanceMethod;
	method.nPrevIndex = prevIndex;
	pageMethod.append(method);
}


