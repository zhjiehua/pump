#include "ui/pages/runparampage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/comboctrl.h"
#include "ui/widgets/editctrl.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

RunParamPage::RunParamPage(MachineController *c, MainWindow *main, QWidget *parent)
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
        h->addWidget(new QLabel(tr("Max Press:")), 2);
        m_max = new EditCtrl;
        connect(m_max, SIGNAL(valueCommitted(QString)), this, SLOT(onMaxCommitted(QString)));
        connect(m_max, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
        h->addWidget(m_max, 2);
        h->addWidget(new QLabel(tr("MPa")), 5);
        makeRow(row);
    }
    col->addStretch(1);
    {
        auto *row = new QWidget;
        auto *h = new QHBoxLayout(row);
        h->setContentsMargins(0, 0, 0, 0);
        h->addWidget(new QLabel(tr("Min Press:")), 2);
        m_min = new EditCtrl;
        connect(m_min, SIGNAL(valueCommitted(QString)), this, SLOT(onMinCommitted(QString)));
        connect(m_min, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
        h->addWidget(m_min, 2);
        h->addWidget(new QLabel(tr("MPa")), 5);
        makeRow(row);
    }
    col->addStretch(1);
    {
        auto *row = new QWidget;
        auto *h = new QHBoxLayout(row);
        h->setContentsMargins(0, 0, 0, 0);
        h->addWidget(new QLabel(tr("Grad Mode:")), 2);
        m_gradient = new ComboCtrl;
        m_gradient->addItem(tr("high"), 0);
        m_gradient->addItem(tr("low"), 1);
        connect(m_gradient, SIGNAL(activated(int)), this, SLOT(onGradientActivated(int)));
        connect(m_gradient, SIGNAL(popupChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
        h->addWidget(m_gradient, 2);
        h->addStretch(5);
        makeRow(row);
    }
    col->addStretch(1);
    {
        auto *row = new QWidget;
        auto *h = new QHBoxLayout(row);
        h->setContentsMargins(0, 0, 0, 0);
        h->addWidget(new QLabel(tr("Comp Coef:")), 2);
        m_coeff = new EditCtrl;
        connect(m_coeff, SIGNAL(valueCommitted(QString)), this, SLOT(onCoeffCommitted(QString)));
        connect(m_coeff, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
        h->addWidget(m_coeff, 2);
        h->addStretch(5);
        makeRow(row);
    }
    col->addStretch(1);

    outer->addLayout(col, 15);
    outer->addStretch(1);

    connect(m_c->session(), SIGNAL(snapshotChanged()), this, SLOT(refresh()));
    loadFromSettings();
}

void RunParamPage::initFocusList()
{
    xList.append(m_max);
    xList.append(m_min);
    xList.append(m_gradient);
    xList.append(m_coeff);
    yList.append(m_max);
    yList.append(m_min);
    yList.append(m_gradient);
    yList.append(m_coeff);
    loadFromSettings();
}

bool RunParamPage::editorsBusy() const
{
    return m_max->isEditing() || m_min->isEditing() || m_coeff->isEditing()
        || m_gradient->isPopupOpen();
}

void RunParamPage::refresh()
{
    if (editorsBusy())
        return;
    loadFromSettings();
}

void RunParamPage::onMaxCommitted(const QString &value)
{
    applyPressLimits(m_min->text().toDouble(), value.toDouble());
}

void RunParamPage::onMinCommitted(const QString &value)
{
    applyPressLimits(value.toDouble(), m_max->text().toDouble());
}

void RunParamPage::onCoeffCommitted(const QString &value)
{
    auto *s = m_c->settings();
    s->coefficient = value.toDouble();
    s->save();
}

void RunParamPage::onGradientActivated(int index)
{
    auto *s = m_c->settings();
    s->gradientMode = m_gradient->itemData(index).toInt();
    s->save();
}

void RunParamPage::applyPressLimits(double pmin, double pmax)
{
    const double cap = effectivePmaxCap();
    if (cap > 0 && pmax > cap)
        pmax = cap;
    m_c->setPressLimits(pmin, pmax);
    m_c->settings()->save();
}

double RunParamPage::effectivePmaxCap() const
{
    auto *s = m_c->settings();
    if (s->pmaxLimit > 0)
        return s->pmaxLimit;
    return s->defaultMaxPressForPump();
}

void RunParamPage::loadFromSettings()
{
    auto *s = m_c->settings();
    const double pumpMax = s->defaultMaxPressForPump();
    const double cap = effectivePmaxCap();
    m_min->setValRange(-50, pumpMax, 2);
    m_max->setValRange(0, cap, 2);
    m_coeff->setValRange(0, 100, 0);
    m_max->setText(QString::number(s->pressMax, 'f', 2));
    m_min->setText(QString::number(s->pressMin, 'f', 2));
    m_coeff->setText(QString::number(s->coefficient, 'f', 0));
    m_gradient->setCurrentIndex(qBound(0, s->gradientMode, 1));
}
