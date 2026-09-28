#include "ui/pages/pressfixpage.h"
#include "core/machinecontroller.h"
#include "protocol/qinfinecodec.h"
#include "ui/mainwindow.h"
#include "ui/widgets/hmitablewidget.h"
#include "ui/widgets/pagescroll.h"
#include "ui/widgets/tableitemdelegate.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QVBoxLayout>

PressFixPage::PressFixPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *v = new QVBoxLayout(inner);
    m_table = new HmiTableWidget;
    m_table->setColumnCount(2);
    m_table->setHorizontalHeaderLabels({tr("adc"), tr("MPa")});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setMinimumHeight(80);
    m_table->setItemDelegateForColumn(0, new TableItemDelegate(QStringLiteral("000.0000")));
    m_table->setItemDelegateForColumn(1, new TableItemDelegate(QStringLiteral("000.0000")));
    v->addWidget(m_table, 1);
    auto *ops = new QHBoxLayout;
    m_add = new QPushButton(tr("+"));
    m_zero = new QPushButton(tr("Zero"));
    m_get = new QPushButton(tr("Get"));
    m_set = new QPushButton(tr("Set"));
    m_back = new QPushButton(tr("Back"));
    ops->addWidget(m_add);
    ops->addWidget(m_zero);
    ops->addWidget(m_get);
    ops->addWidget(m_set);
    ops->addWidget(m_back);
    v->addLayout(ops);
    m_press = new QLabel;
    v->addWidget(m_press);

    connect(m_add, SIGNAL(clicked()), this, SLOT(onAdd()));
    connect(m_zero, SIGNAL(clicked()), this, SLOT(onZero()));
    connect(m_get, SIGNAL(clicked()), this, SLOT(onGet()));
    connect(m_set, SIGNAL(clicked()), this, SLOT(onSet()));
    connect(m_back, SIGNAL(clicked()), this, SLOT(onBack()));
    connect(m_c, SIGNAL(tablesChanged()), this, SLOT(loadTable()));
    connect(m_c, SIGNAL(pressureChanged()), this, SLOT(onPressureChanged()));
    connect(m_table, SIGNAL(outOfTableFocus(int)), this, SLOT(onOutOfTableFocus(int)));
    connect(m_table, SIGNAL(panelShortcutsEnabled(bool)), m_main,
            SLOT(setPanelShortcutsEnabled(bool)));

    loadTable();
    m_table->initIndex();
    installPageScroll(this, inner);
}

void PressFixPage::initFocusList()
{
    xList.append(m_table);
    xList.append(m_add);
    xList.append(m_zero);
    xList.append(m_get);
    xList.append(m_set);
    xList.append(m_back);
    yList.append(m_table);
    yList.append(m_set);
    yList.append(m_add);
    yList.append(m_zero);
    yList.append(m_get);
    yList.append(m_back);
}

void PressFixPage::onOutOfTableFocus(int dir)
{
    if (dir == 0 || dir == 2)
        m_back->setFocus();
    else if (dir == 1)
        m_set->setFocus();
    else if (dir == 3)
        m_add->setFocus();
    loadTable();
}

void PressFixPage::onAdd()
{
    const int r = m_table->rowCount();
    m_table->insertRow(r);
    m_table->setItem(r, 0, new QTableWidgetItem(QStringLiteral("0")));
    m_table->setItem(r, 1, new QTableWidgetItem(QStringLiteral("0")));
}

void PressFixPage::onZero()
{
    m_c->pressZero();
}

void PressFixPage::onGet()
{
    m_c->requestPressTable();
}

void PressFixPage::onSet()
{
    QVector<PressPoint> t;
    for (int i = 0; i < m_table->rowCount(); ++i)
    {
        PressPoint p;
        p.adc = m_table->item(i, 0) ? m_table->item(i, 0)->text().toDouble() : 0;
        p.pressure = m_table->item(i, 1) ? m_table->item(i, 1)->text().toDouble() : 0;
        t.append(p);
    }
    m_c->setWorkMode(QinFine::WORK_PRESSCALIB, 1);
    m_c->writePressTable(t);
    m_c->setWorkMode(QinFine::WORK_PRESSCALIB, 0);
}

void PressFixPage::onBack()
{
    m_main->goBack();
}

void PressFixPage::onPressureChanged()
{
    m_press->setText(tr("Press %1 MPa").arg(m_c->pressure(), 0, 'f', 3));
}

void PressFixPage::loadTable()
{
    const auto t = m_c->pressTable();
    m_table->setRowCount(t.size());
    for (int i = 0; i < t.size(); ++i)
    {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(t[i].adc, 'f', 3)));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::number(t[i].pressure, 'f', 3)));
    }
}
