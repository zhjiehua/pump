#include "ui/pages/configsystempage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/comboctrl.h"
#include "ui/widgets/pagescroll.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

ConfigSystemPage::ConfigSystemPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *page = new QVBoxLayout(inner);
    page->setContentsMargins(4, 4, 4, 4);
    page->setSpacing(6);

    auto *form = new QFormLayout;
    form->setSpacing(4);
    m_scale = new ComboCtrl;
    m_scale->addItems({QStringLiteral("1x"), QStringLiteral("2x"), QStringLiteral("3x")});
    m_pathLbl = new QLabel;
    m_pathLbl->setWordWrap(true);
    m_pathLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_scaleLabel = new QLabel;
    m_jsonLabel = new QLabel;
    form->addRow(m_scaleLabel, m_scale);
    form->addRow(m_jsonLabel, m_pathLbl);
    page->addLayout(form);

    auto *btns = new QHBoxLayout;
    m_save = new QPushButton(tr("Save"));
    m_reconnect = new QPushButton(tr("Reconnect"));
    m_debug = new QPushButton(tr("MCU Debug"));
    btns->addWidget(m_save);
    btns->addWidget(m_reconnect);
    btns->addWidget(m_debug);
    page->addLayout(btns);

    installPageScroll(this, inner);
    retranslateUi();
    loadFromSettings();

    connect(m_scale, SIGNAL(popupChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_scale, SIGNAL(currentIndexChanged(int)), this, SLOT(onScaleChanged(int)));
    connect(m_save, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(m_reconnect, SIGNAL(clicked()), this, SLOT(onReconnect()));
    connect(m_debug, SIGNAL(clicked()), this, SLOT(onDebug()));
}

void ConfigSystemPage::initFocusList()
{
    xList.append(m_scale);
    xList.append(m_save);
    xList.append(m_reconnect);
    xList.append(m_debug);
    yList = xList;
    loadFromSettings();
}

void ConfigSystemPage::retranslateUi()
{
    if (m_scaleLabel)
        m_scaleLabel->setText(tr("UI scale"));
    if (m_jsonLabel)
        m_jsonLabel->setText(tr("JSON config"));
    m_save->setText(tr("Save"));
    m_reconnect->setText(tr("Reconnect"));
    m_debug->setText(tr("MCU Debug"));
}

void ConfigSystemPage::onSave()
{
    auto *s = m_c->settings();
    s->scale = m_scale->currentIndex() + 1;
    if (s->save())
        m_c->postLog(tr("Config saved (JSON + .bak)"));
    else
        m_c->postLog(tr("Config save failed"));
    loadFromSettings();
    if (m_main)
        m_main->rebuildScale();
}

void ConfigSystemPage::onReconnect()
{
    auto *s = m_c->settings();
    s->scale = m_scale->currentIndex() + 1;
    s->save();
    if (m_main)
        m_main->rebuildScale();
    m_c->disconnectMcu();
    m_c->disconnectPc();
    m_c->connectMcu();
    m_c->connectPc();
}

void ConfigSystemPage::onDebug()
{
    m_main->go(MainWindow::DebugMcu);
}

void ConfigSystemPage::onScaleChanged(int i)
{
    m_c->settings()->scale = i + 1;
    if (m_main)
        m_main->rebuildScale();
}

void ConfigSystemPage::loadFromSettings()
{
    auto *s = m_c->settings();
    m_scale->setCurrentIndex(qBound(0, s->scale - 1, 2));
    m_pathLbl->setText(s->deviceInfoPath() + QStringLiteral("\n")
                       + s->configPath() + QStringLiteral("\n")
                       + s->dataPath());
}
