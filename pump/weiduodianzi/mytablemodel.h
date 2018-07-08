#ifndef MYTABLEMODEL_H
#define MYTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

#define TABLE_FLOW_FIX	20
#define TABLE_PRESS_FIX	21

class MyTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum TableInsertMode
	{
		COPYLASTROW,
		USERDATA,
	};


	MyTableModel(QObject *parent, int nColumnCnt = 0, int nWhichTbl = 0);
	~MyTableModel();

	void readDataFromTable(int which);//�����ݿ��������;
	void updateDataToTable();//�������ݵ����ݿ�;
	QList<QStringList>& getTableData(){return tableData;}

	void insertRow(int row, TableInsertMode mode = COPYLASTROW, const QStringList &data = QStringList());
	int rowCount(const QModelIndex &parent  = QModelIndex()) const;//!!!!!!
	int columnCount(const QModelIndex &parent = QModelIndex()) const;//!!!!

	QList<QStringList> tableData;
	QStringList tempData;

private:
	int m_nColumnCnt;//�������;
	int m_nWhichTable;
	TableInsertMode m_insertMode;

/*******************************����ʵ�ֵ��麯��****************************************/
	QVariant data(const QModelIndex &index, int role) const;//!!!!
	bool setData(const QModelIndex &index, const QVariant &value, int role);//!!!!
	bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());//!!!!
	bool removeRows(int row, int count, const QModelIndex &parent  = QModelIndex() );//!!!!
	Qt::ItemFlags flags ( const QModelIndex & index ) const;
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole )const;

//�����װ�ĺ���;
	void insertRowsWithCopyMode(int row, int count);
	void insertRowsWithUsrMode(int row, int count);

signals:

	
};

#endif // MYTABLEMODEL_H
