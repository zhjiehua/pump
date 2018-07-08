#pragma once
#ifndef	MYKEYHANDLER
#define MYKEYHANDLER
#ifndef QT_NO_QWS_KBD_MY
#include <QtGui/qkbd_qws.h>
#include <QSocketNotifier>

//dev data
struct key_action_data{
	unsigned char bPress;
	unsigned char keycode;
};

class MyKeyHandler :public QObject,public QWSKeyboardHandler 
{
	Q_OBJECT 
public:
	MyKeyHandler(const QString &device=QString("/dev/buttons"));
	~MyKeyHandler(void);

	static void stopMotor();
	static void turnOnWarning(bool on);
private: 
	QSocketNotifier *notify; 
	static int kbFd; 
	struct key_action_data keyData[1];
private slots: 
		void readData(); 
};

#endif// QT_NO_QWS_KBD_MY
#endif

