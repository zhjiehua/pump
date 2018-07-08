#include "comboctrl.h"
#include <QKeyEvent>
#include <QModelIndex>
#include <QDebug>
#include <QListView>
#include "baseMainPage.h"
#include "database.h"

//#define COMBOSTYLE "QComboBox::drop-down{width:20px;height:20px;border-image:url(:/weiduodianzi/ui/down-arrow.png);}"\
//	"QComboBox::drop-down:focus{width:15px;height:15px;border-image:url(:/weiduodianzi/ui/down-arrow.png);}"\
//	"QComboBox:focus{border:3px solid rgb(155,200,33);}"

#define COMBOSTYLE "QComboBox::drop-down{width:20px;height:20px;border-image:url(:/weiduodianzi/ui/down-arrow.png);}"\
	"QComboBox::drop-down:focus{width:15px;height:15px;border-image:url(:/weiduodianzi/ui/down-arrow.png);}"\
	"QComboBox:focus{border: 2px solid blue;outline: 5px;}"//ÕÅ½Ü»ªÐÞ¸Ä@2016-06-23


ComboCtrl::ComboCtrl(QWidget *parent)
	: QComboBox(parent)
{
	setStyleSheet(COMBOSTYLE);
	view()->installEventFilter(this);
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(saveData(int)));
}

ComboCtrl::~ComboCtrl()
{

}


void ComboCtrl::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == KEY_RETURN)
	{
		showPopup();
	}
	else
		return QComboBox::keyPressEvent(event);
}

bool ComboCtrl::eventFilter(QObject *obj, QEvent *event)
{
	if(event->type() == QEvent::KeyPress)
	{
		QKeyEvent *ke = static_cast<QKeyEvent *>(event);
		if(ke->key() == KEY_UP && obj == view())
		{
			QModelIndex index = view()->selectionModel()->currentIndex();
			int row = index.row();
			if(row == 0)
				row = count()-1;
			else
				row--;
			QModelIndex indexA = model()->index(row, 0, QModelIndex());
			view()->setCurrentIndex(indexA);
			return true;
		}
		else if(ke->key() == KEY_DOWN && obj == view())
		{
			QModelIndex index = view()->selectionModel()->currentIndex();
			int row = index.row();
			row = ++row%count();
			QModelIndex indexA = model()->index(row, 0, QModelIndex());
			view()->setCurrentIndex(indexA);
			return true;
		}
		else 
			return false;
	}
	else
		return false;
}

void ComboCtrl::setVar( QString name )
{
	m_varName = name;
	DataBase *pDb = DataBase::getInstance();
	int nIndex = pDb->queryData(m_varName).toInt();
	setCurrentIndex(nIndex);
	connect(pDb, SIGNAL(dateChanged(QString, QString)), this, SLOT(dateChanged(QString, QString)));
}

void ComboCtrl::saveData(int nIndex)
{
	QString temp = QString::number(nIndex);
	DataBase::getInstance()->updateDate(m_varName, temp);
}

void ComboCtrl::dateChanged( QString var, QString val )
{
	if(var == m_varName)
	{
		int nIndex = DataBase::getInstance()->queryData(m_varName).toInt();
		setCurrentIndex(nIndex);
	}
}
