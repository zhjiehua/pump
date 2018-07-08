#include "database.h"
#include "sqlite3.h"
#include <QDebug>
#include <QMap>
#include "lineuint.h"
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>

DataBase::DataBase(QObject *parent)
	: QObject(parent)
	, endTime(1)
	, m_zErrMsg(0)
	, m_db(0)
	,m_ncolumn(0)
	,m_nrow(0)
	,m_azResult(0)
{
	initDb();
	createGradientTable();
	createCompensationTable();
	readFlowCompensationTableIntoList();
	readPressCompensationTableIntoList();
	connect(this, SIGNAL(save()), this, SLOT(saveToDataBase()));
	connect(this, SIGNAL(dateChanged(QString, QString)), this, SLOT(saveData(QString, QString)) );
	moveToThread(&m_thread);
	m_thread.start();
}

DataBase::~DataBase()
{
	saveDb();
	sqlite3_free_table( m_azResult );
	sqlite3_free(m_zErrMsg);
	sqlite3_close(m_db);
}

DataBase * DataBase::getInstance()
{
	static DataBase *pDb = new DataBase();
	return pDb;
}

void DataBase::updateDate(QString name, QString data)
{
	dataBase.insert(name, data);
	emit(dateChanged(name, data));
}

QString DataBase::queryData(QString name)
{
	QString val = dataBase.value(name);
	return val;
}

void DataBase::initDb()
{
	m_ret = sqlite3_open("wda.db", &m_db);//打开指定的数据库文件,如果不存在将创建一个同名的数据库文件;
	if( m_ret )
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(m_db));
		sqlite3_close(m_db);
	}
	else 
		qDebug()<<"You have opened a sqlite3 database ";

	//创建全局参数表格!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!;
	char *sql = "DROP TABLE WDA;" ;
	//sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	//sql = "CREATE TABLE IF NOT EXISTS WDA(ID INTEGER PRIMARY KEY,NAME TEXT ,VALUE TEXT);" ;
	sql = "CREATE TABLE IF NOT EXISTS WDA(NAME TEXT PRIMARY KEY,VALUE TEXT);" ;
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	//  	sql = "DELETE FROM SensorData;" ;
	//  	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	//插入数据; 
	//参数页面数据;
	sql = "INSERT INTO WDA VALUES('language' , '0');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('pumptype' , '0');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('flowrate' , '0');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('maxpress' , '42');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('minpress' , '0');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('Compensation' , '0');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('gradient' , '0');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('boltcoefficient' , '0');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('gradientTable' , '0');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('currentGradient' , '0');";//当前运行梯度;
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('coefficient' , '1');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('flowRate' , '1.5');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('pressRawV0' , '20');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('bugleCnt', 0);";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('pcProtocol', 0)";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('pumpPurgeFlowVal', 0)";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );

	//年月日时分秒;
	sql = "INSERT INTO WDA VALUES('year' , '2015');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('month' , '10');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('day' , '1');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('hour' , '5');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('minute' , '5');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('second' , '5');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('datetimeoffset' , '0');";//日期偏移量;
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('firstTryDateTime' , '0');";//开始试用的日期时间;
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );

	//上次维修日期
	sql = "INSERT INTO WDA VALUES('repairYear' , '0000');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('repairMonth' , '00');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('repairDay' , '0');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );

	//生产日期
	sql = "INSERT INTO WDA VALUES('manufYear' , '2015');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('manufMonth' , '10');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('manufDay' , '1');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );

	//安装日期
	sql = "INSERT INTO WDA VALUES('instYear' , '2015');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('instMonth' , '10');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('instDay' , '1');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );

	//许可证序列号;
	sql = "INSERT INTO WDA VALUES('lic' , '123456');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('license' , '1111111111');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('serial' , '0000000000');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('tryDay' , '14');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('activeCode' , '0000000000');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );

	//用户密码;
	sql = "INSERT INTO WDA VALUES('pwd' , '173895');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('usrpwd' , '222222');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );


	//时间页面;
	sql = "INSERT INTO WDA VALUES('usedTime' , '0');";//累计开机时间;
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('pumpUsedTime' , '0');";//栓塞累计使用时间;
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );


	//激活状态;
	sql = "INSERT INTO WDA VALUES('bActive' , '0');";//设备是否永久激活;
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('bTryDay' , '0');";//设备是否到达试用时间;
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );

	//IP地址
	sql = "INSERT INTO WDA VALUES('ip1' , '192');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('ip2' , '168');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('ip3' , '1');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('ip4' , '120');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('port' , '6666');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('remote_ip1' , '192');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('remote_ip2' , '168');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('remote_ip3' , '1');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('remote_ip4' , '115');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('remote_port' , '7777');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "INSERT INTO WDA VALUES('pmax' , '0.00');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );

	sql = "INSERT INTO WDA VALUES('last_time' , '0');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );

	//! 端口(网口或串口)
	sql = "INSERT INTO WDA VALUES('connect_port' , '0');";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );

	sql = "SELECT * FROM WDA ";
	sqlite3_get_table( m_db , sql , &m_azResult , &m_nrow , &m_ncolumn , &m_zErrMsg );
	qDebug("row:%d column=%d \n" , m_nrow , m_ncolumn );
	qDebug()<<( "The result of querying is :" );
	for(int i=2 ; i<( m_nrow + 1 ) * m_ncolumn ; )
	{
		QString key = QString(m_azResult[i]);
		QString val = QString(m_azResult[i+1]);
		dataBase.insert(key, val);
		i = i+2;
	}

	//将数据库读进内存;
	QMap<QString, QString>::const_iterator i;
	for (i = dataBase.constBegin(); i != dataBase.constEnd(); ++i) {
		qDebug() << i.key() << ":" << i.value();
	}
}

