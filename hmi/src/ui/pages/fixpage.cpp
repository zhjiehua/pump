#include "ui/pages/fixpage.h"
#include "ui/mainwindow.h"
#include "ui/widgets/pagescroll.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

FixPage::FixPage(MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_main(main)
    , m_mapper(new QSignalMapper(this))
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(8, 8, 8, 8);

    auto *g = new QGridLayout;
    g->setHorizontalSpacing(8);
    g->setVerticalSpacing(8);

    auto add = [&](QPushButton **slot, int r, int c, const QString &label, MainWindow::Page page) {
        auto *b = new QPushButton(label);
        *slot = b;
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        b->setMinimumHeight(48);
        g->addWidget(b, r, c);
        connect(b, SIGNAL(clicked()), m_mapper, SLOT(map()));
        m_mapper->setMapping(b, int(page));
    };

    add(&m_pressCal, 0, 0, tr("Press Calib"), MainWindow::PressFix);
    add(&m_flowCal, 0, 1, tr("Flow Calib"), MainWindow::FlowFix);
    add(&m_pulseCal, 1, 0, tr("Pulse Calib"), MainWindow::PulseFix);
    add(&m_pressCompen, 1, 1, tr("Press Compen"), MainWindow::PressCompen);

    root->addLayout(g, 1);

    auto *btns = new QHBoxLayout;
    auto *back = new QPushButton(tr("Back"));
    btns->addStretch(1);
    btns->addWidget(back);
    root->addLayout(btns);

    installPageScroll(this, inner);

    connect(m_mapper, SIGNAL(mapped(int)), this, SLOT(goPage(int)));
    connect(back, SIGNAL(clicked()), this, SLOT(onBack()));
}

void FixPage::initFocusList()
{
    xList.append(m_pressCal);
    xList.append(m_flowCal);
    xList.append(m_pulseCal);
    xList.append(m_pressCompen);
    yList.append(m_pressCal);
    yList.append(m_pulseCal);
    yList.append(m_flowCal);
    yList.append(m_pressCompen);
}

void FixPage::goPage(int page)
{
    m_main->go(MainWindow::Page(page));
}

void FixPage::onBack()
{
    m_main->goBack();
}
