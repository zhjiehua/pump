#ifndef BASEPAGE_H
#define BASEPAGE_H
#include <QWidget>
#include <QKeyEvent>
#include <QDebug>
#include "Common.h"


#define BTNGLOBALSTYLE "QPushButton{outline: 0px;}""QPushButton:focus{border: 2px solid blue;outline: 5px;}" //张杰华修改@2016-06-23
//#define BTNGLOBALSTYLE "QPushButton{outline: 0px;}""QPushButton:focus{border: 2px solid green;outline: 0px;}"

class CBasePage;
typedef CBasePage*(*getPageInstance)(QWidget *, quint8, quint8, quint32);

typedef struct tagStructPageMethod
{
	getPageInstance getInstance;
	quint8 nPrevIndex;
}StructPageMethod ,*LPStructPageMethod;


class CBasePage : public QWidget
{
	Q_OBJECT

protected:
	QString m_strTitle;		//标题;
	quint8 m_nPageIndex;		//页面ID;
	quint8 m_nPrevPageIndex;	//下个页面ID;
	quint32 m_nAdd;
	CBasePage *m_pCurrentPage;
	QList<StructPageMethod>pageMethod;
	virtual void initFocusList(){}
	void filterEventForChildren();
	bool eventFilter(QObject *obj, QEvent *event);
	

public:
	CBasePage(  QWidget *parent = 0 );
	CBasePage( QString strTitle, quint8 nPageIndex, quint8 nPrevPageIndex, quint32 add, QWidget *parent = 0 );
	~CBasePage(void);

	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 prevIndex, quint32 add);
	void addPage(getPageInstance pageInstanceMethod, quint8 prevIndex);

	void initFocus();
	void setTitle(QString strTitle) { m_strTitle = strTitle;}
	QString GetTitle()		{ return m_strTitle; }
	quint8 GetPageIndex()		{ return m_nPageIndex; }
	quint8 GetPrevPageIndex()	{ return m_nPrevPageIndex; }
	CBasePage* changePage(int index, quint32 addData = 0);

	//控件焦点列表;
	QObjectList xList;
	QObjectList yList;
};





#endif // BASEPAGE_H