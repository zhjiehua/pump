#include "msgpage.h"
#include <QDebug>
#include "Common.h"
#include "baseMainPage.h"
#include "msgbox.h"
#include <QFile>

#define VERSION					"V1.05"

#ifdef WIN32
#define SRCFILE			"H:/weiduodianzi.txt"
#define TARGETFILE		"K:/weiduodianzi.txt"

#define SRCDBFILE		"H:/wda.txt"
#define TARGETDBFILE	"K:/wda.txt"
#else
#define SRCFILE			"/sdcard/sepuyi"
#define TARGETFILE		"/bin/weiduodianzi"

#define SRCDBFILE		"/sdcard/wda.db"
#define TARGETDBFILE	"/bin/wda.db"
#endif

MsgPage::MsgPage(QWidget *parent /*= 0*/, quint8 index, quint8 previndex, quint32 add)
	: CBasePage(tr("About"), index, previndex, add, parent )
{
	ui.setupUi(this);
	m_strTitle = tr("About");
	ui.msgBtn->setBkImage(":/weiduodianzi/ui/message.png");
	ui.labelVersionx->setText(VERSION);
	ui.labelLicensex->setText(DataBase::getInstance()->queryData("license"));
	ui.labelSerialx->setText(DataBase::getInstance()->queryData("serial"));

	ui.saveDataBtn->hide();
	ui.updateDataBtn->hide();

	setStyleSheet("QPushButton:focus{border:2px solid blue;outline:0px;}");

	//connect(ui.saveDataBtn, SIGNAL(clicked()), this, SLOT(saveDataBtn_clicked()));
	//connect(ui.updateDataBtn, SIGNAL(clicked()), this, SLOT(updateDataBtn_clicked()));
}

MsgPage::~MsgPage()
{

}

void MsgPage::initFocusList()
{
	//ui.updateBtn->setFocus();
	//xList.append(ui.saveDataBtn);
	//xList.append(ui.updateDataBtn);
	xList.append(ui.updateBtn);

	//yList.append(ui.saveDataBtn);
	//yList.append(ui.updateDataBtn);
	yList.append(ui.updateBtn);
}

CBasePage* MsgPage::getInstance(QWidget *parent, quint8 index, quint8 previndex, quint32 add)
{
	return new MsgPage(parent, index, previndex, add);
}

//更新程序
void MsgPage::on_updateBtn_clicked()
{
	MsgBox msgBox(this, tr("Tips"), tr("Comfirm to update program?"));
	if(msgBox.exec() != QMessageBox::Ok)
		return;

	//qDebug() << "on_updateBtn_clicked()";
	QFile file(SRCFILE);
	if(!file.exists())
	{
		MsgBox msgBox(this,tr("Tips"), tr("file not found!"));
		msgBox.exec();
		return;
	}

	QFile target(TARGETFILE);
	if(target.exists())
		qDebug()<<target.remove();
	if(QFile::copy(SRCFILE, TARGETFILE) )
	{
		MsgBox msgBox(this,tr("Tips"), tr("success!"));
		msgBox.exec();
	}
	else
	{
		MsgBox msgBox(this,tr("Tips"), tr("failed!"));
		msgBox.exec();
	}

}

//保存数据
void MsgPage::on_saveDataBtn_clicked()
{
	//qDebug() << "on_saveDataBtn_clicked()";
	QFile file(TARGETDBFILE);
	if(!file.exists())
	{
		MsgBox msgBox(this,tr("Tips"), tr("file not found!"));
		msgBox.exec();
		return;
	}

	QFile target(SRCDBFILE);
	if(target.exists())
		qDebug()<<target.remove();
	if(QFile::copy(TARGETDBFILE, SRCDBFILE) )
	{
		MsgBox msgBox(this,tr("Tips"), tr("success!"));
		msgBox.exec();
	}
	else
	{
		MsgBox msgBox(this,tr("Tips"), tr("failed!"));
		msgBox.exec();
	}
}

//更新数据
void MsgPage::on_updateDataBtn_clicked()
{
	MsgBox msgBox(this, tr("Tips"), tr("Comfirm to update data?"));
	if(msgBox.exec() != QMessageBox::Ok)
		return;

	//qDebug() << "on_updateDataBtn_clicked()";
	QFile file(SRCDBFILE);
	if(!file.exists())
	{
		MsgBox msgBox(this,tr("Tips"), tr("file not found!"));
		msgBox.exec();
		return;
	}

	QFile target(TARGETDBFILE);
	if(target.exists())
		qDebug()<<target.remove();
	if(QFile::copy(SRCDBFILE, TARGETDBFILE))
	{
		MsgBox msgBox(this,tr("Tips"), tr("success!"));
		msgBox.exec();
	}
	else
	{
		MsgBox msgBox(this,tr("Tips"), tr("failed!"));
		msgBox.exec();
	}
}
