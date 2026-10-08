#include "ui/pages/admindatapage.h"
#include "core/dataexchange.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "utils/eventlog.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/msgbox.h"
#include "ui/widgets/pagescroll.h"

#include <QGridLayout>
#include <QVBoxLayout>

namespace {

void expand(QWidget *w)
{
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

} // namespace

AdminDataPage::AdminDataPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *g = new QGridLayout(inner);
    g->setContentsMargins(6, 0, 6, 0);
    g->setHorizontalSpacing(8);
    g->setVerticalSpacing(8);

    m_saveFactory = new BtnCtrl;
    m_restoreFactory = new BtnCtrl;
    m_restoreDefaults = new BtnCtrl;
    m_exportData = new BtnCtrl;
    m_importData = new BtnCtrl;
    m_exportRecords = new BtnCtrl;
    expand(m_saveFactory);
    expand(m_restoreFactory);
    expand(m_restoreDefaults);
    expand(m_exportData);
    expand(m_importData);
    expand(m_exportRecords);
    g->addWidget(m_saveFactory, 0, 0);
    g->addWidget(m_restoreFactory, 0, 1);
    g->addWidget(m_restoreDefaults, 1, 0);
    g->addWidget(m_exportData, 1, 1);
    g->addWidget(m_importData, 2, 0);
    g->addWidget(m_exportRecords, 2, 1);

    installPageScroll(this, inner);
    retranslateUi();

    connect(m_saveFactory, SIGNAL(clicked()), this, SLOT(onSaveFactory()));
    connect(m_restoreFactory, SIGNAL(clicked()), this, SLOT(onRestoreFactory()));
    connect(m_restoreDefaults, SIGNAL(clicked()), this, SLOT(onRestoreDefaults()));
    connect(m_exportData, SIGNAL(clicked()), this, SLOT(onExportData()));
    connect(m_importData, SIGNAL(clicked()), this, SLOT(onImportData()));
    connect(m_exportRecords, SIGNAL(clicked()), this, SLOT(onExportRecords()));
}

void AdminDataPage::initFocusList()
{
    xList.append(m_saveFactory);
    xList.append(m_restoreFactory);
    xList.append(m_restoreDefaults);
    xList.append(m_exportData);
    xList.append(m_importData);
    xList.append(m_exportRecords);
    yList.append(m_saveFactory);
    yList.append(m_restoreDefaults);
    yList.append(m_importData);
    yList.append(m_restoreFactory);
    yList.append(m_exportData);
    yList.append(m_exportRecords);
}

void AdminDataPage::retranslateUi()
{
    m_saveFactory->setText(tr("Save as\nfactory data"));
    m_restoreFactory->setText(tr("Restore\nfactory settings"));
    m_restoreDefaults->setText(tr("Restore\ndefault data"));
    m_exportData->setText(tr("Export\ndata"));
    m_importData->setText(tr("Import\ndata"));
    m_exportRecords->setText(tr("Export\nrecords"));
}

void AdminDataPage::onSaveFactory()
{
    if (MsgBox::question(this, tr("Tips"), tr("Save current settings as factory data?"))
        != MsgBox::Yes)
        return;
    auto *s = m_c->settings();
    if (!s->save())
    {
        MsgBox::warning(this, tr("Tips"), tr("failed!"));
        return;
    }
    if (s->saveFactorySnapshot())
    {
        EventLog::key(QStringLiteral("MAINT"), QStringLiteral("save factory data"));
        MsgBox::information(this, tr("Tips"), tr("success!"));
    }
    else
        MsgBox::warning(this, tr("Tips"), tr("failed!"));
}

void AdminDataPage::onRestoreFactory()
{
    auto *s = m_c->settings();
    if (!s->hasFactorySnapshot())
    {
        MsgBox::warning(this, tr("Tips"), tr("Factory data not found."));
        return;
    }
    if (MsgBox::question(this, tr("Tips"), tr("Restore settings from factory data?"))
        != MsgBox::Yes)
        return;
    if (!s->restoreFactorySnapshot())
    {
        MsgBox::warning(this, tr("Tips"), tr("failed!"));
        return;
    }
    m_c->reloadFromSettings();
    m_c->connectPc();
    EventLog::key(QStringLiteral("MAINT"), QStringLiteral("restore factory settings"));
    MsgBox::information(this, tr("Tips"), tr("success!"));
}

void AdminDataPage::onRestoreDefaults()
{
    if (MsgBox::question(this, tr("Tips"), tr("Restore built-in default data?"))
        != MsgBox::Yes)
        return;
    auto *s = m_c->settings();
    s->resetToBuiltInDefaults();
    if (!s->save())
    {
        MsgBox::warning(this, tr("Tips"), tr("failed!"));
        return;
    }
    m_c->reloadFromSettings();
    m_c->connectPc();
    EventLog::key(QStringLiteral("MAINT"), QStringLiteral("restore default data"));
    MsgBox::information(this, tr("Tips"), tr("success!"));
}

void AdminDataPage::onExportData()
{
    if (MsgBox::question(this, tr("Tips"), tr("Export compressed encrypted data?"))
        != MsgBox::Yes)
        return;
    if (!m_c->settings()->save())
    {
        MsgBox::warning(this, tr("Tips"), tr("failed!"));
        return;
    }
    QString err;
    if (!DataExchange::exportConfig(m_c->settings(), &err))
    {
        if (err == QLatin1String("no media"))
            MsgBox::warning(this, tr("Tips"), tr("USB/SD not found."));
        else
            MsgBox::warning(this, tr("Tips"), tr("failed!"));
        return;
    }
    EventLog::key(QStringLiteral("MAINT"), QStringLiteral("export data"));
    MsgBox::information(this, tr("Tips"),
                             tr("Exported to %1").arg(DataExchange::configPackPath()));
}

void AdminDataPage::onImportData()
{
    if (MsgBox::question(this, tr("Tips"),
                              tr("Import data? Current settings will be overwritten."))
        != MsgBox::Yes)
        return;
    QString err;
    if (!DataExchange::importConfig(m_c->settings(), &err))
    {
        if (err == QLatin1String("no media"))
            MsgBox::warning(this, tr("Tips"), tr("USB/SD not found."));
        else if (err == DataExchange::configPackPath())
            MsgBox::warning(this, tr("Tips"), tr("Pack file not found."));
        else
            MsgBox::warning(this, tr("Tips"), tr("failed!"));
        return;
    }
    m_c->reloadFromSettings();
    m_c->connectPc();
    EventLog::key(QStringLiteral("MAINT"), QStringLiteral("import data"));
    MsgBox::information(this, tr("Tips"), tr("success!"));
}

void AdminDataPage::onExportRecords()
{
    if (MsgBox::question(this, tr("Tips"),
                              tr("Export records and logs (compressed, encrypted)?"))
        != MsgBox::Yes)
        return;
    m_c->records()->save();
    QString err;
    if (!DataExchange::exportRecords(m_c->records(), &err))
    {
        if (err == QLatin1String("no media"))
            MsgBox::warning(this, tr("Tips"), tr("USB/SD not found."));
        else
            MsgBox::warning(this, tr("Tips"), tr("failed!"));
        return;
    }
    EventLog::key(QStringLiteral("MAINT"), QStringLiteral("export records"));
    MsgBox::information(this, tr("Tips"),
                             tr("Exported to %1").arg(DataExchange::recordsPackPath()));
}
