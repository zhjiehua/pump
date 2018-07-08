#ifndef LOGICMANAGER_H
#define LOGICMANAGER_H

#include <QObject>

class QKeyEvent;

class KLModule : public QObject
{
	Q_OBJECT

public:
	KLModule(QObject *parent);
	~KLModule();

//对外接口:
	void logic_key_filter(QWidget *obj, int operationMold = 0, int navigatorCnt = 5);	//用于监控该窗口的按键消息,默认操作方式（0）;
	void logic_navigator_mold(bool mold){m_bLogicKeyMod = mold;}	//按键切换标签页和焦点的操作方式(0)用到，用于切换到标签页操作模式;
	void logic_navigator_current(){emit(logic_navigator_at(m_nCurrentNavigator)); logic_navigator_mold(true);};	//触发当前导航页,方式[0];
	void logic_set_navigator_current(int m_nCurrent);

signals:
	void logic_navigator_at(int index);					//标签页切换消息;
	void logic_focus_next_right();						//向右切换焦点;
	void logic_focus_next_left();						//向左切换焦点;
	void logic_focus_next_up();							//向上切换焦点;
	void logic_focus_next_down();						//向下切换焦点;

protected:
	bool eventFilter(QObject *obj, QEvent *event);

private:
	int m_nLogicKeyOperationMold;						//按键切换页面和焦点的操作方式;
	int m_nNavigatorCnt;								//标签页数目;
	int m_nCurrentNavigator;							//当前导航页;
	bool m_bLogicKeyMod;								//当只有上下左右按键来操作标签页时候用到该变量[按键操作方式0]									

	bool logic_key_deal_mold0(QKeyEvent *ke);			//按键切换标签页和焦点的操作方式(0);
	bool logic_key_deal_mold1(QKeyEvent *ke);			//按键切换标签页和焦点的操作方式(1);！！！！！新的方式可以在此实现，预留;


};

#endif // LOGICMANAGER_H
