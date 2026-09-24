#include "ui/pages/gradienttablepage.h"
#include "core/appsettings.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/hmitablewidget.h"
#include "ui/widgets/pagescroll.h"
#include "ui/widgets/tableitemdelegate.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QVBoxLayout>

GradientTablePage::GradientTablePage(MachineController *c, MainWindow *main, int which,
                                     QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
    , m_which(which)
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);

    m_table = new HmiTableWidget;
    m_table->setColumnCount(2);
    m_table->setHorizontalHeaderLabels({tr("Time"), tr("Flow")});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setMinimumHeight(80);
    const bool tenMl = m_c->settings()->pumpType == 0;
    m_table->setItemDelegateForColumn(0, new TableItemDelegate(QStringLiteral("000.0")));
    m_table->setItemDelegateForColumn(1,
        new TableItemDelegate(tenMl ? QStringLiteral("0000.0000") : QStringLiteral("0000.000")));
    root->addWidget(m_table, 1);

    auto *btns = new QHBoxLayout;
    m_add = new QPushButton(tr("Add Row"));
    m_save = new QPushButton(tr("Save"));
    m_back = new QPushButton(tr("Back"));
    btns->addWidget(m_add);
    btns->addWidget(m_save);
    btns->addWidget(m_back);
    root->addLayout(btns);

    installPageScroll(this, inner);

    connect(m_add, SIGNAL(clicked()), this, SLOT(addRow()));
    connect(m_save, SIGNAL(clicked()), this, SLOT(saveTable()));
    connect(m_back, SIGNAL(clicked()), this, SLOT(onBack()));
    connect(m_table, SIGNAL(outOfTableFocus(int)), this, SLOT(onOutOfTableFocus(int)));
    connect(m_table, SIGNAL(panelShortcutsEnabled(bool)), m_main,
            SLOT(setPanelShortcutsEnabled(bool)));

    reload();
    m_table->initIndex();
}

void GradientTablePage::initFocusList()
{
    xList.append(m_table);
    xList.append(m_save);
    xList.append(m_back);
    yList.append(m_table);
    yList.append(m_save);
    yList.append(m_back);
}

void GradientTablePage::onOutOfTableFocus(int dir)
{
    if (dir == 0 || dir == 2)
        m_back->setFocus();
    else if (dir == 1 || dir == 3)
        m_save->setFocus();
}

void GradientTablePage::onBack()
{
    m_main->goBack();
}

void GradientTablePage::setWhich(int which)
{
    m_which = qBound(0, which, 9);
    reload();
}

void GradientTablePage::reload()
{
    loadTable();
    if (m_main)
        m_main->setPageTitle(tr("Gradient RG%1").arg(m_which + 1));
}

void GradientTablePage::loadTable()
{
    m_table->setRowCount(0);
    const auto &pts = m_c->settings()->gradientTable(m_which);
    for (const GradientPoint &p : pts)
    {
        const int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(p.timeMin, 'f', 1)));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::number(p.flow, 'f', 4)));
    }
}

void GradientTablePage::saveTable()
{
    QVector<GradientPoint> pts;
    for (int r = 0; r < m_table->rowCount(); ++r)
    {
        GradientPoint p;
        if (auto *t = m_table->item(r, 0))
            p.timeMin = t->text().toDouble();
        if (auto *f = m_table->item(r, 1))
            p.flow = f->text().toDouble();
        pts.append(p);
    }
    m_c->settings()->gradientTable(m_which) = pts;
    m_c->settings()->save();
    QMessageBox::information(this, tr("Tips"), tr("save success!"));
}

void GradientTablePage::addRow()
{
    const int row = m_table->rowCount();
    m_table->insertRow(row);
    m_table->setItem(row, 0, new QTableWidgetItem(QStringLiteral("0.0")));
    m_table->setItem(row, 1, new QTableWidgetItem(QStringLiteral("1.0")));
}
