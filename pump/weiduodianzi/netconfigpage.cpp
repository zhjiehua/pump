#include "netconfigpage.h"
#include <QDebug>
#include "baseMainPage.h"
#include <QApplication>
#include <QKeyEvent>


NetConfigPage::NetConfigPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("Network Configuration"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = tr("Network Configuration");
    setStyleSheet(BTNGLOBALSTYLE);

}

NetConfigPage::~NetConfigPage()
{

}








void NetConfigPage::initFocusList()
{
    xList.append(ui.ipEdit1);
    xList.append(ui.ipEdit2);
    xList.append(ui.ipEdit3);
    xList.append(ui.ipEdit4);
    xList.append(ui.portEdit);
    xList.append(ui.remote_ipEdit1);
    xList.append(ui.remote_ipEdit2);
    xList.append(ui.remote_ipEdit3);
    xList.append(ui.remote_ipEdit4);
    xList.append(ui.remote_portEdit);

    yList.append(ui.ipEdit1);
    yList.append(ui.portEdit);
    yList.append(ui.remote_ipEdit1);
    yList.append(ui.remote_portEdit);
    yList.append(ui.ipEdit2);
    yList.append(ui.remote_ipEdit2);
    yList.append(ui.ipEdit3);
    yList.append(ui.remote_ipEdit3);
    yList.append(ui.ipEdit4);
    yList.append(ui.remote_ipEdit4);

    ui.ipEdit1->setValRange("ip1", 0, 255, 0);
    ui.ipEdit2->setValRange("ip2", 0, 255, 0);
    ui.ipEdit3->setValRange("ip3", 0, 255, 0);
    ui.ipEdit4->setValRange("ip4", 0, 255, 0);
    ui.portEdit->setValRange("port", 0, 999999, 0);
    ui.remote_ipEdit1->setValRange("remote_ip1", 0, 255, 0);
    ui.remote_ipEdit2->setValRange("remote_ip2", 0, 255, 0);
    ui.remote_ipEdit3->setValRange("remote_ip3", 0, 255, 0);
    ui.remote_ipEdit4->setValRange("remote_ip4", 0, 255, 0);
    ui.remote_portEdit->setValRange("remote_port", 0, 999999, 0);

    //ÐÅºÅ²Û;
    connect(ui.ipEdit1, SIGNAL(dataChanging(QString)), this, SLOT(ipChanged()));
    connect(ui.ipEdit4, SIGNAL(dataChanging(QString)), this, SLOT(ipChanged()));
    connect(ui.ipEdit4, SIGNAL(dataChanging(QString)), this, SLOT(ipChanged()));
    connect(ui.ipEdit4, SIGNAL(dataChanging(QString)), this, SLOT(ipChanged()));

    connect(ui.portEdit, SIGNAL(dataChanging(QString)), this, SLOT(portChanged()));

    connect(ui.remote_ipEdit1, SIGNAL(dataChanging(QString)), this, SLOT(remoteipChanged()));
    connect(ui.remote_ipEdit2, SIGNAL(dataChanging(QString)), this, SLOT(remoteipChanged()));
    connect(ui.remote_ipEdit3, SIGNAL(dataChanging(QString)), this, SLOT(remoteipChanged()));
    connect(ui.remote_ipEdit4, SIGNAL(dataChanging(QString)), this, SLOT(remoteipChanged()));

    connect(ui.remote_portEdit, SIGNAL(dataChanging(QString)), this, SLOT(remoteportChanged()));
}

CBasePage* NetConfigPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new NetConfigPage(parent, index, previndex, add);
}

void NetConfigPage::ipChanged()
{
    MachineStat::getInstance()->setNetWorkConfig(MachineStat::E_LOCALIP);
}

void NetConfigPage::portChanged()
{
    MachineStat::getInstance()->setNetWorkConfig(MachineStat::E_LOCALPORT);
}

void NetConfigPage::remoteipChanged()
{
    MachineStat::getInstance()->setNetWorkConfig(MachineStat::E_REMOTEIP);
}

void NetConfigPage::remoteportChanged()
{
    MachineStat::getInstance()->setNetWorkConfig(MachineStat::E_REMOTEPORT);
}


