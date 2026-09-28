#include "ui/pages/flowfixpage.h"
#include "core/machinecontroller.h"
#include "protocol/qinfinecodec.h"
#include "ui/mainwindow.h"
#include "ui/widgets/hmitablewidget.h"
#include "ui/widgets/pagescroll.h"
#include "ui/widgets/tableitemdelegate.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QVBoxLayout>

FlowFixPage::FlowFixPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *v = new QVBoxLayout(inner);
    v->setContentsMargins(4, 4, 4, 4);
    m_table = new HmiTableWidget;
    m_table->setColumnCount(2);
    m_table->setHorizontalHeaderLabels({tr("rpm"), tr("rate")});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setMinimumHeight(80);
    m_table->setItemDelegateForColumn(0, new TableItemDelegate(QStringLiteral("0000.0000")));
    m_table->setItemDelegateForColumn(1, new TableItemDelegate(QStringLiteral("000.0000")));
    v->addWidget(m_table, 1);
    auto *run = new QHBoxLayout;
    m_flow = new QLineEdit(QStringLiteral("1.000"));
    run->addWidget(new QLabel(tr("Flow")));
    run->addWidget(m_flow);
    m_start = new QPushButton(tr("Start"));
    m_stop = new QPushButton(tr("Stop"));
    run->addWidget(m_start);
    run->addWidget(m_stop);
    v->addLayout(run);
    auto *ops = new QHBoxLayout;
    m_add = new QPushButton(tr("+"));
    m_get = new QPushButton(tr("Get"));
    m_set = new QPushButton(tr("Set"));
    m_back = new QPushButton(tr("Back"));
    ops->addWidget(m_add);
    ops->addWidget(m_get);
    ops->addWidget(m_set);
    ops->addWidget(m_back);
    v->addLayout(ops);

    connect(m_add, SIGNAL(clicked()), this, SLOT(onAdd()));
    connect(m_get, SIGNAL(clicked()), this, SLOT(onGet()));
    connect(m_set, SIGNAL(clicked()), this, SLOT(onSet()));
    connect(m_start, SIGNAL(clicked()), this, SLOT(onStart()));
    connect(m_stop, SIGNAL(clicked()), this, SLOT(onStop()));
    connect(m_back, SIGNAL(clicked()), this, SLOT(onBack()));
    connect(m_c, SIGNAL(tablesChanged()), this, SLOT(loadTable()));
    connect(m_table, SIGNAL(outOfTableFocus(int)), this, SLOT(onOutOfTableFocus(int)));
    connect(m_table, SIGNAL(panelShortcutsEnabled(bool)), m_main,
            SLOT(setPanelShortcutsEnabled(bool)));
    loadTable();
    m_table->initIndex();
    installPageScroll(this, inner);
}

void FlowFixPage::initFocusList()
{
    xList.append(m_table);
    xList.append(m_flow);
    xList.append(m_start);
    xList.append(m_add);
    xList.append(m_get);
    xList.append(m_set);
    xList.append(m_back);
    yList.append(m_table);
    yList.append(m_set);
    yList.append(m_flow);
    yList.append(m_start);
    yList.append(m_add);
    yList.append(m_get);
    yList.append(m_back);
}

void FlowFixPage::onOutOfTableFocus(int dir)
{
    if (dir == 0 || dir == 2)
        m_back->setFocus();
    else if (dir == 1)
        m_set->setFocus();
    else if (dir == 3)
        m_flow->setFocus();
    loadTable();
}

void FlowFixPage::onAdd()
{
    const int r = m_table->rowCount();
    m_table->insertRow(r);
    m_table->setItem(r, 0, new QTableWidgetItem(QStringLiteral("0")));
    m_table->setItem(r, 1, new QTableWidgetItem(QStringLiteral("0")));
}

void FlowFixPage::onGet()
{
    m_c->requestFlowTable();
}

void FlowFixPage::onSet()
{
    m_c->setWorkMode(QinFine::WORK_FLOWCALIB, 1);
    fromUi();
    m_c->setWorkMode(QinFine::WORK_FLOWCALIB, 0);
}

void FlowFixPage::onStart()
{
    m_c->setFlow(m_flow->text().toDouble());
    m_c->start();
}

void FlowFixPage::onStop()
{
    m_c->stop();
}

void FlowFixPage::onBack()
{
    m_main->goBack();
}

void FlowFixPage::loadTable()
{
    const auto t = m_c->flowTable();
    m_table->setRowCount(t.size());
    for (int i = 0; i < t.size(); ++i)
    {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(t[i].rpm, 'f', 3)));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::number(t[i].rate, 'f', 3)));
    }
}

void FlowFixPage::fromUi()
{
    QVector<RatePoint> t;
    for (int i = 0; i < m_table->rowCount(); ++i)
    {
        RatePoint p;
        p.rpm = m_table->item(i, 0) ? m_table->item(i, 0)->text().toDouble() : 0;
        p.rate = m_table->item(i, 1) ? m_table->item(i, 1)->text().toDouble() : 0;
        t.append(p);
    }
    m_c->writeFlowTable(t);
}
