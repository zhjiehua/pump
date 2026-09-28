#include "ui/pages/debugmcuprotopage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/pagescroll.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

DebugMcuProtoPage::DebugMcuProtoPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    m_log = new QTextEdit;
    m_log->setReadOnly(true);
    root->addWidget(m_log, 1);

    auto *btns = new QHBoxLayout;
    m_reconnect = new QPushButton(tr("Reconnect MCU"));
    m_back = new QPushButton(tr("Back"));
    btns->addWidget(m_reconnect);
    btns->addWidget(m_back);
    root->addLayout(btns);

    installPageScroll(this, inner);

    connect(c, SIGNAL(logLine(QString)), this, SLOT(appendLog(QString)));
    connect(m_reconnect, SIGNAL(clicked()), this, SLOT(onReconnect()));
    connect(m_back, SIGNAL(clicked()), this, SLOT(onBack()));
}

void DebugMcuProtoPage::initFocusList()
{
    xList.append(m_log);
    xList.append(m_reconnect);
    xList.append(m_back);
    yList.append(m_log);
    yList.append(m_reconnect);
    yList.append(m_back);
}

void DebugMcuProtoPage::appendLog(const QString &line)
{
    m_log->append(line);
}

void DebugMcuProtoPage::onReconnect()
{
    m_c->connectMcu();
}

void DebugMcuProtoPage::onBack()
{
    m_main->goBack();
}
