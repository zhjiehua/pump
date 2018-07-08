#include "mytablemodel.h"
#include <QDebug>
#include "baseMainPage.h"

MyTableModel::MyTableModel(QObject *parent, int nColumnCnt/* = 0*/, int nWhichTbl/* = 0*/)
	: QAbstractTableModel(parent)
	, m_insertMode(COPYLASTROW)
	, m_nColumnCnt(nColumnCnt)
	, m_nWhichTable(nWhichTbl)
{

}


MyTableModel::~MyTableModel()
{

}


QVariant MyTableModel::data( const QModelIndex &index, int role ) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::TextAlignmentRole) {
		return int(Qt::AlignLeft | Qt::AlignVCenter);
	}
	else if(role == Qt::DisplayRole)//显示内容;
	{
		return tableData.at(index.row()).at(index.column());
	}
	else if(role == Qt::BackgroundRole)
	{
		/*if(tableData.at(index.row()).count() > m_nColumnCnt)
		{
			if(tableData.at(index.row()).at(m_nColumnCnt).toInt())
				return QBrush(QColor(64, 128, 0));
		}*/
		
		return QBrush(QColor(255, 255, 255));
	}
	else
		return QVariant();
}


int MyTableModel::rowCount( const QModelIndex &parent ) const
{
	return tableData.count();
}


int MyTableModel::columnCount( const QModelIndex &parent ) const
{
	/*if(tableData.count() > 0)
	return tableData.at(0).count();
	else*/
		return m_nColumnCnt;
}


bool MyTableModel::insertRows( int row, int count, const QModelIndex & parent /*= QModelIndex()*/ )
{
	beginInsertRows(QModelIndex(), row, row+count-1);
	
	if(m_insertMode == COPYLASTROW)
		insertRowsWithCopyMode(row, count);
	else
		insertRowsWithUsrMode(row, count);
	
	endInsertRows();
	return true;
}


bool MyTableModel::removeRows( int row, int count, const QModelIndex &parent /*= QModelIndex() */ )
{
	beginRemoveRows(QModelIndex(), row, row+count-1);
	for(int i = 0; i < count; i++)
		tableData.removeAt(row);
	endRemoveRows();
	return true;
}


bool MyTableModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
	if (index.isValid() && role == Qt::EditRole)
	{
		QString temp = value.toString();
		double dData = temp.toDouble();
		temp = QString::number(dData);
		QStringList list = tableData.at(index.row());
		list.replace(index.column(), temp);
		tableData.replace(index.row(), list);
		emit(dataChanged(index, index));
		return true;
	}
	return false;
}


Qt::ItemFlags MyTableModel::flags( const QModelIndex & index ) const
{
	Qt::ItemFlags flags = QAbstractItemModel::flags(index);
	flags |= Qt::ItemIsEditable;
	return flags;
}


void MyTableModel::readDataFromTable(int which)
{
	m_nWhichTable = which;

	//读取数据库资料;
	if(which < TABLE_FLOW_FIX)//读取梯度表;
	{
		tableData = DataBase::getInstance()->readGradientTable(which);
	}
	else if(which == TABLE_FLOW_FIX)//读取流量补偿表格;
	{
		tableData = DataBase::getInstance()->readCompensationTable();
	}
	else if(which == TABLE_PRESS_FIX)//读取压力补偿表格;
	{
		tableData = DataBase::getInstance()->readCompensationTable(1);
	}
}



void MyTableModel::insertRow(int row, TableInsertMode mode, const QStringList &data)
{
	m_insertMode = mode;
	tempData = data;
	QAbstractTableModel::insertRow(row);
}

void MyTableModel::insertRowsWithCopyMode(int row, int count)
{
	if( tableData.count() )
	{
		QStringList list = tableData.at(row-1);
		QString temp = tableData.at(row-1).at(0);
		for(int i = 0; i < count; i++)
		{
			double time = temp.toDouble();
			time += 0.1;
			temp = QString::number(time, 'f', 1);
			list.replace(0, temp);
			tableData.insert(row+i, list);
		}
	}
	else//插入新的行;
	{
		QStringList list;
		for (int i = 0; i < m_nColumnCnt; i++)
		{
			list.append("0");
		}
		tableData.insert(row, list);
	}
}

void MyTableModel::insertRowsWithUsrMode(int row, int count)
{
	for(int i = 0; i < count; i++)
	{
		tableData.insert(row+i, tempData);
	}
}


QVariant MyTableModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole */ ) const
{
	QStringList gradientHeader;
	gradientHeader << QObject::tr("Time") << QObject::tr("Flow") << QObject::tr("A") << QObject::tr("B"); 
	
	QStringList flowFixHeader;
	flowFixHeader << QObject::tr("Flow") << QObject::tr("Factor") ; 

	QStringList pressFixHeader;
	pressFixHeader << QObject::tr("Press") << QObject::tr("Factor") ; 

	if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{	
		if(m_nWhichTable < TABLE_FLOW_FIX)
			return gradientHeader[section];
		else if(m_nWhichTable == TABLE_FLOW_FIX)
			return flowFixHeader[section];
		else if(m_nWhichTable == TABLE_PRESS_FIX)
			return pressFixHeader[section];
	}
	return QAbstractTableModel::headerData(section, orientation, role);
}















