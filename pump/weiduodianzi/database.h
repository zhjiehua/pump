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

	void saveDb();														//�������ݿ��ļ�;
	void updateDate(QString name, QString data);						//��������;
	QString queryData(QString name);									//��ѯ����;
	quint32 getGradientEndTime(){return endTime;};						//��ȡ�ݶȱ����ʱ��;
	QList<LineUint> &getGradientList(){return gradientList;}			//��ȡ�ݶȱ�;
	QList<LineUint> &getFlowCompensationList(){return flowCompensationList;}//��ȡ����������;
	QList<LineUint> &getPressCompensationList(){return pressCompensationList;}//��ȡ����������;

	QList<QStringList> readGradientTable(int which = 0);				//��ȡ�ݶȱ�;
	void updateGradientTable(int which, QList<QStringList>data);		//�����ݶȱ�;

	QList<QStringList> readCompensationTable(int which = 0);			//��ȡ������(�������ݶȲ���);
	void updateCompensationTable(int which, QList<QStringList>data);	//���²�����;
	void updateFlowCompensationTable(QList<QStringList>data);			
	void updatePressCompensationTable(QList<QStringList>data);

	void readGradientTableIntoList(int which = 0);						//������ȱ����б�Ҫ��ת��;
	void readFlowCompensationTableIntoList();							//�����������������б�Ҫ��ת��;
	void readFlowCompensationTableIntoList(QList<QStringList>&data);	//�����������������б�Ҫ��ת��;
	void readPressCompensationTableIntoList();							//����ѹ�����������б�Ҫ��ת��;
	void readPressCompensationTableIntoList(QList<QStringList>&data);	//����ѹ�����������б�Ҫ��ת��;
	void reOrderList(QList<QStringList> &list);							//����;

	void saveTableToFile(QString fileName, QList<QStringList>&data);//�������ļ�;

	//�ָ�����Ĭ�ϲ������(ѹ��������);
	void restoreCompensationTable();

private:
	DataBase(QObject *parent = 0);
	QThread m_thread;
	QMap<QString, QString>dataBase;										//���ݿ�����;
	quint32 endTime;
	QList<LineUint> gradientList;
	QList<LineUint> flowCompensationList;
	QList<LineUint> pressCompensationList;


	char *m_zErrMsg;
	sqlite3 *m_db;
	int m_ret;
	int m_nrow;
	int m_ncolumn;
	char **m_azResult;									//��ά�����Ž��;


	//method
	void initDb();										//��ʼ�����ݿ�;
	void createGradientTable();							//����12���ݶȱ�;
	void createCompensationTable();						//���������������ѹ��������;
	void restoreDefPressCompenTable(int pumpType);		//��������Ĭ��ѹ��У�����;
	void restoreDefFlowCompenTable(int pumpType);		//��������Ĭ������У�����;
	

signals:
	void dateChanged(QString, QString);
	void save();

private slots:
	void saveToDataBase();//�������ݿ��ļ�;
	void saveData(QString name, QString val);			//�������ݵ����ݿ��ļ�;
	
};

#endif // DATABASE_H
