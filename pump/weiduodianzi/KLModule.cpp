#include "KLModule.h"
#include <QTimer>
#include <QWidget>
#include <QKeyEvent>
#include <QDebug>

KLModule::KLModule(QObject *parent)
	: QObject(parent)
	, m_nLogicKeyOperationMold(0)
	, m_nCurrentNavigator(0)
	, m_nNavigatorCnt(0)
	, m_bLogicKeyMod(true)
{

}


KLModule::~KLModule()
{

}



bool KLModule::eventFilter(QObject *obj, QEvent *event)
{
	if(event->type() == QEvent::KeyPress)
	{
		QKeyEvent *ke = static_cast<QKeyEvent *>(event);
		if(m_nLogicKeyOperationMold == 0)
			return logic_key_deal_mold0(ke);
		else if(m_nLogicKeyOperationMold == 1)
			return logic_key_deal_mold1(ke);
		else
			return false;
	}
	else
		return false;
}





//******************************************************����ӿ�*******************************************************
void KLModule::logic_key_filter(QWidget *obj, int operationMold/* = 0*/, int navigatorCnt /*= 5*/)
{
	if(!obj)
		qDebug()<<"LogicManager::logic_key_filter object invalid";

	m_nLogicKeyOperationMold = operationMold;
	m_nNavigatorCnt = navigatorCnt;
	obj->installEventFilter(this);
}






//*****************************************************�߼��Ķ�*********************************************************
#define LOGIC_KEY_LEFT		Qt::Key_F1
#define LOGIC_KEY_UP		Qt::Key_F2
#define LOGIC_KEY_RIGHT		Qt::Key_F3
#define LOGIC_KEY_DOWN		Qt::Key_F5
/*˵��:
 *1����忪��ʱ�����л���ǩҳ��ģʽ�����߼����Ҽ������л���ǩҳ;
 *2�����ڲ���1�У������߼��ϼ�����崦���л�����ģʽ;
 *3�����л�����ģʽ��,����������ʵ�ֽ����л�;
 *4������Ҫ���»ص��л���ǩҳģʽ����ʹ��void logic_navigator_mold();
*/
bool KLModule::logic_key_deal_mold0(QKeyEvent *ke)
{
	int keyVal = ke->key();
	switch(keyVal)
	{
		case LOGIC_KEY_LEFT://��
		{
			if(m_bLogicKeyMod)//���Ҽ����ڵ���ģʽ;
			{
				if(m_nNavigatorCnt <= 0)
					return false;

				m_nCurrentNavigator == 0 ? m_nCurrentNavigator = m_nNavigatorCnt-1 : m_nCurrentNavigator--;
				emit(logic_navigator_at(m_nCurrentNavigator));
			}
			else//���Ҽ������л��ؼ�����ģʽ;
				emit(logic_focus_next_left());
		}
		break;
		case LOGIC_KEY_RIGHT://��
		{
			if(m_bLogicKeyMod)//���Ҽ����ڵ���ģʽ;
			{
				if(m_nNavigatorCnt > 0)
				{
					m_nCurrentNavigator = ++m_nCurrentNavigator%m_nNavigatorCnt;
					emit(logic_navigator_at(m_nCurrentNavigator));
				}
				else
					return false;
				
			}
			else//���Ҽ������л��ؼ�����ģʽ;
				emit(logic_focus_next_right());
		}
		break;
		case LOGIC_KEY_UP://��
		{
			if(m_bLogicKeyMod)
			{
				logic_navigator_mold(false);
			}
			emit(logic_focus_next_up());
		}
		break;
		case LOGIC_KEY_DOWN://��
		{
			if(m_bLogicKeyMod)
			{
				logic_navigator_mold(false);
			}
			emit(logic_focus_next_down());
		}
		break;
	}
	return false;
}


bool KLModule::logic_key_deal_mold1(QKeyEvent *ke)
{
	return false;
}

void KLModule::logic_set_navigator_current(int m_nCurrent)
{
	if(m_nCurrent >=0 && m_nCurrent < m_nNavigatorCnt)
	{
		if( m_nCurrentNavigator != m_nCurrent )
		{
			m_nCurrentNavigator = m_nCurrent;
			emit(logic_navigator_at(m_nCurrentNavigator));
		}
	}
		
}









