#include "ui/pages/adminmaintenancepage.h"
#include "core/appsettings.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "utils/eventlog.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/editfield.h"
#include "ui/widgets/msgbox.h"
#include "ui/widgets/pagescroll.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace {

void expand(QWidget *w)
{
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QString repairDateText(const AppSettings *s)
{
    return s->repairYear + QLatin1Char('-') + s->repairMonth + QLatin1Char('-') + s->repairDay;
}

void logRepairDateIfChanged(const QString &oldDate, const AppSettings *s)
{
    const QString newDate = repairDateText(s);
    if (oldDate == newDate)
        return;
    EventLog::key(QStringLiteral("MAINT"),
                  QStringLiteral("last repair date %1").arg(newDate));
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

AdminMaintenancePage::AdminMaintenancePage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *vl = new QVBoxLayout(inner);
    vl->setContentsMargins(6, 0, 6, 0);
    vl->setSpacing(6);

    m_repairYear = new EditCtrl;
    m_repairMonth = new EditCtrl;
    m_repairDay = new EditCtrl;
    expand(m_repairYear);
    expand(m_repairMonth);
    expand(m_repairDay);
    m_repairYear->setValRange(1990, 2050, 0);
    m_repairMonth->setValRange(1, 12, 0);
    m_repairDay->setValRange(0, 31, 0);

    auto *dateRow = new QHBoxLayout;
    m_repairCap = new QLabel;
    expand(m_repairCap);
    dateRow->addWidget(m_repairCap, 1);
    dateRow->addLayout(dateFields(m_repairYear, m_repairMonth, m_repairDay), 5);
    vl->addLayout(dateRow, 1);
    vl->addStretch(1);

    m_save = new BtnCtrl;
    expand(m_save);
    auto *saveRow = new QHBoxLayout;
    saveRow->addStretch(1);
    saveRow->addWidget(m_save, 2);
    saveRow->addStretch(1);
    vl->addLayout(saveRow, 1);
    vl->addStretch(1);

    auto *actions = new QGridLayout;
    actions->setHorizontalSpacing(6);
    actions->setVerticalSpacing(6);
    m_cancelActive = new BtnCtrl;
    m_clearSys = new BtnCtrl;
    m_clearPump = new BtnCtrl;
    m_clearRecords = new BtnCtrl;
    expand(m_cancelActive);
    expand(m_clearSys);
    expand(m_clearPump);
    expand(m_clearRecords);
    actions->addWidget(m_cancelActive, 0, 0);
    actions->addWidget(m_clearSys, 0, 1);
    actions->addWidget(m_clearPump, 1, 0);
    actions->addWidget(m_clearRecords, 1, 1);
    vl->addLayout(actions, 2);
    vl->addStretch(2);

    installPageScroll(this, inner);
    retranslateUi();
    loadFromSettings();

    const QList<EditCtrl *> edits = QList<EditCtrl *>()
        << m_repairYear << m_repairMonth << m_repairDay;
    for (int i = 0; i < edits.size(); ++i)
    {
        connect(edits.at(i), SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
        connect(edits.at(i), SIGNAL(valueCommitted(QString)), this, SLOT(onFieldChanged()));
    }
    connect(m_save, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(m_cancelActive, SIGNAL(clicked()), this, SLOT(onCancelActive()));
    connect(m_clearSys, SIGNAL(clicked()), this, SLOT(onClearSys()));
    connect(m_clearPump, SIGNAL(clicked()), this, SLOT(onClearPump()));
    connect(m_clearRecords, SIGNAL(clicked()), this, SLOT(onClearRecords()));
}

void AdminMaintenancePage::initFocusList()
{
    xList.append(m_repairYear);
    xList.append(m_repairMonth);
    xList.append(m_repairDay);
    xList.append(m_save);
    xList.append(m_cancelActive);
    xList.append(m_clearSys);
    xList.append(m_clearPump);
    xList.append(m_clearRecords);

    yList.append(m_repairYear);
    yList.append(m_save);
    yList.append(m_cancelActive);
    yList.append(m_clearPump);
    yList.append(m_repairMonth);
    yList.append(m_clearSys);
    yList.append(m_clearRecords);
    yList.append(m_repairDay);

    loadFromSettings();
}

void AdminMaintenancePage::retranslateUi()
{
    m_repairCap->setText(tr("Last Repair Date:"));
    m_save->setText(tr("Save"));
    m_cancelActive->setText(tr("Deactive"));
    m_clearSys->setText(tr("Clear system\nused time"));
    m_clearPump->setText(tr("Clear pump\nused time"));
    m_clearRecords->setText(tr("Clear all\nrecords"));
}

void AdminMaintenancePage::onSave()
{
    auto *s = m_c->settings();
    const QString oldRepair = repairDateText(s);
    applyToSettings();
    if (s->save())
    {
        logRepairDateIfChanged(oldRepair, s);
        MsgBox::information(this, tr("Tips"), tr("success!"));
    }
    else
        MsgBox::warning(this, tr("Tips"), tr("failed!"));
}

void AdminMaintenancePage::onFieldChanged()
{
    auto *s = m_c->settings();
    const QString oldRepair = repairDateText(s);
    applyToSettings();
    if (s->save())
        logRepairDateIfChanged(oldRepair, s);
}

void AdminMaintenancePage::loadFromSettings()
{
    auto *s = m_c->settings();
    m_repairYear->setText(s->repairYear);
    m_repairMonth->setText(s->repairMonth);
    m_repairDay->setText(s->repairDay);
}

void AdminMaintenancePage::applyToSettings()
{
    auto *s = m_c->settings();
    s->repairYear = m_repairYear->text().trimmed();
    s->repairMonth = m_repairMonth->text().trimmed();
    s->repairDay = m_repairDay->text().trimmed();
}

void AdminMaintenancePage::onCancelActive()
{
    if (MsgBox::question(this, tr("Tips"), tr("Comfirm to Cancel Active!!!"))
        != MsgBox::Yes)
        return;
    auto *s = m_c->settings();
    s->bActive = false;
    s->sysUsedSec = 0;
    s->save();
    EventLog::key(QStringLiteral("MAINT"), QStringLiteral("deactivate"));
}

void AdminMaintenancePage::onClearSys()
{
    if (MsgBox::question(this, tr("Tips"), tr("Comfirm to Clear!!!"))
        != MsgBox::Yes)
        return;
    m_c->usage()->clearSystemTime();
    EventLog::key(QStringLiteral("MAINT"), QStringLiteral("clear system used time"));
}

void AdminMaintenancePage::onClearPump()
{
    if (MsgBox::question(this, tr("Tips"), tr("Comfirm to Clear!!!"))
        != MsgBox::Yes)
        return;
    m_c->usage()->clearPumpTime();
    EventLog::key(QStringLiteral("MAINT"), QStringLiteral("clear pump used time"));
}

void AdminMaintenancePage::onClearRecords()
{
    if (MsgBox::question(this, tr("Tips"), tr("Clear all records?"))
        != MsgBox::Yes)
        return;
    m_c->records()->clearAll();
}
