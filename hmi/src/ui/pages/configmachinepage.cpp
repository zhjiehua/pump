#include "ui/pages/configmachinepage.h"
#include "core/appsettings.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/comboctrl.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/pagescroll.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

ConfigMachinePage::ConfigMachinePage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *page = new QVBoxLayout(inner);
    page->setContentsMargins(4, 4, 4, 4);
    page->setSpacing(6);

    auto *form = new QFormLayout;
    form->setContentsMargins(6, 8, 6, 6);
    form->setSpacing(4);

    m_pumpTypeLabel = new QLabel;
    m_pumpType = new ComboCtrl;
    for (const char *name : {"10mL", "50mL", "100mL", "150mL", "250mL", "300mL",
                             "500mL", "800mL", "1000mL", "2000mL", "3000mL"})
        m_pumpType->addItem(QString::fromLatin1(name));
    m_wordFactorLabel = new QLabel;
    m_wordFactor = new EditCtrl;
    m_wordFactor->setValRange(0, 1e12, 6);
    m_pressScaleLabel = new QLabel;
    m_pressScale = new EditCtrl;
    m_pressScale->setValRange(0, 1000, 8);
    m_pmaxLabel = new QLabel;
    m_pmax = new EditCtrl;
    m_pmax->setValRange(0, 200, 2);
    m_pmaxField = new QWidget;
    auto *pmaxLay = new QHBoxLayout(m_pmaxField);
    pmaxLay->setContentsMargins(0, 0, 0, 0);
    pmaxLay->setSpacing(4);
    pmaxLay->addWidget(m_pmax, 1);
    auto *mpa = new QLabel(QStringLiteral("MPa"));
    pmaxLay->addWidget(mpa);

    form->addRow(m_pumpTypeLabel, m_pumpType);
    form->addRow(m_wordFactorLabel, m_wordFactor);
    form->addRow(m_pressScaleLabel, m_pressScale);
    form->addRow(m_pmaxLabel, m_pmaxField);
    page->addLayout(form);

    auto *btns = new QHBoxLayout;
    m_save = new QPushButton;
    btns->addStretch(1);
    btns->addWidget(m_save);
    btns->addStretch(1);
    page->addLayout(btns);

    installPageScroll(this, inner);
    retranslateUi();
    loadFromSettings();

    connect(m_wordFactor, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_pressScale, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_pmax, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_pumpType, SIGNAL(popupChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_pumpType, SIGNAL(currentIndexChanged(int)), this, SLOT(onPumpTypeChanged(int)));
    connect(m_save, SIGNAL(clicked()), this, SLOT(onSave()));
}

void ConfigMachinePage::initFocusList()
{
    xList.append(m_pumpType);
    xList.append(m_wordFactor);
    xList.append(m_pressScale);
    xList.append(m_pmax);
    xList.append(m_save);
    yList = xList;
    loadFromSettings();
    updateMcuDrivenFields();
}

void ConfigMachinePage::retranslateUi()
{
    m_pumpTypeLabel->setText(tr("Pump type"));
    m_wordFactorLabel->setText(tr("Word factor"));
    m_pressScaleLabel->setText(tr("Press raw scale"));
    m_pmaxLabel->setText(tr("Pmax:"));
    m_save->setText(tr("Save"));
}

void ConfigMachinePage::onPumpTypeChanged(int i)
{
    AppSettings tmp;
    tmp.pumpType = i;
    tmp.applyPumpTypeFactor();
    m_wordFactor->setText(QString::number(tmp.mcuWordFactor, 'g', 12));
}

void ConfigMachinePage::onSave()
{
    applyToSettings();
    if (m_c->settings()->save())
        m_c->postLog(tr("Config saved (JSON + .bak)"));
    else
        m_c->postLog(tr("Config save failed"));
    m_c->reloadFromSettings();
}

void ConfigMachinePage::loadFromSettings()
{
    auto *s = m_c->settings();
    m_pumpType->setCurrentIndex(qBound(0, s->pumpType, 10));
    m_wordFactor->setText(QString::number(s->mcuWordFactor, 'g', 12));
    m_pressScale->setText(QString::number(s->pressRawScale, 'g', 8));
    const double pmax = s->pmaxLimit > 0 ? s->pmaxLimit : s->pressMax;
    m_pmax->setText(QString::number(pmax, 'f', 0));
    updateMcuDrivenFields();
}

void ConfigMachinePage::updateMcuDrivenFields()
{
    const bool qf = m_c->settings()->mcuProtocol == AppSettings::QinFine;
    m_pumpType->setEnabled(!qf);
    m_wordFactor->setEnabled(!qf);
    m_pressScale->setEnabled(!qf);
}

void ConfigMachinePage::applyToSettings()
{
    auto *s = m_c->settings();
    const int newType = m_pumpType->currentIndex();
    const bool typeChanged = newType != s->pumpType;
    s->pumpType = newType;
    if (typeChanged)
    {
        s->applyPumpTypeFactor();
        s->restoreDefaultCalibrationTables();
    }
    s->mcuWordFactor = m_wordFactor->text().toDouble();
    s->pressRawScale = m_pressScale->text().toDouble();
    if (s->pressRawScale <= 0)
        s->pressRawScale = 0.0128;
    s->pmaxLimit = m_pmax->text().toDouble();
    if (s->pmaxLimit > 0)
        s->pressMax = s->pmaxLimit;
}