void DataBase::saveDb()
{
	emit(save());
}


void DataBase::createGradientTable()
{
	//创建梯度表格;
	QString tableName;
	QString temp;
	for(int i=0; i < 12; i++)
	{
		tableName = QString("GRADIENTTABLE%1").arg(i);

		temp = QString("CREATE TABLE IF NOT EXISTS %1(ID INTEGER PRIMARY KEY, TIME TEXT,VALUE TEXT, ABENG TEXT, BBENG TEXT);").arg(tableName) ;
		sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );

		//插入默认数据;
		temp = QString("INSERT INTO %1 VALUES(0,'0' , '0.5', '100','0');").arg(tableName);
		sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );

		temp = QString("INSERT INTO %1 VALUES(1,'0.1' , '0.6', '100','0');").arg(tableName);
		sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );

		temp = QString("INSERT INTO %1 VALUES(2,'0.2' , '0.7', '100','0');").arg(tableName);
		sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );

		temp = QString("INSERT INTO %1 VALUES(3,'0.3' , '0.8','100','0');").arg(tableName);
		sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );

		temp = QString("INSERT INTO %1 VALUES(4,'0.4' , '0.9','100','0');").arg(tableName);
		sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );
	}
}

void DataBase::createCompensationTable()
{
	char *sql = 0;
	int ret = 0;

	//查询目前使用的泵头类型;
	int pumpType = queryData("pumptype").toInt();

	//创建流量补偿表,当烧录程序到核心板，第一次创建表格时候,ret返回SQLITE_OK,否则返回错误;
	//当第一次创建该表格时候，需要设为出厂默认参数;
	sql = "DROP TABLE COMPENSATIONTABLE0";
	//ret = sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	sql = "CREATE TABLE COMPENSATIONTABLE0(FLOW TEXT PRIMARY KEY, VALUE TEXT);";
	ret = sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );
	if(ret == SQLITE_OK)//恢复默认数据;
		restoreDefFlowCompenTable(pumpType);
	


	//创建压力补偿表,当烧录程序到核心板，第一次创建表格时候,ret返回SQLITE_OK,否则返回错误;
	//当第一次创建该表格时候，需要设为出厂默认参数;
	sql = "DROP TABLE COMPENSATIONTABLE1";
	//ret = sqlite3_exec( m_db , sql, 0 , 0 , &m_zErrMsg );
	sql = "CREATE TABLE COMPENSATIONTABLE1(PRESS TEXT PRIMARY KEY, VALUE TEXT);";
	ret = sqlite3_exec( m_db , sql, 0 , 0 , &m_zErrMsg );
	if(ret == SQLITE_OK)//恢复出厂默认数据;
		restoreDefPressCompenTable(pumpType);
}

