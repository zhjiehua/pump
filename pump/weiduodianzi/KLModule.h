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

//����ӿ�:
	void logic_key_filter(QWidget *obj, int operationMold = 0, int navigatorCnt = 5);	//���ڼ�ظô��ڵİ�����Ϣ,Ĭ�ϲ�����ʽ��0��;
	void logic_navigator_mold(bool mold){m_bLogicKeyMod = mold;}	//�����л���ǩҳ�ͽ���Ĳ�����ʽ(0)�õ��������л�����ǩҳ����ģʽ;
	void logic_navigator_current(){emit(logic_navigator_at(m_nCurrentNavigator)); logic_navigator_mold(true);};	//������ǰ����ҳ,��ʽ[0];
	void logic_set_navigator_current(int m_nCurrent);

signals:
	void logic_navigator_at(int index);					//��ǩҳ�л���Ϣ;
	void logic_focus_next_right();						//�����л�����;
	void logic_focus_next_left();						//�����л�����;
	void logic_focus_next_up();							//�����л�����;
	void logic_focus_next_down();						//�����л�����;

protected:
	bool eventFilter(QObject *obj, QEvent *event);

private:
	int m_nLogicKeyOperationMold;						//�����л�ҳ��ͽ���Ĳ�����ʽ;
	int m_nNavigatorCnt;								//��ǩҳ��Ŀ;
	int m_nCurrentNavigator;							//��ǰ����ҳ;
	bool m_bLogicKeyMod;								//��ֻ���������Ұ�����������ǩҳʱ���õ��ñ���[����������ʽ0]									

	bool logic_key_deal_mold0(QKeyEvent *ke);			//�����л���ǩҳ�ͽ���Ĳ�����ʽ(0);
	bool logic_key_deal_mold1(QKeyEvent *ke);			//�����л���ǩҳ�ͽ���Ĳ�����ʽ(1);�����������µķ�ʽ�����ڴ�ʵ�֣�Ԥ��;


};

#endif // LOGICMANAGER_H
