#include "ui/pages/adminpage.h"
#include "utils/qtwidgetsutil.h"
#include "core/appsettings.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/comboctrl.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/editfield.h"
#include "ui/widgets/pagescroll.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

namespace {

void expand(QWidget *w)
{
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QHBoxLayout *dateFields(EditCtrl *y, EditCtrl *mo, EditCtrl *d)
{
    auto *fields = new QHBoxLayout;
    fields->addWidget(y);
    fields->addWidget(makeSep(QStringLiteral("-")));
    fields->addWidget(mo);
    fields->addWidget(makeSep(QStringLiteral("-")));
    fields->addWidget(d);
    return fields;
}

} // namespace

AdminPage::AdminPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *vl = new QVBoxLayout(inner);
    vl->setContentsMargins(6, 0, 6, 0);
    vl->setSpacing(6);

    auto *actions = new QHBoxLayout;
    m_cancelActive = new BtnCtrl;
    m_clearSys = new BtnCtrl;
    m_clearPump = new BtnCtrl;
    m_restore = new BtnCtrl;
    expand(m_cancelActive);
    expand(m_clearSys);
    expand(m_clearPump);
    expand(m_restore);
    actions->addWidget(m_cancelActive, 1);
    actions->addWidget(m_clearSys, 1);
    actions->addWidget(m_clearPump, 1);
    actions->addWidget(m_restore, 1);
    vl->addLayout(actions, 1);
    vl->addStretch(1);

    auto *dataRow = new QHBoxLayout;
    dataRow->setSpacing(12);
    m_saveData = new BtnCtrl;
    m_updateData = new BtnCtrl;
    expand(m_saveData);
    expand(m_updateData);
    dataRow->addWidget(m_saveData, 1);
    dataRow->addWidget(m_updateData, 1);
    vl->addLayout(dataRow, 1);
    vl->addStretch(1);

    m_manufYear = new EditCtrl;
    m_manufMonth = new EditCtrl;
    m_manufDay = new EditCtrl;
    m_instYear = new EditCtrl;
    m_instMonth = new EditCtrl;
    m_instDay = new EditCtrl;
    m_repairYear = new EditCtrl;
    m_repairMonth = new EditCtrl;
    m_repairDay = new EditCtrl;
    m_license = new EditCtrl;
    m_serial = new EditCtrl;
    m_pmax = new EditCtrl;
    expand(m_manufYear);
    expand(m_manufMonth);
    expand(m_manufDay);
    expand(m_instYear);
    expand(m_instMonth);
    expand(m_instDay);
    expand(m_repairYear);
    expand(m_repairMonth);
    expand(m_repairDay);
    expand(m_license);
    expand(m_serial);
    expand(m_pmax);

    m_manufYear->setValRange(1990, 2050, 0);
    m_instYear->setValRange(1990, 2050, 0);
    m_repairYear->setValRange(1990, 2050, 0);
    m_manufMonth->setValRange(1, 12, 0);
    m_instMonth->setValRange(1, 12, 0);
    m_repairMonth->setValRange(1, 12, 0);
    m_manufDay->setValRange(0, 31, 0);
    m_instDay->setValRange(0, 31, 0);
    m_repairDay->setValRange(0, 31, 0);
    m_license->setValRange(0, 9999999999ULL, 0, true);
    m_serial->setValRange(0, 9999999999ULL, 0, true);
    m_pmax->setValRange(0, 200, 2);
    m_license->setReadOnly(true);
    m_instYear->setReadOnly(true);

    auto addDateRow = [&](QLabel **cap, EditCtrl *y, EditCtrl *mo, EditCtrl *d) {
        auto *row = new QHBoxLayout;
        *cap = new QLabel;
        expand(*cap);
        row->addWidget(*cap, 1);
        row->addLayout(dateFields(y, mo, d), 5);
        vl->addLayout(row, 1);
        vl->addStretch(1);
    };
    addDateRow(&m_manufCap, m_manufYear, m_manufMonth, m_manufDay);
    addDateRow(&m_instCap, m_instYear, m_instMonth, m_instDay);
    addDateRow(&m_repairCap, m_repairYear, m_repairMonth, m_repairDay);

    m_pcProto = new ComboCtrl;
    expand(m_pcProto);
    m_pcProto->addItem(QStringLiteral("CXTH"), int(AppSettings::LegacyPc));
    m_pcProto->addItem(QStringLiteral("Clarity"), int(AppSettings::Clarity));
    m_pcProto->addItem(QStringLiteral("QinFine"), int(AppSettings::QinFinePc));

    auto *licenRow = new QHBoxLayout;
    m_licenCap = new QLabel;
    m_protoCap = new QLabel;
    expand(m_licenCap);
    expand(m_protoCap);
    licenRow->addWidget(m_licenCap, 1);
    licenRow->addWidget(m_license, 5);
    licenRow->addStretch(1);
    licenRow->addWidget(m_protoCap, 1);
    licenRow->addWidget(m_pcProto, 5);
    vl->addLayout(licenRow, 1);
    vl->addStretch(1);

    m_connect = new ComboCtrl;
    expand(m_connect);
    m_connect->addItem(QString(), int(AppSettings::Serial));
    m_connect->addItem(QString(), int(AppSettings::Udp));
    m_connect->addItem(QString(), int(AppSettings::TcpServer));