void DataBase::readGradientTableIntoList(int which /*= 0*/)
{
	QString tableName = QString("GRADIENTTABLE%1").arg(which);

	//判断是哪个梯度表格;
	QStringList timeList;//时间坐标点;
	QStringList valueList;//值坐标点;
	QStringList percentList;//百分比;
	QString temp;
	temp = QString("SELECT TIME FROM %1 ").arg(tableName);
	sqlite3_get_table( m_db , temp.toLatin1().data() , &m_azResult , &m_nrow , &m_ncolumn , &m_zErrMsg );
	//qDebug()<<( "The result of querying is :" );
	for(int i=1 ; i<( m_nrow + 1 ) * m_ncolumn ; i++ )
	{
		//qDebug( "azResult[%d] = %s", i , m_azResult[i] );
		QString time(m_azResult[i]);
		timeList.append(time);
	}

	temp = QString("SELECT VALUE FROM %1 ").arg(tableName);
	sqlite3_get_table( m_db , temp.toLatin1().data() , &m_azResult , &m_nrow , &m_ncolumn , &m_zErrMsg );
	qDebug()<<( "The result of querying is :" );
	for(int i=1 ; i<( m_nrow + 1 ) * m_ncolumn ; i++ )
	{
		//qDebug( "azResult[%d] = %s", i , m_azResult[i] );
		QString value(m_azResult[i]);
		valueList.append(value);
	}

	temp = QString("SELECT ABENG FROM %1 ").arg(tableName);
	sqlite3_get_table( m_db , temp.toLatin1().data() , &m_azResult , &m_nrow , &m_ncolumn , &m_zErrMsg );
	for(int i=1 ; i<( m_nrow + 1 ) * m_ncolumn ; i++ )
	{
		//qDebug( "azResult[%d] = %s", i , m_azResult[i] );
		QString value(m_azResult[i]);
		percentList.append(value);
	}
	

	//分钟转换成秒;
	//流速转成数值;

	//判断点的个数;
	int len = timeList.count();
	//情况1:0个点;//未实现!!!!!!!!!!!!!!!!!!
	//情况2:1个点://未实现!!!!!!!!!!!!!!!!

	//情况3:2个点以上;//正常!!!!!!!!!!!!!!!!!!!
	if(len >= 2 )
	{
		//清空gradientList
		gradientList.clear();
		for(int i = 0; i < len-1; i++)
		{
			LineUint line;
			double x1 = timeList.at(i).toDouble() * 60;
			double x2 = timeList.at(i+1).toDouble() * 60;
			double y1 = valueList.at(i).toDouble();
			double y2 = valueList.at(i+1).toDouble();
			line.setRange(x1, y1, x2, y2, valueList.at(i));
			line.setPercentVal(percentList.at(i).toInt());

			gradientList.append(line);
		}
		endTime = timeList.last().toDouble()*60;
	}
	else
		return;
}

QList<QStringList> DataBase::readGradientTable(int which /*= 0*/)
{
	QString temp;
	QString tableName = QString("GRADIENTTABLE%1").arg(which);
	temp = QString("SELECT * FROM %1;").arg(tableName);
	sqlite3_get_table( m_db , temp.toLatin1().data() , &m_azResult , &m_nrow , &m_ncolumn , &m_zErrMsg );

	QList<QStringList>ret;
	for(int i=1 ; i<( m_nrow + 1 ) ; i++ )
	{
		QStringList list;
		for(int j = 0; j < m_ncolumn; j++)
		{
			int index = i*5+j;
			if(index%5 == 0)
				continue;
			QString data = QString(m_azResult[index]);
			list.append(data);
		}
		ret.append(list);
	}
	return ret;
}

