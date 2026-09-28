#include "ui/pages/glpinfopage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/pagescroll.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

namespace {

QString dateLabel(const QString &y, const QString &mo, const QString &d)
{
    return y + QChar('-') + mo + QChar('-') + d;
}

double fluidVolumeMl(int pumpType, quint32 bugleCnt)
{
    static const double kVol[] = {
        0.0608057, 0.2481608, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
    const int i = qBound(0, pumpType % 11, 10);
    return kVol[i] * bugleCnt;
}

} // namespace

GlpInfoPage::GlpInfoPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(8, 8, 8, 8);

    auto *form = new QFormLayout;
    m_pump = new QComboBox;
    for (const char *name : {"10mL", "50mL", "100mL", "150mL", "250mL", "300mL",
                             "500mL", "800mL", "1000mL", "2000mL", "3000mL"})
        m_pump->addItem(QString::fromLatin1(name));

    m_manuf = new QLabel;
    m_inst = new QLabel;
    m_repair = new QLabel;
    m_used = new QLabel;
    m_bugle = new QLabel;
    m_totalFluid = new QLabel;

    form->addRow(tr("Pump type"), m_pump);
    form->addRow(tr("Manufacture"), m_manuf);
    form->addRow(tr("Install"), m_inst);
    form->addRow(tr("Repair"), m_repair);
    form->addRow(tr("Used time"), m_used);
    form->addRow(tr("Bugle count"), m_bugle);
    form->addRow(tr("Total fluid"), m_totalFluid);
    root->addLayout(form);

    auto *btns = new QHBoxLayout;
    auto *back = new QPushButton(tr("Back"));
    btns->addStretch(1);
    btns->addWidget(back);
    root->addLayout(btns);
    root->addStretch(1);

    installPageScroll(this, inner);

    loadFromSettings();

    connect(m_pump, SIGNAL(currentIndexChanged(int)), this, SLOT(onPumpChanged(int)));
    connect(back, SIGNAL(clicked()), this, SLOT(onBack()));

    auto *tick = new QTimer(this);
    connect(tick, SIGNAL(timeout()), this, SLOT(onTick()));
    tick->start(1000);
}

void GlpInfoPage::initFocusList()
{
    xList.append(m_pump);
    yList.append(m_pump);
}

void GlpInfoPage::onBack()
{
    m_main->goBack();
}

void GlpInfoPage::onTick()
{
    updateUsedTime();
    updateFluidLabels();
}

void GlpInfoPage::loadFromSettings()
{
    auto *s = m_c->settings();
    m_pump->blockSignals(true);
    m_pump->setCurrentIndex(qBound(0, s->pumpType, 10));
    m_pump->blockSignals(false);

    m_manuf->setText(dateLabel(s->manufYear, s->manufMonth, s->manufDay));
    m_inst->setText(dateLabel(s->instYear, s->instMonth, s->instDay));
    m_repair->setText(dateLabel(s->repairYear, s->repairMonth, s->repairDay));
    updateUsedTime();
    updateFluidLabels();
}

void GlpInfoPage::updateUsedTime()
{
    const quint32 sec = m_c->settings()->sysUsedSec;
    const int days = int(sec / 86400);
    const int hours = int((sec % 86400) / 3600);
    const int mins = int((sec % 3600) / 60);
    const int secs = int(sec % 60);
    m_used->setText(QStringLiteral("%1%2  %3%4  %5%6  %7%8")
                        .arg(days)
                        .arg(tr("Day"))
                        .arg(hours)
                        .arg(tr("Hour"))
                        .arg(mins)
                        .arg(tr("Min"))
                        .arg(secs)
                        .arg(tr("Sec")));
}

void GlpInfoPage::updateFluidLabels()
{
    auto *s = m_c->settings();
    m_bugle->setText(QString::number(s->bugleCnt));
    m_totalFluid->setText(QString::number(fluidVolumeMl(s->pumpType, s->bugleCnt), 'f', 4));
}

void GlpInfoPage::onPumpChanged(int index)
{
    auto *s = m_c->settings();
    s->pumpType = index;
    s->applyPumpTypeFactor();
    s->pressMax = s->defaultMaxPressForPump();
    if (s->pressMin > s->pressMax)
        s->pressMin = 0;
    s->save();
    m_c->setPressLimits(s->pressMin, s->pressMax);
    updateFluidLabels();
}