    auto *serialRow = new QHBoxLayout;
    m_serialCap = new QLabel;
    m_conneCap = new QLabel;
    expand(m_serialCap);
    expand(m_conneCap);
    serialRow->addWidget(m_serialCap, 1);
    serialRow->addWidget(m_serial, 5);
    serialRow->addStretch(1);
    serialRow->addWidget(m_conneCap, 1);
    serialRow->addWidget(m_connect, 5);
    vl->addLayout(serialRow, 1);
    vl->addStretch(1);

    auto *pmaxRow = new QHBoxLayout;
    m_pmaxCap = new QLabel;
    expand(m_pmaxCap);
    auto *mpa = new QLabel(QStringLiteral("MPa"));
    expand(mpa);
    pmaxRow->addWidget(m_pmaxCap, 2);
    pmaxRow->addWidget(m_pmax, 5);
    pmaxRow->addWidget(mpa, 2);
    pmaxRow->addStretch(5);
    vl->addLayout(pmaxRow, 1);

    installPageScroll(this, inner);
    retranslateUi();
    loadFromSettings();

    const QList<EditCtrl *> edits = QList<EditCtrl *>()
        << m_manufYear << m_manufMonth << m_manufDay
        << m_instYear << m_instMonth << m_instDay
        << m_repairYear << m_repairMonth << m_repairDay
        << m_license << m_serial << m_pmax;
    for (int i = 0; i < edits.size(); ++i)
    {
        connect(edits.at(i), SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
        connect(edits.at(i), SIGNAL(valueCommitted(QString)), this, SLOT(onFieldChanged()));
    }
    connect(m_pcProto, SIGNAL(popupChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_connect, SIGNAL(popupChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_pcProto, SIGNAL(currentIndexChanged(int)), this, SLOT(onFieldChanged()));
    connect(m_connect, SIGNAL(currentIndexChanged(int)), this, SLOT(onFieldChanged()));

    connect(m_cancelActive, SIGNAL(clicked()), this, SLOT(onCancelActive()));
    connect(m_clearSys, SIGNAL(clicked()), this, SLOT(onClearSys()));
    connect(m_clearPump, SIGNAL(clicked()), this, SLOT(onClearPump()));
    connect(m_restore, SIGNAL(clicked()), this, SLOT(onRestore()));
    connect(m_saveData, SIGNAL(clicked()), this, SLOT(onSaveData()));
    connect(m_updateData, SIGNAL(clicked()), this, SLOT(onUpdateData()));
}

void AdminPage::initFocusList()
{
    xList.append(m_cancelActive);
    xList.append(m_clearSys);
    xList.append(m_clearPump);
    xList.append(m_restore);
    xList.append(m_saveData);
    xList.append(m_updateData);
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

    yList.append(m_cancelActive);
    yList.append(m_clearSys);
    yList.append(m_clearPump);
    yList.append(m_restore);
    yList.append(m_saveData);
    yList.append(m_updateData);
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

    loadFromSettings();
}

void AdminPage::retranslateUi()
{
    m_cancelActive->setText(tr("Deactive"));
    m_clearSys->setText(tr("UTC"));
    m_clearPump->setText(tr("PTC"));
    m_restore->setText(tr("Reset"));
    m_saveData->setText(tr("saveData"));
    m_updateData->setText(tr("updateData"));
    m_manufCap->setText(tr("Manuf Date:"));
    m_instCap->setText(tr("Install Date:"));
    m_repairCap->setText(tr("La Rep Date:"));
    m_licenCap->setText(tr("Licen:"));
    m_protoCap->setText(tr("Proto:"));
    m_serialCap->setText(tr("Serial:"));
    m_conneCap->setText(tr("Conne:"));
    m_pmaxCap->setText(tr("Pmax:"));
    m_connect->setItemText(0, tr("RS232"));
    m_connect->setItemText(1, tr("RJ45"));
    m_connect->setItemText(2, tr("TCP Server"));
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

void AdminPage::onSaveData()
{
    applyToSettings();
    if (m_c->settings()->save())
        QMessageBox::information(this, tr("Tips"), tr("success!"));
    else
        QMessageBox::warning(this, tr("Tips"), tr("failed!"));
    m_c->connectPc();
}

void AdminPage::onUpdateData()
{
    if (QMessageBox::question(this, tr("Tips"), tr("Comfirm to update data?"))
        != QMessageBox::Yes)
        return;
    const QString path = QFileDialog::getOpenFileName(
        this, tr("updateData"), m_c->settings()->configPath(),
        tr("JSON files (*.json);;All files (*)"));
    if (path.isEmpty())
        return;
    if (m_c->importJsonConfig(path))
        QMessageBox::information(this, tr("Tips"), tr("success!"));
    else
        QMessageBox::warning(this, tr("Tips"), tr("failed!"));
    loadFromSettings();
}

void AdminPage::onFieldChanged()
{
    applyToSettings();
    m_c->settings()->save();
    m_c->connectPc();
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
    m_pcProto->blockSignals(true);
    m_connect->blockSignals(true);
    m_pcProto->setCurrentIndex(int(s->pcProtocol));
    int conIdx = m_connect->findData(int(s->pcPort));
    if (conIdx < 0)
        conIdx = m_connect->findData(int(AppSettings::Udp));
    m_connect->setCurrentIndex(qMax(0, conIdx));
    m_pcProto->blockSignals(false);
    m_connect->blockSignals(false);
    const double pmax = s->pmaxLimit > 0 ? s->pmaxLimit : s->pressMax;
    m_pmax->setText(QString::number(pmax, 'f', 0));
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
    if (s->pmaxLimit > 0)
        s->pressMax = s->pmaxLimit;
}
