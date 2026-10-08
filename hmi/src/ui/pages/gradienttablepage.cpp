#include "ui/pages/gradienttablepage.h"
#include "core/appsettings.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/hmitablewidget.h"
#include "ui/widgets/pagescroll.h"
#include "ui/widgets/tableitemdelegate.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QStringList>
#include <QVBoxLayout>

GradientTablePage::GradientTablePage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);

    m_table = new HmiTableWidget;
    m_table->setDataColumnCount(2);
    m_table->setDataHeaders({tr("Time"), tr("Flow")});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setMinimumHeight(80);
    const bool tenMl = m_c->settings()->pumpType == 0;
    m_table->setDataDelegate(0, new TableItemDelegate(QStringLiteral("000.0")));
    m_table->setDataDelegate(1,
        new TableItemDelegate(tenMl ? QStringLiteral("0000.0000") : QStringLiteral("0000.000")));
    root->addWidget(m_table, 1);

    auto *btns = new QHBoxLayout;
    m_save = new BtnCtrl;
    m_back = new BtnCtrl;
    btns->addWidget(m_save);
    btns->addWidget(m_back);
    root->addLayout(btns);

    installPageScroll(this, inner);
    retranslateUi();

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

void GradientTablePage::retranslateUi()
{
    m_table->setDataHeaders({tr("Time"), tr("Flow")});
    m_save->setText(tr("Save"));
    m_back->setText(tr("Back"));
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

void GradientTablePage::reload()
{
    loadTable();
}

void GradientTablePage::loadTable()
{
    QVector<QStringList> rows;
    const auto &pts = m_c->settings()->gradientTable();
    for (const GradientPoint &p : pts)
        rows.append({QString::number(p.timeMin, 'f', 1), QString::number(p.flow, 'f', 4)});
    m_table->setFilledRowTexts(rows);
}

void GradientTablePage::saveTable()
{
    QVector<GradientPoint> pts;
    for (int r = 0; r < m_table->rowCount(); ++r)
    {
        if (m_table->isDataRowEmpty(r))
            continue;
        GradientPoint p;
        p.timeMin = m_table->dataText(r, 0).toDouble();
        p.flow = m_table->dataText(r, 1).toDouble();
        pts.append(p);
    }
    m_c->settings()->gradientTable() = pts;
    m_c->settings()->save();
    m_c->gradient()->reload();
    QMessageBox::information(this, tr("Tips"), tr("save success!"));
}
