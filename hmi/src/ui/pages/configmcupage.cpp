#include "ui/pages/configmcupage.h"
#include "ui/pages/configpageutil.h"
#include "core/appsettings.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/comboctrl.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/pagescroll.h"
#include "utils/qtwidgetsutil.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

ConfigMcuPage::ConfigMcuPage(MachineController *c, MainWindow *main, QWidget *parent)
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

    m_mcuTypeLabel = new QLabel;
    m_mcuProto = new ComboCtrl;
    m_mcuProto->addItem(QStringLiteral("CXTH"), int(AppSettings::Cxth));
    m_mcuProto->addItem(QStringLiteral("QinFine"), int(AppSettings::QinFine));
    m_mcuPortLabel = new QLabel;
    m_mcuPort = new ComboCtrl;
    m_mcuPort->setEditable(true);
    m_mcuBaudLabel = new QLabel;
    m_mcuBaud = new EditCtrl;
    m_mcuBaud->setValRange(0, 10000000, 0);
    m_mcuAddrLabel = new QLabel;
    m_mcuAddr = new EditCtrl;
    m_mcuAddr->setValRange(0, 255, 0, true);

    form->addRow(m_mcuTypeLabel, m_mcuProto);
    form->addRow(m_mcuPortLabel, m_mcuPort);
    form->addRow(m_mcuBaudLabel, m_mcuBaud);
    form->addRow(m_mcuAddrLabel, m_mcuAddr);
    page->addLayout(form);

    auto *btns = new QHBoxLayout;
    m_save = new QPushButton;
    btns->addStretch(1);
    btns->addWidget(m_save);
    btns->addStretch(1);
    page->addLayout(btns);

    installPageScroll(this, inner);
    retranslateUi();
    refreshSerialComboList(m_mcuPort, nullptr);
    loadFromSettings();

    connect(m_mcuBaud, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_mcuAddr, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_mcuProto, SIGNAL(popupChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_mcuPort, SIGNAL(popupChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_mcuProto, SIGNAL(currentIndexChanged(int)), this, SLOT(onMcuProtocolChanged(int)));
    connect(m_save, SIGNAL(clicked()), this, SLOT(onSave()));
}

void ConfigMcuPage::initFocusList()
{
    xList.append(m_mcuProto);
    xList.append(m_mcuPort);
    xList.append(m_mcuBaud);
    xList.append(m_mcuAddr);
    xList.append(m_save);
    yList = xList;
    refreshSerialComboList(m_mcuPort, nullptr);
    loadFromSettings();
}

void ConfigMcuPage::retranslateUi()
{
    m_mcuTypeLabel->setText(tr("Protocol"));
    m_mcuPortLabel->setText(tr("Port"));
    m_mcuBaudLabel->setText(tr("Baud"));
    m_mcuAddrLabel->setText(tr("Addr"));
    m_save->setText(tr("Save"));
}

void ConfigMcuPage::onMcuProtocolChanged(int)
{
    updateMcuFieldVisibility();
}

void ConfigMcuPage::onSave()
{
    applyToSettings();
    if (m_c->settings()->save())
        m_c->postLog(tr("Config saved (JSON + .bak)"));
    else
        m_c->postLog(tr("Config save failed"));
    m_c->reloadFromSettings();
}

void ConfigMcuPage::loadFromSettings()
{
    auto *s = m_c->settings();
    int mcuIdx = m_mcuProto->findData(int(s->mcuProtocol));
    if (mcuIdx < 0)
        mcuIdx = 0;
    m_mcuProto->setCurrentIndex(mcuIdx);
    hmiComboSetCurrentText(m_mcuPort, s->mcuPort);
    m_mcuBaud->setText(QString::number(s->mcuBaud));
    m_mcuAddr->setText(QStringLiteral("0x%1").arg(s->mcuAddress, 2, 16, QChar('0')));
    updateMcuFieldVisibility();
}

void ConfigMcuPage::updateMcuFieldVisibility()
{
    const bool qf = hmiComboCurrentData(m_mcuProto).toInt() == int(AppSettings::QinFine);
    setConfigFormRowVisible(m_mcuAddrLabel, m_mcuAddr, qf);
    if (qf && m_mcuBaud->text() == QStringLiteral("9600"))
        m_mcuBaud->setText(QStringLiteral("115200"));
    else if (!qf && m_mcuBaud->text() == QStringLiteral("115200"))
        m_mcuBaud->setText(QStringLiteral("9600"));
}

void ConfigMcuPage::applyToSettings()
{
    auto *s = m_c->settings();
    s->mcuProtocol = AppSettings::McuProtocol(hmiComboCurrentData(m_mcuProto).toInt());
    s->mcuPort = m_mcuPort->currentText().trimmed();
    s->mcuBaud = m_mcuBaud->text().toInt();
    bool ok = false;
    s->mcuAddress = quint8(m_mcuAddr->text().trimmed().toUInt(&ok, 0));
    if (!ok)
        s->mcuAddress = 0x01;
}