void DataBase::updateGradientTable(int which, QList<QStringList>data)
{
	//清空表格;
	QString tableName = QString("GRADIENTTABLE%1").arg(which);
	QString temp;
	temp = QString("DELETE FROM %1;").arg(tableName);
	sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );

	//插入数据;
	for ( int i = 0; i < data.count(); i++)
	{
		temp = QString("INSERT INTO %1 VALUES(%2,'%3' , '%4', '%5','%6');")
			.arg(tableName)
			.arg(i)
			.arg(data.at(i).at(0))
			.arg(data.at(i).at(1))
			.arg(data.at(i).at(2))
			.arg(data.at(i).at(3));

		sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );
	}
}

QList<QStringList> DataBase::readCompensationTable(int which /*= 0*/)
{
	QString temp;
	QString tableName = QString("COMPENSATIONTABLE%1").arg(which);
	temp = QString("SELECT * FROM %1").arg(tableName);
	sqlite3_get_table( m_db , temp.toLatin1().data() , &m_azResult , &m_nrow , &m_ncolumn , &m_zErrMsg );

	//读取数据库表格;
	QList<QStringList>ret;
	for(int i=1 ; i<( m_nrow + 1 ) ; i++ )
	{
		QStringList list;
		for(int j = 0; j < m_ncolumn; j++)
		{
			int index = i*m_ncolumn+j;
			QString data = QString(m_azResult[index]);
			list.append(data);
		}
		ret.append(list);
	}

	return ret;
}

void DataBase::updateFlowCompensationTable(QList<QStringList>data)
{
	int row = data.count();

	//清空表格;
	QString temp;
	temp = QString("DELETE FROM COMPENSATIONTABLE0;");
	sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );

	//插入数据;
	for ( int i = 0; i < row; i++)
	{
		temp = QString("INSERT INTO COMPENSATIONTABLE0 VALUES('%1' , '%2');")
			.arg(data.at(i).at(0))
			.arg(data.at(i).at(1));
		sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );
	}
	readFlowCompensationTableIntoList();

}

void DataBase::updatePressCompensationTable(QList<QStringList>data)
{
	int row = data.count();

	//清空表格;
	QString temp;
	temp = QString("DELETE FROM COMPENSATIONTABLE1;");
	sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );

	//插入数据;
	for ( int i = 0; i < data.count(); i++)
	{
		temp = QString("INSERT INTO COMPENSATIONTABLE1 VALUES('%1' , '%2');")
			.arg(data.at(i).at(0))
			.arg(data.at(i).at(1));
		sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );
	}
	readPressCompensationTableIntoList();
}

void DataBase::readFlowCompensationTableIntoList()
{
	QString temp;
	temp = QString("SELECT * FROM COMPENSATIONTABLE0 ");
	sqlite3_get_table( m_db , temp.toLatin1().data() , &m_azResult , &m_nrow , &m_ncolumn , &m_zErrMsg );
	//qDebug()<<( "The result of querying is :" );
	QList<QStringList>list;
	for(int i=2 ; i<( m_nrow + 1 ) * m_ncolumn ; )//0、1个是标题;
	{
		//qDebug( "azResult[%d] = %s", i , m_azResult[i] );
		QString flow(m_azResult[i]);
		QString value(m_azResult[i+1]);
		QStringList record;
		record.append(flow);
		record.append(value);
		list.append(record);
		i= i+2;
	}
	readFlowCompensationTableIntoList(list);
}

