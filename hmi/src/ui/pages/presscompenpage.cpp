#include "ui/pages/presscompenpage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/pagescroll.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

PressCompenPage::PressCompenPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *v = new QVBoxLayout(inner);
    m_en = new QCheckBox(tr("Enable pressure compensation"));
    v->addWidget(m_en);
    auto row = [&](const QString &n, QLineEdit **e) {
        auto *h = new QHBoxLayout;
        h->addWidget(new QLabel(n));
        *e = new QLineEdit;
        h->addWidget(*e, 1);
        v->addLayout(h);
    };
    row(tr("Rate"), &m_rate);
    row(tr("RealRate"), &m_real);
    row(tr("Pressure"), &m_press);
    auto *h = new QHBoxLayout;
    m_set = new QPushButton(tr("Set"));
    m_back = new QPushButton(tr("Back"));
    h->addWidget(m_set);
    h->addWidget(m_back);
    v->addLayout(h);
    v->addStretch(1);

    installPageScroll(this, inner);

    fillTable();
    connect(m_c, SIGNAL(tablesChanged()), this, SLOT(fillTable()));
    connect(m_set, SIGNAL(clicked()), this, SLOT(onSet()));
    connect(m_back, SIGNAL(clicked()), this, SLOT(onBack()));
}

void PressCompenPage::initFocusList()
{
    xList.append(m_en);
    xList.append(m_rate);
    xList.append(m_real);
    xList.append(m_press);
    xList.append(m_set);
    xList.append(m_back);
    yList = xList;
}

void PressCompenPage::fillTable()
{
    m_en->setChecked(m_c->pressCompen() != 0);
    m_rate->setText(QString::number(m_c->loadRate()));
    m_real->setText(QString::number(m_c->loadReal()));
    m_press->setText(QString::number(m_c->loadPress()));
}

void PressCompenPage::onSet()
{
    m_c->setPressCompen(m_en->isChecked() ? 1 : 0);
    m_c->setLoadParams(m_rate->text().toDouble(), m_real->text().toDouble(), m_press->text().toDouble());
}

void PressCompenPage::onBack()
{
    m_main->goBack();
}
