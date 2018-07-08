#include "editctrl.h"
#include <QDebug>
#include "KeyboardDialog.h"
#include "baseMainPage.h"
#include <QKeyEvent>
#include <math.h>
#include "database.h"

EditCtrl::EditCtrl(QWidget *parent)
	: QLineEdit(parent)
	, m_minVal(0)
	, m_maxVal(100)
	, m_nDecimal(0)
	, m_bReadOnly(false)
	, m_nWidth(-1)
	, m_bTextMode(false)
{
	//setStyleSheet("QLineEdit:focus{border:2px solid rgb(155,200,33);}");
	setStyleSheet("QLineEdit:focus{border: 2px solid blue;outline: 5px;}");//张杰华修改@2016-06-23
	setReadOnly(true);
}

EditCtrl::~EditCtrl()
{

}


void EditCtrl::setValRange(QString var, double dMin, double dMax, quint8 nDecimal/* = 0*/, int nWidth/* = -1*/, bool bReadOnly/* = false*/)
{
	if(dMax < dMin)
		return;
	m_varName = var;
	m_minVal = dMin;
	m_maxVal = dMax;
	m_nDecimal = nDecimal;
	m_bReadOnly = bReadOnly;
	m_nWidth = nWidth;
	m_bAutoSave = true;


	QString temp  = QString::number(m_maxVal, 'f', nDecimal);
	int index = temp.indexOf('.');
	temp.remove('.');
	len = temp.length();
	if(index == -1)
		bei = 1;
	else
	{
		bei = pow((double)10, len-index);
	}
	

	//读取数据库;
	DataBase *pDb = DataBase::getInstance();
	tempData = pDb->queryData(m_varName);
	setText(tempData);


	connect(pDb, SIGNAL(dateChanged(QString, QString)), this, SLOT(dateChanged(QString, QString)));
	//setToolTip("aaa");
}


void EditCtrl::checkValidity(QString val)
{
	double dTemp = val.toDouble();
	if(dTemp > m_maxVal || dTemp < m_minVal)
	{
		setText(tempData);
		return;
	}
	
	QString strDisplay;
	if(m_nWidth == -1 || m_nDecimal!=0)
	{
		strDisplay= QString::number(dTemp,'f',m_nDecimal);
	}
	else
	{
		strDisplay = QString("%1").arg(val.toLongLong(), m_nWidth, 10, QChar('0'));
	}
	setText(strDisplay);

	//更新数据库;
	if(m_bAutoSave)
	{
		saveData(strDisplay);
	}
	emit(dataChanging(tempData));
}


void EditCtrl::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == KEY_RETURN)
	{
		selectAll();
		if(isReadOnly())//进入编辑状态
		{
			g_pMainWindow->setShortCutDisable(true);
			setReadOnly(false);
			for(int i=0;i<text().length();i++)
				data.append(text().at(i));
		}
		else//进入保存状态;
		{
			setReadOnly(true);
			g_pMainWindow->setShortCutDisable(false);

			//解决许可证不能以0开头的问题，张杰华添加@2016-08-10
			if(m_bTextMode)
			{
				setText(text());

				//更新数据库;
				if(m_bAutoSave)
				{
					saveData(text());
				}
				emit(dataChanging(text()));
			}
			else
				checkValidity(text());
		}
	}
	else if(event->key() == KEY_BACKSPACE)
	{
		setText(tempData);
		setReadOnly(true);
		g_pMainWindow->setShortCutDisable(false);
	}
	else if(event->key() == KEY_LEFT)
	{

		QString temp;

		if(selectedText().length())
		{
			data.clear();
			clear();
			temp = "";
		}
		else
		{
			temp = text();
			if(temp.length())
				temp.chop(1);
			if(data.length())
				data.removeLast();
		}

		setText(temp);
	}
	else if(event->key() == KEY_RIGHT)
	{
		if(selectedText().length())
		{
			QString temp = text();
			setText(temp);
		}

		//QString temp = text();
		//if(temp.indexOf('-') != -1)
		//{
		//	temp.remove('-');
		//	data.removeOne("-");
		//}
		//else
		//{
		//	temp.insert(0,'-');
		//	data.insert(0,"-");
		//}

		//setText(temp);
		
	}
	else
	{
		if(selectedText().length())
		{
			data.clear();
			clear();
		}
		if(event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9)
		{
			if(isReadOnly())
				return;

			if(data.length() == len)
				data.clear();
			else
				data.append(event->text());

			QString temp = data.join("");

			if(m_bTextMode)
			{
				setText(temp);
			}
			else
			{
				double ret = temp.toDouble();
				ret = ret/bei;
				setText(QString::number(ret));
			}
		}
		else
			return QLineEdit::keyPressEvent(event);
				
	}
}



void EditCtrl::dateChanged(QString var, QString val)
{
	if(var == m_varName)
		setText(val);
}

void EditCtrl::restoreData()
{
	setText(tempData);
}

void EditCtrl::saveData(QString data)
{
	DataBase::getInstance()->updateDate(m_varName, data);
	tempData = DataBase::getInstance()->queryData(m_varName);
}

void EditCtrl::setTextMode(bool mode)
{
	m_bTextMode = mode;
}