void DataBase::readFlowCompensationTableIntoList(QList<QStringList> &list)
{
	reOrderList(list);
	QList<QStringList> &data = list;
	QStringList flowList;//流量坐标点;
	QStringList valueList;//校正值坐标点;
	for (int i = 0; i < data.count(); i++)
	{
		QStringList list = data.at(i);
		flowList.append(list.at(0));
		valueList.append(list.at(1));
	}
	//判断点的个数;
	int len = data.count();
	//情况1:0个点;//未实现!!!!!!!!!!!!!!!!!!
	//情况2:1个点://未实现!!!!!!!!!!!!!!!!

	//情况3:2个点以上;//正常!!!!!!!!!!!!!!!!!!!
	if(len >= 2 )
	{
		flowCompensationList.clear();
		for(int i = 0; i < len-1; i++)
		{
			LineUint line;
			double x1 = flowList.at(i).toDouble();
			double x2 = flowList.at(i+1).toDouble();
			double y1 = valueList.at(i).toDouble();
			double y2 = valueList.at(i+1).toDouble();
			line.setRange(x1, y1, x2, y2, valueList.at(i));
			flowCompensationList.append(line);
		}
	}
	else
		return;
}

void DataBase::readPressCompensationTableIntoList()
{
	QString temp;
	temp = QString("SELECT * FROM COMPENSATIONTABLE1");
	sqlite3_get_table( m_db , temp.toLatin1().data() , &m_azResult , &m_nrow , &m_ncolumn , &m_zErrMsg );
	QList<QStringList>list;
	for(int i=2 ; i<( m_nrow + 1 ) * m_ncolumn ; )
	{
		QString flow(m_azResult[i]);
		QString value(m_azResult[i+1]);
		i= i+2;
		QStringList record;
		record.append(flow);
		record.append(value);
		list.append(record);
	}

	readPressCompensationTableIntoList(list);
}

void DataBase::readPressCompensationTableIntoList(QList<QStringList> &list)
{
	//排序;
	reOrderList(list);
	QList<QStringList> &data = list;
	QStringList pressList;//流量坐标点;
	QStringList valueList;//校正值坐标点;
	//qDebug()<<"after reorder!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
	for (int i = 0; i < data.count(); i++)
	{
		QStringList list = data.at(i);
		//qDebug()<<list;
		pressList.append(list.at(0));
		valueList.append(list.at(1));
	}
	//判断点的个数;
	int len = data.count();
	//情况1:0个点;//未实现!!!!!!!!!!!!!!!!!!
	//情况2:1个点://未实现!!!!!!!!!!!!!!!!

	//情况3:2个点以上;//正常!!!!!!!!!!!!!!!!!!!
	if(len >= 2 )
	{
		pressCompensationList.clear();
		for(int i = 0; i < len-1; i++)
		{
			LineUint line;
			double x1 = pressList.at(i).toDouble();
			double x2 = pressList.at(i+1).toDouble();
			double y1 = valueList.at(i).toDouble();
			double y2 = valueList.at(i+1).toDouble();
			line.setRange(x1, y1, x2, y2, valueList.at(i));
			pressCompensationList.append(line);
		}
	}
	else
		return;
}


bool compareForList(const QStringList &s1, const QStringList &s2)
{
	if(s1.count() <=0 || s2.count() <=0 )
		return false;

	if(s1.at(0).toDouble() < s2.at(0).toDouble() )
		return true;
	else
		return false;
}

void DataBase::reOrderList(QList<QStringList> &list)
{
	qSort(list.begin(), list.end(), compareForList);
}

void DataBase::saveTableToFile(QString fileName, QList<QStringList>&data)
{
	QFile file(fileName);

	if (!file.open(QFile::WriteOnly | QFile::Truncate))
		return;
	QTextStream out(&file);
	for(int i = 0; i < data.count(); i++)
	{
		for(int j = 0; j < data.at(i).count(); j++)
		{
			out<<qSetFieldWidth(10)<<left<<data.at(i).at(j);
		}
		out<<qSetFieldWidth(0)<<endl;
	}
}

void DataBase::saveToDataBase()
{
#if 0
	QString temp;
	for (QMap<QString, QString>::ConstIterator i = dataBase.constBegin(); i != dataBase.constEnd(); ++i)
	{
		temp = QString("UPDATE WDA SET VALUE = '%1' WHERE NAME='%2';")
			.arg(i.value())
			.arg(i.key());
		//qDebug()<<temp;
		sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );
	}
