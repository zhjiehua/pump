#include "ui/pages/adminpage.h"
#include "utils/qtwidgetsutil.h"
#include "core/appsettings.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/pagescroll.h"

#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

AdminPage::AdminPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto *scroll = makePageScroll();
    auto *inner = new QWidget;
    auto *vl = new QVBoxLayout(inner);
    vl->setContentsMargins(4, 4, 4, 4);

    auto *actions = new QHBoxLayout;
    m_cancelActive = new QPushButton(tr("Cancel Active"));
    m_clearSys = new QPushButton(tr("Clear Sys Time"));
    m_clearPump = new QPushButton(tr("Clear Pump Time"));
    m_restore = new QPushButton(tr("Restore"));
    m_importJson = new QPushButton(tr("Import JSON"));
    actions->addWidget(m_cancelActive);
    actions->addWidget(m_clearSys);
    actions->addWidget(m_clearPump);
    actions->addWidget(m_restore);
    actions->addWidget(m_importJson);
    vl->addLayout(actions);

    auto *dates = new QGroupBox(tr("Dates"));
    auto *dg = new QGridLayout(dates);
    m_manufYear = new QLineEdit;
    m_manufMonth = new QLineEdit;
    m_manufDay = new QLineEdit;
    m_instYear = new QLineEdit;
    m_instMonth = new QLineEdit;
    m_instDay = new QLineEdit;
    m_repairYear = new QLineEdit;
    m_repairMonth = new QLineEdit;
    m_repairDay = new QLineEdit;

    dg->addWidget(new QLabel(tr("Manuf Y/M/D")), 0, 0);
    dg->addWidget(m_manufYear, 0, 1);
    dg->addWidget(m_manufMonth, 0, 2);
    dg->addWidget(m_manufDay, 0, 3);
    dg->addWidget(new QLabel(tr("Inst Y/M/D")), 1, 0);
    dg->addWidget(m_instYear, 1, 1);
    dg->addWidget(m_instMonth, 1, 2);
    dg->addWidget(m_instDay, 1, 3);
    dg->addWidget(new QLabel(tr("Repair Y/M/D")), 2, 0);
    dg->addWidget(m_repairYear, 2, 1);
    dg->addWidget(m_repairMonth, 2, 2);
    dg->addWidget(m_repairDay, 2, 3);
    vl->addWidget(dates);

    auto *form = new QFormLayout;
    m_license = new QLineEdit;
    m_license->setReadOnly(true);
    m_serial = new QLineEdit;
    m_pcProto = new QComboBox;
    m_pcProto->addItem(tr("Legacy"), int(AppSettings::LegacyPc));
    m_pcProto->addItem(tr("Clarity"), int(AppSettings::Clarity));
    m_connect = new QComboBox;
    m_connect->addItem(tr("Serial"), int(AppSettings::Serial));
    m_connect->addItem(tr("UDP"), int(AppSettings::Udp));
    m_pmax = new QLineEdit;

    form->addRow(tr("License"), m_license);
    form->addRow(tr("Serial"), m_serial);
    form->addRow(tr("PC protocol"), m_pcProto);
    form->addRow(tr("Connect"), m_connect);
    form->addRow(tr("Pmax limit"), m_pmax);
    vl->addLayout(form);

    scroll->setWidget(inner);
    root->addWidget(scroll, 1);

    auto *btns = new QHBoxLayout;
    m_save = new QPushButton(tr("Save"));
    m_back = new QPushButton(tr("Back"));
    btns->addWidget(m_save);
    btns->addWidget(m_back);
    root->addLayout(btns);

    loadFromSettings();

    connect(m_cancelActive, SIGNAL(clicked()), this, SLOT(onCancelActive()));
    connect(m_clearSys, SIGNAL(clicked()), this, SLOT(onClearSys()));
    connect(m_clearPump, SIGNAL(clicked()), this, SLOT(onClearPump()));
    connect(m_restore, SIGNAL(clicked()), this, SLOT(onRestore()));
    connect(m_importJson, SIGNAL(clicked()), this, SLOT(onImportJson()));
    connect(m_save, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(m_back, SIGNAL(clicked()), this, SLOT(onBack()));
}

