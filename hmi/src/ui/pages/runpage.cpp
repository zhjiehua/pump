#include "ui/pages/runpage.h"
#include "core/machinecontroller.h"
#include "domain/pumpsession.h"
#include "ui/mainwindow.h"
#include "ui/widgets/comboctrl.h"
#include "ui/widgets/editctrl.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

RunPage::RunPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *outer = new QHBoxLayout(this);
    outer->setContentsMargins(8, 4, 8, 4);
    outer->addStretch(1);

    auto *col = new QVBoxLayout;
    col->setSpacing(4);
    col->addStretch(1);

    auto makeRow = [&](QWidget *w) { col->addWidget(w, 1); };

    {
        auto *row = new QWidget;
        auto *h = new QHBoxLayout(row);
        h->setContentsMargins(0, 0, 0, 0);
        auto *lab = new QLabel(tr("Flow:"));
        QFont bf = lab->font();
        bf.setBold(true);
        lab->setFont(bf);
        m_flow = new EditCtrl;
        m_flow->setFont(bf);
        m_flow->setValRange(0, 10, 3);
        connect(m_flow, SIGNAL(valueCommitted(QString)), this, SLOT(onFlowCommitted(QString)));
        connect(m_flow, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
        auto *unit = new QLabel(tr("mL/min"));
        m_percent = new QLabel(QStringLiteral("100%"));
        m_percent->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        h->addWidget(lab, 1);
        h->addWidget(m_flow, 2);
        h->addWidget(unit, 2);
        h->addWidget(m_percent, 1);
        makeRow(row);
    }
    col->addStretch(1);
    {
        auto *row = new QWidget;
        auto *h = new QHBoxLayout(row);
        h->setContentsMargins(0, 0, 0, 0);
        auto *lab = new QLabel(tr("Press:"));
        QFont bf = lab->font();
        bf.setBold(true);
        lab->setFont(bf);
        m_press = new QLabel(QStringLiteral("0"));
        m_press->setFont(bf);
        auto *unit = new QLabel(tr("MPa"));
        h->addWidget(lab, 1);
        h->addWidget(m_press, 2);
        h->addWidget(unit, 1);
        h->addStretch(2);
        makeRow(row);
    }
    col->addStretch(1);
    {
        auto *row = new QWidget;
        auto *h = new QHBoxLayout(row);
        h->setContentsMargins(0, 0, 0, 0);
        h->addWidget(new QLabel(tr("State:")), 1);
        m_stat = new ComboCtrl;
        m_stat->addItems({tr("Stop"), tr("Pause"), tr("Runnning"), tr("Pump"), tr("Purge"), tr("PC")});
        connect(m_stat, SIGNAL(popupChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
        h->addWidget(m_stat, 2);
        h->addStretch(3);
        makeRow(row);
    }
    col->addStretch(1);
    {
        auto *row = new QWidget;
        auto *h = new QHBoxLayout(row);
        h->setContentsMargins(0, 0, 0, 0);
        h->addWidget(new QLabel(tr("Time:")), 1);
        m_time = new QLabel(QStringLiteral("00:00:00"));
        h->addWidget(m_time, 2);
        h->addStretch(3);
        makeRow(row);
    }
    col->addStretch(1);

    outer->addLayout(col, 15);
    outer->addStretch(1);

    connect(m_stat, SIGNAL(activated(int)), this, SLOT(onStatActivated(int)));
    connect(m_c->session(), SIGNAL(snapshotChanged()), this, SLOT(refresh()));
    connect(m_c->session(), SIGNAL(pressureChanged()), this, SLOT(refresh()));

    refresh();
}

void RunPage::initFocusList()
{
    xList.append(m_flow);
    xList.append(m_stat);
    yList.append(m_flow);
    yList.append(m_stat);
}

void RunPage::onFlowCommitted(const QString &value)
{
    m_c->setFlow(value.toDouble());
}

void RunPage::onStatActivated(int i)
{
    if (m_c->authority()->isRemote())
    {
        refresh();
        return;
    }
    if (i == 5)
        m_c->enterPcControl();
    else
        m_c->setStat(MachineController::Stat(i));
}

void RunPage::updateTimeLabel(quint32 sec)
{
    const int h = int(sec / 3600);
    const int m = int((sec % 3600) / 60);
    const int s = int(sec % 60);
    m_time->setText(QStringLiteral("%1:%2:%3")
                        .arg(h, 2, 10, QChar('0'))
                        .arg(m, 2, 10, QChar('0'))
                        .arg(s, 2, 10, QChar('0')));
}

void RunPage::refresh()
{
    const PumpSession::Snap snap = m_c->session()->copy();
    if (!m_flow->hasFocus())
        m_flow->setText(QString::number(snap.flow, 'f', 3));
    m_percent->setText(QString::number(snap.percent, 'f', 0) + QStringLiteral("%"));
    m_press->setText(QString::number(snap.pressure, 'f', 2));
    updateTimeLabel(snap.runSeconds);
    if (snap.stat >= 0 && snap.stat <= 5)
        m_stat->setCurrentIndex(snap.stat);
}