#endif
}

void DataBase::restoreDefPressCompenTable(int pumpType)
{
	//泵的压力校正表;
	QMap<double, double>tbl;
	tbl.clear();
	switch( pumpType )
	{
		case DEF_COMPEN_TBL_10ML:
			{
				tbl[0.1]=1;
				tbl[0.5]=1;
				tbl[1]=1;
				tbl[3]=1;
				tbl[5]=1;
				tbl[10]=1;
				tbl[15]=1;
				tbl[20]=1;
				tbl[25]=1;
				tbl[30]=1;
				tbl[35]=1;
				tbl[40]=1;
				tbl[45]=1;
			}
			break;
		case DEF_COMPEN_TBL_50ML:
			{
				tbl[0.1]=1;
				tbl[0.5]=1;
				tbl[1]=1;
				tbl[3]=1;
				tbl[5]=1;
				tbl[10]=1;
				tbl[15]=1;
				tbl[20]=1;
				tbl[25]=1;
				tbl[30]=1;
				tbl[35]=1;
			}
		case DEF_COMPEN_TBL_100ML:
		case DEF_COMPEN_TBL_150ML:
		case DEF_COMPEN_TBL_250ML:
		case DEF_COMPEN_TBL_300ML:
		case DEF_COMPEN_TBL_500ML:
		case DEF_COMPEN_TBL_800ML:
		case DEF_COMPEN_TBL_1000ML:
		case DEF_COMPEN_TBL_2000ML:
		case DEF_COMPEN_TBL_3000ML:
			{
				tbl[0.1]=1;
				tbl[0.5]=1;
				tbl[1]=1;
				tbl[3]=1;
				tbl[5]=1;
				tbl[7]=1;
				tbl[9]=1;
				tbl[10]=1;
				tbl[12]=1;
			}
			break;
		default:
			pumpType = -1;
		break;
	}
	
	if(pumpType == -1)
		return;

	//先删除之前的表格
	char *sql = "DELETE FROM COMPENSATIONTABLE1";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );

	//插入出厂默认值;
	QMap<double, double>::const_iterator it;
	for (it = tbl.constBegin(); it != tbl.constEnd(); ++it) {
		QString temp = QString("INSERT INTO COMPENSATIONTABLE1 VALUES('%1', '%2');").arg(it.key()).arg(it.value());
		sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );
	}
	
}

