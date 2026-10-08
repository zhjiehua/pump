#include "ui/pages/setuppage.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/pagescroll.h"

#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

namespace {

BtnCtrl *iconBtn(PictureManager::Picture normal, PictureManager::Picture focus)
{
    auto *b = new BtnCtrl;
    b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    b->setMinimumSize(40, 40);
    b->setStyleSheet(PictureManager::instance().iconButtonStyle(normal, focus));
    return b;
}

QWidget *iconCell(BtnCtrl *btn, QLabel **label)
{
    auto *w = new QWidget;
    w->setMinimumSize(56, 64);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(2);
    v->addWidget(btn, 5);
    *label = new QLabel;
    (*label)->setAlignment(Qt::AlignCenter);
    (*label)->setMinimumHeight(14);
    v->addWidget(*label, 1);
    return w;
}

} // namespace

SetupPage::SetupPage(MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_main(main)
{
    // Row0: Lang, Calib, Permit, GLP  (weiduodianzi SetupPage.ui)
    // Row1: Time, Grad, About, Net
    auto *inner = new QWidget;
    auto *g = new QGridLayout(inner);
    g->setContentsMargins(8, 6, 8, 6);
    g->setHorizontalSpacing(10);
    g->setVerticalSpacing(8);

    m_lang = iconBtn(PictureManager::Global, PictureManager::GlobalFocus);
    m_cal = iconBtn(PictureManager::Calibration, PictureManager::CalibrationFocus);
    m_perm = iconBtn(PictureManager::Permission, PictureManager::PermissionFocus);
    m_clock = iconBtn(PictureManager::Clock, PictureManager::ClockFocus);
    m_msg = iconBtn(PictureManager::Message, PictureManager::MessageFocus);
    m_net = iconBtn(PictureManager::NetConfig, PictureManager::NetConfigFocus);
    m_grid = iconBtn(PictureManager::Grid, PictureManager::GridFocus);
    m_glp = iconBtn(PictureManager::GlpInfo, PictureManager::GlpInfoFocus);

    g->addWidget(iconCell(m_lang, &m_langLabel), 0, 0);
    g->addWidget(iconCell(m_cal, &m_calLabel), 0, 1);
    g->addWidget(iconCell(m_perm, &m_permLabel), 0, 2);
    g->addWidget(iconCell(m_glp, &m_glpLabel), 0, 3);
    g->addWidget(iconCell(m_clock, &m_clockLabel), 1, 0);
    g->addWidget(iconCell(m_grid, &m_gridLabel), 1, 1);
    g->addWidget(iconCell(m_msg, &m_msgLabel), 1, 2);
    g->addWidget(iconCell(m_net, &m_netLabel), 1, 3);

    installPageScroll(this, inner);
    retranslateUi();

    connect(m_lang, SIGNAL(clicked()), this, SLOT(goLanguage()));
    connect(m_cal, SIGNAL(clicked()), this, SLOT(goFix()));
    connect(m_perm, SIGNAL(clicked()), this, SLOT(goPermit()));
    connect(m_clock, SIGNAL(clicked()), this, SLOT(goTime()));
    connect(m_msg, SIGNAL(clicked()), this, SLOT(goMsg()));
    connect(m_net, SIGNAL(clicked()), this, SLOT(goNet()));
    connect(m_grid, SIGNAL(clicked()), this, SLOT(goGradient()));
    connect(m_glp, SIGNAL(clicked()), this, SLOT(goGlp()));
}

void SetupPage::retranslateUi()
{
    m_langLabel->setText(tr("Lang"));
    m_calLabel->setText(tr("Calib"));
    m_permLabel->setText(tr("Permit"));
    m_glpLabel->setText(tr("GLP"));
    m_clockLabel->setText(tr("Time"));
    m_gridLabel->setText(tr("Grad"));
    m_msgLabel->setText(tr("About"));
    m_netLabel->setText(tr("Net"));
}

void SetupPage::initFocusList()
{
    xList.append(m_lang);
    xList.append(m_cal);
    xList.append(m_perm);
    xList.append(m_glp);
    xList.append(m_clock);
    xList.append(m_grid);
    xList.append(m_msg);
    xList.append(m_net);
    yList.append(m_lang);
    yList.append(m_clock);
    yList.append(m_cal);
    yList.append(m_grid);
    yList.append(m_perm);
    yList.append(m_msg);
    yList.append(m_glp);
    yList.append(m_net);
}

void SetupPage::goLanguage() { m_main->go(MainWindow::Language); }
void SetupPage::goFix() { m_main->go(MainWindow::Fix); }
void SetupPage::goPermit() { m_main->go(MainWindow::Permit); }
void SetupPage::goTime() { m_main->go(MainWindow::Time); }
void SetupPage::goMsg() { m_main->go(MainWindow::Msg); }
void SetupPage::goNet() { m_main->go(MainWindow::Net); }
void SetupPage::goGradient() { m_main->goGradientTable(); }
void SetupPage::goGlp() { m_main->go(MainWindow::Glp); }
