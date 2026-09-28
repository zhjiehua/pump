#include "ui/pages/runparampage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

RunParamPage::RunParamPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);

    auto *form = new QFormLayout;
    m_min = new QLineEdit;
    m_max = new QLineEdit;
    m_coeff = new QLineEdit;
    m_gradient = new QComboBox;
    for (int i = 1; i <= 10; ++i)
        m_gradient->addItem(QStringLiteral("RG%1").arg(i), i - 1);

    form->addRow(tr("Pmin (MPa)"), m_min);
    form->addRow(tr("Pmax (MPa)"), m_max);
    form->addRow(tr("Coefficient (%)"), m_coeff);
    form->addRow(tr("Gradient"), m_gradient);
    root->addLayout(form);

    auto *btns = new QHBoxLayout;
    m_save = new QPushButton(tr("Save"));
    m_grad = new QPushButton(tr("Grad"));
    m_back = new QPushButton(tr("Back"));
    btns->addWidget(m_save);
    btns->addWidget(m_grad);
    btns->addWidget(m_back);
    root->addLayout(btns);
    root->addStretch(1);

    loadFromSettings();

    connect(m_save, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(m_grad, SIGNAL(clicked()), this, SLOT(onGrad()));
    connect(m_back, SIGNAL(clicked()), this, SLOT(onBack()));
}

void RunParamPage::initFocusList()
{
    xList.append(m_max);
    xList.append(m_min);
    xList.append(m_gradient);
    xList.append(m_coeff);
    xList.append(m_save);
    xList.append(m_grad);
    xList.append(m_back);
    yList.append(m_max);
    yList.append(m_min);
    yList.append(m_gradient);
    yList.append(m_coeff);
    yList.append(m_save);
    yList.append(m_grad);
    yList.append(m_back);
}

void RunParamPage::onSave()
{
    auto *s = m_c->settings();
    const double pmin = m_min->text().toDouble();
    double pmax = m_max->text().toDouble();
    const double cap = effectivePmaxCap();
    if (cap > 0 && pmax > cap)
        pmax = cap;
    s->pressMin = pmin;
    s->pressMax = pmax;
    s->coefficient = m_coeff->text().toDouble();
    s->percent = s->coefficient;
    s->gradientIndex = m_gradient->itemData(m_gradient->currentIndex()).toInt();
    m_c->setPressLimits(pmin, pmax);
    m_c->setPercent(s->coefficient);
    s->save();
    m_c->postLog(tr("Run parameters saved"));
}

void RunParamPage::onGrad()
{
    m_main->go(MainWindow::Gradient);
}

void RunParamPage::onBack()
{
    m_main->goBack();
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
    m_min->setText(QString::number(s->pressMin, 'f', 2));
    m_max->setText(QString::number(s->pressMax, 'f', 2));
    m_coeff->setText(QString::number(s->coefficient, 'f', 0));
    m_gradient->setCurrentIndex(qBound(0, s->gradientIndex, 9));
}
