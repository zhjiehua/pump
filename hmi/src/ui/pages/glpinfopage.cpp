#include "ui/pages/glpinfopage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/comboctrl.h"
#include "ui/widgets/pagescroll.h"
#include "utils/hmikeys.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
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

QHBoxLayout *capValueRow(QLabel **cap, QLabel **value, int capStretch, int valueStretch)
{
    auto *row = new QHBoxLayout;
    *cap = new QLabel;
    (*cap)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    *value = new QLabel;
    (*value)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    row->addWidget(*cap, capStretch);
    row->addWidget(*value, valueStretch);
    return row;
}

} // namespace

GlpInfoPage::GlpInfoPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(6, 6, 6, 6);
    root->setSpacing(6);

    root->addLayout(capValueRow(&m_manufCap, &m_manuf, 20, 92), 1);
    root->addStretch(1);
    root->addLayout(capValueRow(&m_instCap, &m_inst, 20, 92), 1);
    root->addStretch(1);
    root->addLayout(capValueRow(&m_repairCap, &m_repair, 20, 92), 1);
    root->addStretch(1);
    root->addLayout(capValueRow(&m_usedCap, &m_used, 20, 92), 1);
    root->addStretch(1);
    root->addLayout(capValueRow(&m_bugleCap, &m_bugle, 2, 7), 1);
    root->addStretch(1);

    auto *fluidRow = new QHBoxLayout;
    m_fluidCap = new QLabel;
    m_fluidCap->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_totalFluid = new QLabel;
    m_totalFluid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto *fluidUnit = new QLabel(QStringLiteral("mL"));
    fluidUnit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    fluidRow->addWidget(m_fluidCap, 2);
    fluidRow->addWidget(m_totalFluid, 2);
    fluidRow->addWidget(fluidUnit, 5);
    root->addLayout(fluidRow, 1);
    root->addStretch(1);

    auto *pumpRow = new QHBoxLayout;
    m_pumpCap = new QLabel;
    m_pumpCap->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pump = new ComboCtrl;
    m_pump->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    const char *types[] = {"10", "50", "100", "150", "250", "300",
                           "500", "800", "1000", "2000", "3000"};
    for (int i = 0; i < 11; ++i)
        m_pump->addItem(QString::fromLatin1(types[i]));
    auto *pumpUnit = new QLabel(QStringLiteral("mL"));
    pumpUnit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pumpRow->addWidget(m_pumpCap, 2);
    pumpRow->addWidget(m_pump, 2);
    pumpRow->addWidget(pumpUnit, 5);
    root->addLayout(pumpRow, 1);

    installPageScroll(this, inner);
    retranslateUi();
    loadFromSettings();

    m_pump->installEventFilter(this);
    m_pump->setChangeLocked(true);
    connect(m_pump, SIGNAL(currentIndexChanged(int)), this, SLOT(onPumpChanged(int)));
    connect(m_pump, SIGNAL(popupChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_pump, SIGNAL(changeBlocked()), this, SLOT(onPumpAuthNeeded()));

    auto *tick = new QTimer(this);
    connect(tick, SIGNAL(timeout()), this, SLOT(onTick()));
    tick->start(1000);
}

void GlpInfoPage::initFocusList()
{
    xList.append(m_pump);
    yList.append(m_pump);
    m_pwdNeed = !m_main->consumeLoginOkFor(MainWindow::Glp);
    m_pump->setChangeLocked(m_pwdNeed);
    loadFromSettings();
    if (!m_pwdNeed && m_resumePump)
        QTimer::singleShot(0, this, SLOT(resumePumpEdit()));
}

void GlpInfoPage::retranslateUi()
{
    m_manufCap->setText(tr("Manuf Date:"));
    m_instCap->setText(tr("Install Date:"));
    m_repairCap->setText(tr("La Rep Date:"));
    m_usedCap->setText(tr("Used Time:"));
    m_bugleCap->setText(tr("Pump Run Count:"));
    m_fluidCap->setText(tr("Total Fluid:"));
    m_pumpCap->setText(tr("Pump Type:"));
    updateUsedTime();
}

bool GlpInfoPage::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_pump && event->type() == QEvent::KeyPress && m_pwdNeed)
    {
        auto *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == KEY_RETURN || ke->key() == Qt::Key_Enter)
        {
            requestPumpAuth();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void GlpInfoPage::requestPumpAuth()
{
    if (!m_pwdNeed)
        return;
    m_resumePump = true;
    m_main->requestPasswordThen(MainWindow::Glp, true);
}

void GlpInfoPage::onPumpAuthNeeded()
{
    requestPumpAuth();
}

void GlpInfoPage::resumePumpEdit()
{
    if (!m_resumePump || m_pwdNeed)
        return;
    m_resumePump = false;
    m_pump->setFocus(Qt::OtherFocusReason);
    m_pump->showPopup();
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
    m_used->setText(QString::number(days) + tr("Day") + QStringLiteral("  ")
                    + QString::number(hours) + tr("Hour") + QStringLiteral("  ")
                    + QString::number(mins) + tr("Min") + QStringLiteral("  ")
                    + QString::number(secs) + tr("Sec"));
}

void GlpInfoPage::updateFluidLabels()
{
    auto *s = m_c->settings();
    m_bugle->setText(QString::number(s->bugleCnt));
    m_totalFluid->setText(QString::number(fluidVolumeMl(s->pumpType, s->bugleCnt)));
}

void GlpInfoPage::onPumpChanged(int index)
{
    if (m_pwdNeed)
    {
        m_pump->blockSignals(true);
        m_pump->setCurrentIndex(qBound(0, m_c->settings()->pumpType, 10));
        m_pump->blockSignals(false);
        requestPumpAuth();
        return;
    }

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
