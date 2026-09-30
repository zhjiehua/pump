#include "ui/pages/fixpage.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/imgbutton.h"
#include "ui/widgets/pagescroll.h"

#include <QHBoxLayout>
#include <QSignalMapper>
#include <QVBoxLayout>

FixPage::FixPage(MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_main(main)
    , m_mapper(new QSignalMapper(this))
{
    auto *inner = new QWidget;
    auto *root = new QHBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);
    root->setSpacing(0);

    root->addStretch(1);

    auto *iconCol = new QVBoxLayout;
    iconCol->addStretch(2);
    m_icon = new ImgButton;
    m_icon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_icon->setFocusPolicy(Qt::NoFocus);
    m_icon->setBkImage(PictureManager::Calibration);
    iconCol->addWidget(m_icon, 7);
    iconCol->addStretch(2);
    root->addLayout(iconCol, 3);

    root->addStretch(1);

    auto *btnCol = new QVBoxLayout;
    btnCol->setSpacing(6);
    btnCol->addStretch(1);

    auto addBtn = [&](BtnCtrl **slot, MainWindow::Page page) {
        auto *b = new BtnCtrl;
        *slot = b;
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        btnCol->addWidget(b, 2);
        connect(b, SIGNAL(clicked()), m_mapper, SLOT(map()));
        m_mapper->setMapping(b, int(page));
        btnCol->addStretch(1);
    };
    addBtn(&m_pressCal, MainWindow::PressFix);
    addBtn(&m_flowCal, MainWindow::FlowFix);

    root->addLayout(btnCol, 3);
    root->addStretch(1);

    installPageScroll(this, inner);
    retranslateUi();

    connect(m_mapper, SIGNAL(mapped(int)), this, SLOT(goPage(int)));
}

void FixPage::initFocusList()
{
    xList.append(m_pressCal);
    xList.append(m_flowCal);
    yList.append(m_pressCal);
    yList.append(m_flowCal);
}

void FixPage::retranslateUi()
{
    m_pressCal->setText(tr("Press Calibration"));
    m_flowCal->setText(tr("Flow Calibration"));
}

void FixPage::goPage(int page)
{
    m_main->go(MainWindow::Page(page));
}
