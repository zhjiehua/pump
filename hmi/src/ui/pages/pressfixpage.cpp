#include "ui/pages/pressfixpage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/hmitablewidget.h"
#include "ui/widgets/pagescroll.h"
#include "ui/widgets/tableitemdelegate.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QHideEvent>
#include <QLabel>
#include <QMessageBox>
#include <QShowEvent>
#include <QSizePolicy>
#include <QStringList>
#include <QVBoxLayout>

namespace {

void expand(QWidget *w)
{
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QHBoxLayout *labeledRow(QLabel **nameLab, QWidget *value, QLabel **unitLab)
{
    auto *h = new QHBoxLayout;
    *nameLab = new QLabel;
    expand(*nameLab);
    expand(value);
    *unitLab = new QLabel;
    expand(*unitLab);
    h->addWidget(*nameLab, 2);
    h->addWidget(value, 5);
    h->addWidget(*unitLab, 2);
    return h;
}

} // namespace

PressFixPage::PressFixPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);
    root->setSpacing(6);

    auto *top = new QHBoxLayout;
    m_table = new HmiTableWidget;
    m_table->setDataColumnCount(2);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->setMinimumHeight(80);
    m_table->setDataDelegate(0, new TableItemDelegate(QStringLiteral("000.0000")));
    m_table->setDataDelegate(1, new TableItemDelegate(QStringLiteral("000.0000")));
    top->addWidget(m_table, 6);

    auto *right = new QVBoxLayout;
    right->setSpacing(6);

    m_flow = new EditCtrl;
    connect(m_flow, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    right->addLayout(labeledRow(&m_flowLabel, m_flow, &m_flowUnit), 1);
    right->addStretch(1);

    m_press = new QLabel(QStringLiteral("0"));
    right->addLayout(labeledRow(&m_pressLabel, m_press, &m_pressUnit), 1);
    right->addStretch(1);

    m_clear = new BtnCtrl;
    expand(m_clear);
    right->addWidget(m_clear, 1);
    right->addStretch(1);

    m_start = new BtnCtrl;
    expand(m_start);
    right->addWidget(m_start, 1);

    top->addLayout(right, 5);
    root->addLayout(top, 7);

    auto *bottom = new QHBoxLayout;
    m_save = new BtnCtrl;
    m_back = new BtnCtrl;
    expand(m_save);
    expand(m_back);
    bottom->addWidget(m_save, 1);
    bottom->addWidget(m_back, 1);
    root->addLayout(bottom, 1);

    installPageScroll(this, inner);
    applyFlowRange();
    m_flow->setText(QString::number(m_c->flow(), 'f', m_c->settings()->pumpType == 0 ? 4 : 3));
    retranslateUi();

    connect(m_clear, SIGNAL(clicked()), this, SLOT(onClearPress()));
    connect(m_start, SIGNAL(clicked()), this, SLOT(onStart()));
    connect(m_save, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(m_back, SIGNAL(clicked()), this, SLOT(onBack()));
    connect(m_c, SIGNAL(tablesChanged()), this, SLOT(loadTable()));
    connect(m_c, SIGNAL(pressureChanged()), this, SLOT(onPressureChanged()));
    connect(m_table, SIGNAL(outOfTableFocus(int)), this, SLOT(onOutOfTableFocus(int)));
    connect(m_table, SIGNAL(panelShortcutsEnabled(bool)), m_main,
            SLOT(setPanelShortcutsEnabled(bool)));

    m_table->setEditAuthPage(m_main, int(MainWindow::PressFix));
    loadTable();
    m_table->initIndex();
    onPressureChanged();
}

PressFixPage::~PressFixPage()
{
    stopRun();
    m_c->setPressCalibActive(false);
}

void PressFixPage::initFocusList()
{
    xList.append(m_table);
    xList.append(m_flow);
    xList.append(m_clear);
    xList.append(m_start);
    xList.append(m_save);
    xList.append(m_back);

    yList.append(m_table);
    yList.append(m_save);
    yList.append(m_flow);
    yList.append(m_clear);
    yList.append(m_start);
    yList.append(m_back);
    m_table->refreshEditAuth();
}

void PressFixPage::retranslateUi()
{
    m_table->setDataHeaders({tr("Press"), tr("Real Press")});
    m_flowLabel->setText(tr("Flow:"));
    m_flowUnit->setText(tr("mL"));
    m_pressLabel->setText(tr("Press:"));
    m_pressUnit->setText(tr("MPa"));
    m_clear->setText(tr("Clear Press"));
    refreshStartText();
    m_save->setText(tr("Save"));
    m_back->setText(tr("Back"));
}

void PressFixPage::showEvent(QShowEvent *event)
{
    FocusPage::showEvent(event);
    m_c->setPressCalibActive(true);
    applyFlowRange();
    loadTable();
    m_c->requestPressTable();
    onPressureChanged();
}

void PressFixPage::hideEvent(QHideEvent *event)
{
    stopRun();
    m_c->setPressCalibActive(false);
    FocusPage::hideEvent(event);
}

void PressFixPage::applyFlowRange()
{
    const double maxFlow = m_c->settings()->defaultMaxFlowForPump();
    const quint8 decimals = (m_c->settings()->pumpType == 0) ? 4 : 3;
    m_flow->setValRange(0, maxFlow, decimals);
}

void PressFixPage::refreshStartText()
{
    m_start->setText(m_running ? tr("Stop") : tr("Start"));
}

void PressFixPage::stopRun()
{
    if (!m_running)
        return;
    m_running = false;
    refreshStartText();
    m_c->stop();
}

void PressFixPage::onStart()
{
    if (m_running)
    {
        stopRun();
        return;
    }
    m_c->setFlow(m_flow->text().toDouble());
    m_c->start();
    m_running = true;
    refreshStartText();
}

void PressFixPage::onClearPress()
{
    m_c->pressZero();
}

void PressFixPage::onSave()
{
    m_table->commitActiveEditor();
    m_c->writePressTable(collectTable());
    QMessageBox::information(this, tr("Tips"), tr("save success!"));
}

void PressFixPage::onBack()
{
    m_main->goBack();
}

void PressFixPage::onPressureChanged()
{
    m_press->setText(QString::number(m_c->pressure(), 'f', 2));
}

void PressFixPage::onOutOfTableFocus(int dir)
{
    if (dir == 0 || dir == 2)
        m_back->setFocus();
    else if (dir == 1)
        m_save->setFocus();
    else if (dir == 3)
        m_flow->setFocus();
}

void PressFixPage::loadTable()
{
    QVector<QStringList> rows;
    const auto t = m_c->pressTable();
    for (int i = 0; i < t.size(); ++i)
        rows.append({QString::number(t[i].adc, 'f', 4),
                     QString::number(t[i].pressure, 'f', 4)});
    m_table->setFilledRowTexts(rows);
}

QVector<PressPoint> PressFixPage::collectTable() const
{
    QVector<PressPoint> t;
    for (int i = 0; i < m_table->rowCount(); ++i)
    {
        if (m_table->isDataRowEmpty(i))
            continue;
        PressPoint p;
        p.adc = m_table->dataText(i, 0).toDouble();
        p.pressure = m_table->dataText(i, 1).toDouble();
        t.append(p);
    }
    return t;
}
