#include "ui/pages/pulsefixpage.h"
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

PulseFixPage::PulseFixPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *v = new QVBoxLayout(inner);
    auto *top = new QHBoxLayout;
    m_steps = new QLineEdit(QStringLiteral("128000"));
    m_div = new QLineEdit(QStringLiteral("32"));
    m_gen = new QPushButton(tr("Generate"));
    top->addWidget(new QLabel(tr("Steps")));
    top->addWidget(m_steps);
    top->addWidget(new QLabel(tr("Div")));
    top->addWidget(m_div);
    top->addWidget(m_gen);
    v->addLayout(top);
    m_table = new HmiTableWidget;
    m_table->setColumnCount(2);
    m_table->setHorizontalHeaderLabels({tr("pos"), tr("factor")});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setMinimumHeight(80);
    m_table->setItemDelegateForColumn(0, new TableItemDelegate(QStringLiteral("0000000")));
    m_table->setItemDelegateForColumn(1, new TableItemDelegate(QStringLiteral("0.0000")));
    v->addWidget(m_table, 1);
    auto *ops = new QHBoxLayout;
    m_get = new QPushButton(tr("Get"));
    m_set = new QPushButton(tr("Set"));
    m_save = new QPushButton(tr("Save"));
    m_clr = new QPushButton(tr("Clear"));
    m_back = new QPushButton(tr("Back"));
    ops->addWidget(m_get);
    ops->addWidget(m_set);
    ops->addWidget(m_save);
    ops->addWidget(m_clr);
    ops->addWidget(m_back);
    v->addLayout(ops);

    connect(m_gen, SIGNAL(clicked()), this, SLOT(onGenerate()));
    connect(m_get, SIGNAL(clicked()), this, SLOT(onGet()));
    connect(m_set, SIGNAL(clicked()), this, SLOT(onSet()));
    connect(m_save, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(m_clr, SIGNAL(clicked()), this, SLOT(onClear()));
    connect(m_back, SIGNAL(clicked()), this, SLOT(onBack()));
    connect(m_c, SIGNAL(tablesChanged()), this, SLOT(loadTable()));
    connect(m_table, SIGNAL(outOfTableFocus(int)), this, SLOT(onOutOfTableFocus(int)));
    connect(m_table, SIGNAL(panelShortcutsEnabled(bool)), m_main,
            SLOT(setPanelShortcutsEnabled(bool)));
    loadTable();
    m_table->initIndex();
    installPageScroll(this, inner);
}

void PulseFixPage::onOutOfTableFocus(int dir)
{
    if (dir == 0 || dir == 2)
        m_back->setFocus();
    else if (dir == 1)
        m_save->setFocus();
    else if (dir == 3)
        m_steps->setFocus();
}

void PulseFixPage::initFocusList()
{
    xList.append(m_steps);
    xList.append(m_div);
    xList.append(m_gen);
    xList.append(m_table);
    xList.append(m_get);
    xList.append(m_set);
    xList.append(m_save);
    xList.append(m_clr);
    xList.append(m_back);
    yList = xList;
}

void PulseFixPage::onGenerate()
{
    const int steps = m_steps->text().toInt();
    const int div = qMax(1, m_div->text().toInt());
    m_table->setRowCount(div);
    for (int i = 0; i < div; ++i)
    {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(i * (steps / div))));
        m_table->setItem(i, 1, new QTableWidgetItem(QStringLiteral("1.000")));
    }
}

void PulseFixPage::onGet()
{
    m_c->requestPulseTable();
}

void PulseFixPage::onSet()
{
    m_c->setWorkMode(QinFine::WORK_PULSECOMPEN, 1);
    m_c->writePulseTable(collectTable(), false);
    m_c->setWorkMode(QinFine::WORK_PULSECOMPEN, 0);
}

void PulseFixPage::onSave()
{
    m_c->writePulseTable(collectTable(), true);
}

void PulseFixPage::onClear()
{
    m_c->clearPulseTable();
}

void PulseFixPage::onBack()
{
    m_main->goBack();
}

QVector<PulsePoint> PulseFixPage::collectTable() const
{
    QVector<PulsePoint> t;
    for (int i = 0; i < m_table->rowCount(); ++i)
    {
        PulsePoint p;
        p.position = m_table->item(i, 0) ? m_table->item(i, 0)->text().toDouble() : 0;
        p.factor = m_table->item(i, 1) ? m_table->item(i, 1)->text().toDouble() : 1;
        t.append(p);
    }
    return t;
}

void PulseFixPage::loadTable()
{
    const auto t = m_c->pulseTable();
    m_table->setRowCount(t.size());
    for (int i = 0; i < t.size(); ++i)
    {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(t[i].position, 'f', 0)));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::number(t[i].factor, 'f', 4)));
    }
}
