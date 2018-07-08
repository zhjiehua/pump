#ifndef NETCONFIGPAGE_H
#define NETCONFIGPAGE_H

#include <BasePage.h>
#include "ui_netconfigpage.h"

class NetConfigPage : public CBasePage
{
	Q_OBJECT

public:
	NetConfigPage(QWidget *parent = 0, quint8 index = 0, quint8 previndex = 0, quint32 add = 0);
	~NetConfigPage();
	static CBasePage* getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add);

protected:
	virtual void initFocusList();

private:
	Ui::NetConfigPageClass ui;
	
private slots:
    void ipChanged();
    void portChanged();
    void remoteipChanged();
    void remoteportChanged();
};

#endif // NETCONFIGPAGE_H