void DataBase::restoreDefFlowCompenTable(int pumpType)
{
	//泵的压力校正表;
	QMap<double, double>tbl;
	tbl.clear();
	switch( pumpType )
	{
	case DEF_COMPEN_TBL_10ML:
		{
			tbl[0.01]=1;
			tbl[0.02]=1;
			tbl[0.05]=1;
			tbl[0.1]=1;
			tbl[0.2]=1;
			tbl[0.5]=1;
			tbl[1]=1;
			tbl[2]=1;
			tbl[5]=1;
			tbl[10]=1;
		}
		break;
	case DEF_COMPEN_TBL_50ML:
		{
			tbl[0.05]=1;
			tbl[0.2]=1;
			tbl[0.5]=1;
			tbl[1]=1;
			tbl[2]=1;
			tbl[5]=1;
			tbl[10]=1;
			tbl[20]=1;
			tbl[35]=1;
			tbl[50]=1;
		}
	case DEF_COMPEN_TBL_100ML:
		{
			tbl[1]=1;
			tbl[2]=1;
			tbl[4]=1;
			tbl[8]=1;
			tbl[20]=1;
			tbl[50]=1;
			tbl[70]=1;
			tbl[80]=1;
			tbl[85]=1;
			tbl[100]=1;
		}
		break;
	case DEF_COMPEN_TBL_150ML:
		{
			tbl[2]=1;
			tbl[5]=1;
			tbl[10]=1;
			tbl[20]=1;
			tbl[40]=1;
			tbl[60]=1;
			tbl[80]=1;
			tbl[100]=1;
			tbl[125]=1;
			tbl[150]=1;
		}
		break;
	case DEF_COMPEN_TBL_250ML:
		{
			tbl[2]=1;
			tbl[5]=1;
			tbl[10]=1;
			tbl[20]=1;
			tbl[50]=1;
			tbl[125]=1;
			tbl[175]=1;
			tbl[200]=1;
			tbl[210]=1;
			tbl[250]=1;
		}
		break;
	case DEF_COMPEN_TBL_300ML:
		{
			tbl[5]=1;
			tbl[10]=1;
			tbl[20]=1;
			tbl[40]=1;
			tbl[70]=1;
			tbl[100]=1;
			tbl[150]=1;
			tbl[200]=1;
			tbl[250]=1;
			tbl[300]=1;
		}
		break;
	case DEF_COMPEN_TBL_500ML:
		{
			tbl[5]=1;
			tbl[10]=1;
			tbl[20]=1;
			tbl[40]=1;
			tbl[100]=1;
			tbl[250]=1;
			tbl[350]=1;
			tbl[400]=1;
			tbl[425]=1;
			tbl[500]=1;
		}
		break;
	case DEF_COMPEN_TBL_800ML:
		{
			tbl[10]=1;
			tbl[20]=1;
			tbl[40]=1;
			tbl[80]=1;
			tbl[150]=1;
			tbl[250]=1;
			tbl[350]=1;
			tbl[500]=1;
			tbl[650]=1;
			tbl[800]=1;
		}
		break;
	case DEF_COMPEN_TBL_1000ML:
		{
			tbl[10]=1;
			tbl[20]=1;
			tbl[40]=1;
			tbl[80]=1;
			tbl[200]=1;
			tbl[500]=1;
			tbl[700]=1;
			tbl[800]=1;
			tbl[850]=1;
			tbl[1000]=1;
		}
		break;
	case DEF_COMPEN_TBL_2000ML:
		{
			tbl[30]=1;
			tbl[60]=1;
			tbl[120]=1;
			tbl[240]=1;
			tbl[500]=1;
			tbl[750]=1;
			tbl[1000]=1;
			tbl[1300]=1;
			tbl[1600]=1;
			tbl[2000]=1;
		}
		break;
	case DEF_COMPEN_TBL_3000ML:
		{
			tbl[30]=1;
			tbl[60]=1;
			tbl[120]=1;
			tbl[240]=1;
			tbl[600]=1;
			tbl[1500]=1;
			tbl[2100]=1;
			tbl[2400]=1;
			tbl[2600]=1;
			tbl[3000]=1;
		}
		break;
	default:
		pumpType = -1;
		break;
	}

	if(pumpType == -1)
		return;

	//先删除之前的表格
	char *sql = "DELETE FROM COMPENSATIONTABLE0";
	sqlite3_exec( m_db , sql , 0 , 0 , &m_zErrMsg );

	//插入出厂默认值;
	QMap<double, double>::const_iterator it;
	for (it = tbl.constBegin(); it != tbl.constEnd(); ++it) {
		QString temp = QString("INSERT INTO COMPENSATIONTABLE0 VALUES('%1', '%2');").arg(it.key()).arg(it.value());
		sqlite3_exec( m_db , temp.toLatin1().data() , 0 , 0 , &m_zErrMsg );
	}
}

void DataBase::restoreCompensationTable()
{
	//查询目前的泵头型号;
	int pumpType = queryData("pumptype").toInt();
	//恢复出厂默认压力校正表格
	restoreDefPressCompenTable(pumpType);
	//恢复出厂默认流量校正表格
	restoreDefFlowCompenTable(pumpType);
}

void DataBase::saveData( QString name, QString val )
{
	//TimeHelper::begin();
	QString sql = QString("UPDATE WDA SET VALUE = '%1' WHERE NAME = '%2';").arg(val).arg(name);
	//qDebug()<<sql;
	sqlite3_exec( m_db , sql.toLatin1().data() , 0 , 0 , &m_zErrMsg );
	//TimeHelper::end();
}
