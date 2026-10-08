#include "ui/pages/admindevicepage.h"
#include "core/appsettings.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/editfield.h"
#include "ui/widgets/msgbox.h"
#include "ui/widgets/pagescroll.h"

#include <QHBoxLayout>
#include <QLabel>
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

AdminDevicePage::AdminDevicePage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *vl = new QVBoxLayout(inner);
    vl->setContentsMargins(6, 0, 6, 0);
    vl->setSpacing(6);

    m_manufYear = new EditCtrl;
    m_manufMonth = new EditCtrl;
    m_manufDay = new EditCtrl;
    m_instYear = new EditCtrl;
    m_instMonth = new EditCtrl;
    m_instDay = new EditCtrl;
    m_license = new EditCtrl;
    m_serial = new EditCtrl;
    expand(m_manufYear);
    expand(m_manufMonth);
    expand(m_manufDay);
    expand(m_instYear);
    expand(m_instMonth);
    expand(m_instDay);
    expand(m_license);
    expand(m_serial);

    m_manufYear->setValRange(1990, 2050, 0);
    m_instYear->setValRange(1990, 2050, 0);
    m_manufMonth->setValRange(1, 12, 0);
    m_instMonth->setValRange(1, 12, 0);
    m_manufDay->setValRange(0, 31, 0);
    m_instDay->setValRange(0, 31, 0);
    m_license->setValRange(0, 9999999999ULL, 0, true);
    m_serial->setValRange(0, 9999999999ULL, 0, true);
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

    auto *licenRow = new QHBoxLayout;
    m_licenCap = new QLabel;
    expand(m_licenCap);
    licenRow->addWidget(m_licenCap, 1);
    licenRow->addWidget(m_license, 5);
    vl->addLayout(licenRow, 1);
    vl->addStretch(1);

    auto *serialRow = new QHBoxLayout;
    m_serialCap = new QLabel;
    expand(m_serialCap);
    serialRow->addWidget(m_serialCap, 1);
    serialRow->addWidget(m_serial, 5);
    vl->addLayout(serialRow, 1);
    vl->addStretch(1);

    m_save = new BtnCtrl;
    expand(m_save);
    auto *saveRow = new QHBoxLayout;
    saveRow->addStretch(1);
    saveRow->addWidget(m_save, 2);
    saveRow->addStretch(1);
    vl->addLayout(saveRow, 1);

    installPageScroll(this, inner);
    retranslateUi();
    loadFromSettings();

    const QList<EditCtrl *> edits = QList<EditCtrl *>()
        << m_manufYear << m_manufMonth << m_manufDay
        << m_instYear << m_instMonth << m_instDay
        << m_license << m_serial;
    for (int i = 0; i < edits.size(); ++i)
    {
        connect(edits.at(i), SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
        connect(edits.at(i), SIGNAL(valueCommitted(QString)), this, SLOT(onFieldChanged()));
    }
    connect(m_save, SIGNAL(clicked()), this, SLOT(onSave()));
}

void AdminDevicePage::initFocusList()
{
    xList.append(m_manufYear);
    xList.append(m_manufMonth);
    xList.append(m_manufDay);
    xList.append(m_instYear);
    xList.append(m_instMonth);
    xList.append(m_instDay);
    xList.append(m_license);
    xList.append(m_serial);
    xList.append(m_save);

    yList.append(m_manufYear);
    yList.append(m_instYear);
    yList.append(m_manufMonth);
    yList.append(m_instMonth);
    yList.append(m_manufDay);
    yList.append(m_instDay);
    yList.append(m_license);
    yList.append(m_serial);
    yList.append(m_save);

    loadFromSettings();
}

void AdminDevicePage::retranslateUi()
{
    m_manufCap->setText(tr("Manuf Date:"));
    m_instCap->setText(tr("Install Date:"));
    m_licenCap->setText(tr("Licen:"));
    m_serialCap->setText(tr("Serial:"));
    m_save->setText(tr("Save"));
}

void AdminDevicePage::onSave()
{
    auto *s = m_c->settings();
    applyToSettings();
    if (s->save())
        MsgBox::information(this, tr("Tips"), tr("success!"));
    else
        MsgBox::warning(this, tr("Tips"), tr("failed!"));
}

void AdminDevicePage::onFieldChanged()
{
    applyToSettings();
    m_c->settings()->save();
}

void AdminDevicePage::loadFromSettings()
{
    auto *s = m_c->settings();
    m_manufYear->setText(s->manufYear);
    m_manufMonth->setText(s->manufMonth);
    m_manufDay->setText(s->manufDay);
    m_instYear->setText(s->instYear);
    m_instMonth->setText(s->instMonth);
    m_instDay->setText(s->instDay);
    m_license->setText(s->license);
    m_serial->setText(s->serial);
}

void AdminDevicePage::applyToSettings()
{
    auto *s = m_c->settings();
    s->manufYear = m_manufYear->text().trimmed();
    s->manufMonth = m_manufMonth->text().trimmed();
    s->manufDay = m_manufDay->text().trimmed();
    s->instYear = m_instYear->text().trimmed();
    s->instMonth = m_instMonth->text().trimmed();
    s->instDay = m_instDay->text().trimmed();
    s->serial = m_serial->text().trimmed();
}
