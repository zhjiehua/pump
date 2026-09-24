#include "ui/pages/setuppage.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"
#include "ui/widgets/pagescroll.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

namespace {

QPushButton *iconBtn(PictureManager::Picture normal, PictureManager::Picture focus)
{
    auto *b = new QPushButton;
    b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    b->setMinimumSize(40, 40);
    b->setStyleSheet(PictureManager::instance().iconButtonStyle(normal, focus));
    return b;
}

QWidget *iconCell(QPushButton *btn, const QString &caption)
{
    auto *w = new QWidget;
    w->setMinimumSize(56, 64);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(2);
    v->addWidget(btn, 5);
    auto *lab = new QLabel(caption);
    lab->setAlignment(Qt::AlignCenter);
    lab->setMinimumHeight(14);
    v->addWidget(lab, 1);
    return w;
}

} // namespace

SetupPage::SetupPage(MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_main(main)
{
    // Row0: Lang, Calib, Permit, Time
    // Row1: About, Net, Grad, GLP
    // Row2: Admin (厂家), Internal (Internal last)
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
    m_admin = iconBtn(PictureManager::Key, PictureManager::KeyFocus);
    m_internal = iconBtn(PictureManager::Setup, PictureManager::Setup);

    g->addWidget(iconCell(m_lang, tr("Lang")), 0, 0);
    g->addWidget(iconCell(m_cal, tr("Calib")), 0, 1);
    g->addWidget(iconCell(m_perm, tr("Permit")), 0, 2);
    g->addWidget(iconCell(m_clock, tr("Time")), 0, 3);
    g->addWidget(iconCell(m_msg, tr("About")), 1, 0);
    g->addWidget(iconCell(m_net, tr("Net")), 1, 1);
    g->addWidget(iconCell(m_grid, tr("Grad")), 1, 2);
    g->addWidget(iconCell(m_glp, tr("GLP")), 1, 3);
    g->addWidget(iconCell(m_admin, tr("Admin")), 2, 2);
    g->addWidget(iconCell(m_internal, tr("Internal")), 2, 3);

    installPageScroll(this, inner);

    connect(m_lang, SIGNAL(clicked()), this, SLOT(goLanguage()));
    connect(m_cal, SIGNAL(clicked()), this, SLOT(goFix()));
    connect(m_perm, SIGNAL(clicked()), this, SLOT(goPermit()));
    connect(m_clock, SIGNAL(clicked()), this, SLOT(goTime()));
    connect(m_msg, SIGNAL(clicked()), this, SLOT(goMsg()));
    connect(m_net, SIGNAL(clicked()), this, SLOT(goNet()));
    connect(m_grid, SIGNAL(clicked()), this, SLOT(goGradient()));
    connect(m_glp, SIGNAL(clicked()), this, SLOT(goGlp()));
    connect(m_admin, SIGNAL(clicked()), this, SLOT(onAdmin()));
    connect(m_internal, SIGNAL(clicked()), this, SLOT(goInternal()));
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
    xList.append(m_admin);
    xList.append(m_internal);
    yList.append(m_lang);
    yList.append(m_clock);
    yList.append(m_cal);
    yList.append(m_grid);
    yList.append(m_perm);
    yList.append(m_msg);
    yList.append(m_glp);
    yList.append(m_net);
    yList.append(m_admin);
    yList.append(m_internal);
}

void SetupPage::goLanguage() { m_main->go(MainWindow::Language); }
void SetupPage::goFix() { m_main->go(MainWindow::Fix); }
void SetupPage::goPermit() { m_main->go(MainWindow::Permit); }
void SetupPage::goTime() { m_main->go(MainWindow::Time); }
void SetupPage::goMsg() { m_main->go(MainWindow::Msg); }
void SetupPage::goNet() { m_main->go(MainWindow::Net); }
void SetupPage::goGradient() { m_main->go(MainWindow::Gradient); }
void SetupPage::goGlp() { m_main->go(MainWindow::Glp); }
void SetupPage::onAdmin() { m_main->requestAdminAccess(); }
void SetupPage::goInternal() { m_main->go(MainWindow::Internal); }
