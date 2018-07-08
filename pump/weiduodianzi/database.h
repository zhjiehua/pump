#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QMap>
#include <QList>
#include "lineuint.h"
#include "sqlite3.h"
#include <QThread>

#define	DEF_COMPEN_TBL_10ML		0
#define DEF_COMPEN_TBL_50ML		1
#define DEF_COMPEN_TBL_100ML	2
#define DEF_COMPEN_TBL_150ML	3
#define DEF_COMPEN_TBL_250ML	4
#define DEF_COMPEN_TBL_300ML	5
#define DEF_COMPEN_TBL_500ML	6
#define DEF_COMPEN_TBL_800ML	7
#define DEF_COMPEN_TBL_1000ML	8
#define DEF_COMPEN_TBL_2000ML	9
#define DEF_COMPEN_TBL_3000ML	10

class DataBase : public QObject
{
	Q_OBJECT

public:
	~DataBase();
	static DataBase *getInstance();

	void saveDb();														//保存数据库文件;
	void updateDate(QString name, QString data);						//更新数据;
	QString queryData(QString name);									//查询数据;
	quint32 getGradientEndTime(){return endTime;};						//获取梯度表结束时间;
	QList<LineUint> &getGradientList(){return gradientList;}			//获取梯度表;
	QList<LineUint> &getFlowCompensationList(){return flowCompensationList;}//获取流量补偿表;
	QList<LineUint> &getPressCompensationList(){return pressCompensationList;}//获取流量补偿表;

	QList<QStringList> readGradientTable(int which = 0);				//读取梯度表;
	void updateGradientTable(int which, QList<QStringList>data);		//更新梯度表;

	QList<QStringList> readCompensationTable(int which = 0);			//读取补偿表(流量或梯度补偿);
	void updateCompensationTable(int which, QList<QStringList>data);	//更新补偿表;
	void updateFlowCompensationTable(QList<QStringList>data);			
	void updatePressCompensationTable(QList<QStringList>data);

	void readGradientTableIntoList(int which = 0);						//读出剃度表，进行必要的转换;
	void readFlowCompensationTableIntoList();							//读出流量补偿表，进行必要的转换;
	void readFlowCompensationTableIntoList(QList<QStringList>&data);	//读出流量补偿表，进行必要的转换;
	void readPressCompensationTableIntoList();							//读出压力补偿表，进行必要的转换;
	void readPressCompensationTableIntoList(QList<QStringList>&data);	//读出压力补偿表，进行必要的转换;
	void reOrderList(QList<QStringList> &list);							//排序;

	void saveTableToFile(QString fileName, QList<QStringList>&data);//保存表格到文件;

	//恢复出厂默认补偿表格(压力和流速);
	void restoreCompensationTable();

private:
	DataBase(QObject *parent = 0);
	QThread m_thread;
	QMap<QString, QString>dataBase;										//数据库内容;
	quint32 endTime;
	QList<LineUint> gradientList;
	QList<LineUint> flowCompensationList;
	QList<LineUint> pressCompensationList;


	char *m_zErrMsg;
	sqlite3 *m_db;
	int m_ret;
	int m_nrow;
	int m_ncolumn;
	char **m_azResult;									//二维数组存放结果;


	//method
	void initDb();										//初始化数据库;
	void createGradientTable();							//创建12个梯度表;
	void createCompensationTable();						//创建流量补偿表和压力补偿表;
	void restoreDefPressCompenTable(int pumpType);		//创建出厂默认压力校正表格;
	void restoreDefFlowCompenTable(int pumpType);		//创建出厂默认流速校正表格;
	

signals:
	void dateChanged(QString, QString);
	void save();

private slots:
	void saveToDataBase();//保存数据库文件;
	void saveData(QString name, QString val);			//保存数据到数据库文件;
	
};

#endif // DATABASE_H
