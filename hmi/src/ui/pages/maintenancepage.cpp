#include "ui/pages/maintenancepage.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/pagescroll.h"

#include <QGridLayout>
#include <QLabel>
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

MaintenancePage::MaintenancePage(MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *g = new QGridLayout(inner);
    g->setContentsMargins(8, 6, 8, 6);
    g->setHorizontalSpacing(10);
    g->setVerticalSpacing(8);

    m_maint = iconBtn(PictureManager::Permission, PictureManager::PermissionFocus);
    m_device = iconBtn(PictureManager::GlpInfo, PictureManager::GlpInfoFocus);
    m_data = iconBtn(PictureManager::Message, PictureManager::MessageFocus);
    m_mcu = iconBtn(PictureManager::Mcu, PictureManager::McuFocus);
    m_cds = iconBtn(PictureManager::NetConfig, PictureManager::NetConfigFocus);
    m_machine = iconBtn(PictureManager::Calibration, PictureManager::CalibrationFocus);
    m_system = iconBtn(PictureManager::Clock, PictureManager::ClockFocus);
    m_records = iconBtn(PictureManager::Grid, PictureManager::GridFocus);

    g->addWidget(iconCell(m_maint, &m_maintLabel), 0, 0);
    g->addWidget(iconCell(m_device, &m_deviceLabel), 0, 1);
    g->addWidget(iconCell(m_data, &m_dataLabel), 0, 2);
    g->addWidget(iconCell(m_mcu, &m_mcuLabel), 0, 3);
    g->addWidget(iconCell(m_cds, &m_cdsLabel), 1, 0);
    g->addWidget(iconCell(m_machine, &m_machineLabel), 1, 1);
    g->addWidget(iconCell(m_system, &m_systemLabel), 1, 2);
    g->addWidget(iconCell(m_records, &m_recordsLabel), 1, 3);

    installPageScroll(this, inner);
    retranslateUi();

    connect(m_maint, SIGNAL(clicked()), this, SLOT(goMaintenance()));
    connect(m_device, SIGNAL(clicked()), this, SLOT(goDevice()));
    connect(m_data, SIGNAL(clicked()), this, SLOT(goData()));
    connect(m_mcu, SIGNAL(clicked()), this, SLOT(goMcu()));
    connect(m_cds, SIGNAL(clicked()), this, SLOT(goCds()));
    connect(m_machine, SIGNAL(clicked()), this, SLOT(goMachine()));
    connect(m_system, SIGNAL(clicked()), this, SLOT(goSystem()));
    connect(m_records, SIGNAL(clicked()), this, SLOT(goRecords()));
}

void MaintenancePage::retranslateUi()
{
    m_maintLabel->setText(tr("Service"));
    m_deviceLabel->setText(tr("Device"));
    m_dataLabel->setText(tr("Data"));
    m_mcuLabel->setText(tr("MCU"));
    m_cdsLabel->setText(tr("CDS"));
    m_machineLabel->setText(tr("Machine"));
    m_systemLabel->setText(tr("System"));
    m_recordsLabel->setText(tr("Records"));
}

void MaintenancePage::initFocusList()
{
    xList.append(m_maint);
    xList.append(m_device);
    xList.append(m_data);
    xList.append(m_mcu);
    xList.append(m_cds);
    xList.append(m_machine);
    xList.append(m_system);
    xList.append(m_records);
    yList.append(m_maint);
    yList.append(m_cds);
    yList.append(m_device);
    yList.append(m_machine);
    yList.append(m_data);
    yList.append(m_system);
    yList.append(m_mcu);
    yList.append(m_records);
}

void MaintenancePage::goMaintenance() { m_main->go(MainWindow::AdminMaintenance); }
void MaintenancePage::goDevice() { m_main->go(MainWindow::AdminDevice); }
void MaintenancePage::goData() { m_main->go(MainWindow::AdminData); }
void MaintenancePage::goMcu() { m_main->go(MainWindow::ConfigMcu); }
void MaintenancePage::goCds() { m_main->go(MainWindow::ConfigCds); }
void MaintenancePage::goMachine() { m_main->go(MainWindow::ConfigMachine); }
void MaintenancePage::goSystem() { m_main->go(MainWindow::ConfigSystem); }
void MaintenancePage::goRecords() { m_main->go(MainWindow::Records); }