void AdminPage::initFocusList()
{
    xList.append(m_cancelActive);
    xList.append(m_clearSys);
    xList.append(m_clearPump);
    xList.append(m_restore);
    xList.append(m_importJson);
    xList.append(m_save);
    xList.append(m_manufYear);
    xList.append(m_manufMonth);
    xList.append(m_manufDay);
    xList.append(m_instYear);
    xList.append(m_instMonth);
    xList.append(m_instDay);
    xList.append(m_repairYear);
    xList.append(m_repairMonth);
    xList.append(m_repairDay);
    xList.append(m_license);
    xList.append(m_pcProto);
    xList.append(m_serial);
    xList.append(m_connect);
    xList.append(m_pmax);
    xList.append(m_back);

    yList.append(m_cancelActive);
    yList.append(m_clearSys);
    yList.append(m_clearPump);
    yList.append(m_restore);
    yList.append(m_importJson);
    yList.append(m_save);
    yList.append(m_manufYear);
    yList.append(m_instYear);
    yList.append(m_repairYear);
    yList.append(m_manufMonth);
    yList.append(m_instMonth);
    yList.append(m_repairMonth);
    yList.append(m_manufDay);
    yList.append(m_instDay);
    yList.append(m_repairDay);
    yList.append(m_license);
    yList.append(m_serial);
    yList.append(m_pmax);
    yList.append(m_pcProto);
    yList.append(m_connect);
    yList.append(m_back);
}

void AdminPage::onCancelActive()
{
    if (QMessageBox::question(this, tr("Tips"), tr("Comfirm to Cancel Active!!!"))
        != QMessageBox::Yes)
        return;
    auto *s = m_c->settings();
    s->bActive = false;
    s->sysUsedSec = 0;
    s->save();
    m_c->postLog(tr("Activation cancelled"));
}

void AdminPage::onClearSys()
{
    if (QMessageBox::question(this, tr("Tips"), tr("Comfirm to Clear!!!"))
        != QMessageBox::Yes)
        return;
    m_c->usage()->clearSystemTime();
}

void AdminPage::onClearPump()
{
    if (QMessageBox::question(this, tr("Tips"), tr("Comfirm to Clear!!!"))
        != QMessageBox::Yes)
        return;
    m_c->usage()->clearPumpTime();
}

void AdminPage::onRestore()
{
    if (QMessageBox::question(this, tr("Tips"), tr("Comfirm to Restore!!!"))
        != QMessageBox::Yes)
        return;
    QMessageBox::information(this, tr("Tips"), tr("Restore not implemented."));
}

void AdminPage::onImportJson()
{
    const QString path = QFileDialog::getOpenFileName(
        this, tr("Import JSON config"), m_c->settings()->configPath(),
        tr("JSON files (*.json);;All files (*)"));
    if (path.isEmpty())
        return;
    if (m_c->importJsonConfig(path))
        QMessageBox::information(this, tr("Tips"), tr("JSON config imported."));
    else
        QMessageBox::warning(this, tr("Tips"), tr("Import failed — see log."));
    loadFromSettings();
}

void AdminPage::onSave()
{
    applyToSettings();
    if (m_c->settings()->save())
        m_c->postLog(tr("Admin settings saved"));
    m_c->connectPc();
}

void AdminPage::onBack()
{
    m_main->goBack();
}

void AdminPage::loadFromSettings()
{
    auto *s = m_c->settings();
    m_manufYear->setText(s->manufYear);
    m_manufMonth->setText(s->manufMonth);
    m_manufDay->setText(s->manufDay);
    m_instYear->setText(s->instYear);
    m_instMonth->setText(s->instMonth);
    m_instDay->setText(s->instDay);
    m_repairYear->setText(s->repairYear);
    m_repairMonth->setText(s->repairMonth);
    m_repairDay->setText(s->repairDay);
    m_license->setText(s->license);
    m_serial->setText(s->serial);
    m_pcProto->setCurrentIndex(int(s->pcProtocol));
    m_connect->setCurrentIndex(int(s->pcPort));
    m_pmax->setText(s->pmaxLimit > 0 ? QString::number(s->pmaxLimit, 'f', 0)
                                     : QStringLiteral("0"));
}

void AdminPage::applyToSettings()
{
    auto *s = m_c->settings();
    s->manufYear = m_manufYear->text().trimmed();
    s->manufMonth = m_manufMonth->text().trimmed();
    s->manufDay = m_manufDay->text().trimmed();
    s->instYear = m_instYear->text().trimmed();
    s->instMonth = m_instMonth->text().trimmed();
    s->instDay = m_instDay->text().trimmed();
    s->repairYear = m_repairYear->text().trimmed();
    s->repairMonth = m_repairMonth->text().trimmed();
    s->repairDay = m_repairDay->text().trimmed();
    s->serial = m_serial->text().trimmed();
    s->pcProtocol = AppSettings::PcProtocol(hmiComboCurrentData(m_pcProto).toInt());
    s->pcPort = AppSettings::PcPort(hmiComboCurrentData(m_connect).toInt());
    s->pmaxLimit = m_pmax->text().toDouble();
}
