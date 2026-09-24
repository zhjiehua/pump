#include "ui/pages/gradientpage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/pagescroll.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

GradientPage::GradientPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
    , m_mapper(new QSignalMapper(this))
{
    Q_UNUSED(m_c);

    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(8, 8, 8, 8);

    auto *grid = new QGridLayout;
    grid->setHorizontalSpacing(6);
    grid->setVerticalSpacing(6);

    m_rg.resize(10);
    for (int i = 0; i < 10; ++i)
    {
        const int row = i / 5;
        const int col = i % 5;
        auto *btn = new QPushButton(QStringLiteral("RG%1").arg(i + 1));
        m_rg[i] = btn;
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        btn->setMinimumHeight(40);
        grid->addWidget(btn, row, col);
        connect(btn, SIGNAL(clicked()), m_mapper, SLOT(map()));
        m_mapper->setMapping(btn, i);
    }
    root->addLayout(grid, 1);

    auto *btns = new QHBoxLayout;
    m_back = new QPushButton(tr("Back"));
    btns->addStretch(1);
    btns->addWidget(m_back);
    root->addLayout(btns);

    installPageScroll(this, inner);

    connect(m_mapper, SIGNAL(mapped(int)), this, SLOT(onGradientSelected(int)));
    connect(m_back, SIGNAL(clicked()), this, SLOT(onBack()));
}

void GradientPage::initFocusList()
{
    for (QPushButton *b : m_rg)
        xList.append(b);
    xList.append(m_back);
    for (int col = 0; col < 5; ++col)
    {
        if (m_rg[col])
            yList.append(m_rg[col]);
        if (m_rg[col + 5])
            yList.append(m_rg[col + 5]);
    }
    yList.append(m_back);
}

void GradientPage::onGradientSelected(int index)
{
    m_main->goGradientTable(index);
}

void GradientPage::onBack()
{
    m_main->goBack();
}
