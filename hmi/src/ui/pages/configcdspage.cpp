#include "ui/pages/configcdspage.h"
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

ConfigCdsPage::ConfigCdsPage(MachineController *c, MainWindow *main, QWidget *parent)
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

    m_cdsProtoLabel = new QLabel;
    m_pcProto = new ComboCtrl;
    m_pcProto->addItem(QStringLiteral("CXTH"), int(AppSettings::CxthPc));
    m_pcProto->addItem(QStringLiteral("Clarity"), int(AppSettings::Clarity));
    m_pcProto->addItem(QStringLiteral("QinFine"), int(AppSettings::QinFinePc));
    m_cdsLinkLabel = new QLabel;
    m_pcPortType = new ComboCtrl;
    m_pcPortType->addItem(QStringLiteral("RS232"), int(AppSettings::Serial));
    m_pcPortType->addItem(QStringLiteral("UDP"), int(AppSettings::Udp));
    m_pcPortType->addItem(QStringLiteral("TCP Server"), int(AppSettings::TcpServer));
    m_pcSerialLabel = new QLabel;
    m_pcSerial = new ComboCtrl;
    m_pcSerial->setEditable(true);
    m_pcBaudLabel = new QLabel;
    m_pcBaud = new EditCtrl;
    m_pcBaud->setValRange(0, 10000000, 0);
    m_machineCodeLabel = new QLabel;
    m_machineCode = new EditCtrl;
    m_machineCode->setValRange(0, 255, 0, true);

    form->addRow(m_cdsProtoLabel, m_pcProto);
    form->addRow(m_cdsLinkLabel, m_pcPortType);
    form->addRow(m_pcSerialLabel, m_pcSerial);
    form->addRow(m_pcBaudLabel, m_pcBaud);
    form->addRow(m_machineCodeLabel, m_machineCode);
    page->addLayout(form);

    auto *btns = new QHBoxLayout;
    m_save = new QPushButton;
    btns->addStretch(1);
    btns->addWidget(m_save);
    btns->addStretch(1);
    page->addLayout(btns);

    installPageScroll(this, inner);
    retranslateUi();
    refreshSerialComboList(nullptr, m_pcSerial);
    loadFromSettings();

    connect(m_pcBaud, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_machineCode, SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    const QList<ComboCtrl *> combos = {m_pcProto, m_pcPortType, m_pcSerial};
    for (ComboCtrl *cb : combos)
        connect(cb, SIGNAL(popupChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_pcProto, SIGNAL(currentIndexChanged(int)), this, SLOT(onPcProtocolChanged(int)));
    connect(m_pcPortType, SIGNAL(currentIndexChanged(int)), this, SLOT(onPcPortTypeChanged(int)));
    connect(m_save, SIGNAL(clicked()), this, SLOT(onSave()));
}

void ConfigCdsPage::initFocusList()
{
    xList.append(m_pcProto);
    xList.append(m_pcPortType);
    xList.append(m_pcSerial);
    xList.append(m_pcBaud);
    xList.append(m_machineCode);
    xList.append(m_save);
    yList = xList;
    refreshSerialComboList(nullptr, m_pcSerial);
    loadFromSettings();
}

void ConfigCdsPage::retranslateUi()
{
    m_cdsProtoLabel->setText(tr("Protocol"));
    m_cdsLinkLabel->setText(tr("Link"));
    m_pcSerialLabel->setText(tr("Serial"));
    m_pcBaudLabel->setText(tr("Baud"));
    m_machineCodeLabel->setText(tr("Device code (hex)"));
    m_save->setText(tr("Save"));
    m_pcProto->setItemText(0, tr("CXTH"));
    m_pcProto->setItemText(1, tr("Clarity"));
    m_pcProto->setItemText(2, tr("QinFine"));
    m_pcPortType->setItemText(0, tr("RS232"));
    m_pcPortType->setItemText(1, tr("UDP"));
    m_pcPortType->setItemText(2, tr("TCP Server"));
}

void ConfigCdsPage::onPcProtocolChanged(int)
{
    updateCdsFieldVisibility();
}

void ConfigCdsPage::onPcPortTypeChanged(int)
{
    updateCdsFieldVisibility();
}

void ConfigCdsPage::onSave()
{
    applyToSettings();
    if (m_c->settings()->save())
        m_c->postLog(tr("Config saved (JSON + .bak)"));
    else
        m_c->postLog(tr("Config save failed"));
}

void ConfigCdsPage::loadFromSettings()
{
    auto *s = m_c->settings();
    m_pcProto->setCurrentIndex(int(s->pcProtocol));
    int pcIdx = m_pcPortType->findData(int(s->pcPort));
    if (pcIdx < 0)
        pcIdx = m_pcPortType->findData(int(AppSettings::Udp));
    m_pcPortType->setCurrentIndex(qMax(0, pcIdx));
    hmiComboSetCurrentText(m_pcSerial, s->pcSerialPort);
    m_pcBaud->setText(QString::number(s->pcSerialBaud));
    m_machineCode->setText(QString::number(s->machineCode, 16));
    updateCdsFieldVisibility();
}

void ConfigCdsPage::updateCdsFieldVisibility()
{
    const bool clarity = hmiComboCurrentData(m_pcProto).toInt() == int(AppSettings::Clarity);
    const bool rs232 = hmiComboCurrentData(m_pcPortType).toInt() == int(AppSettings::Serial);
    setConfigFormRowVisible(m_machineCodeLabel, m_machineCode, clarity);
    setConfigFormRowVisible(m_pcSerialLabel, m_pcSerial, rs232);
    setConfigFormRowVisible(m_pcBaudLabel, m_pcBaud, rs232);
}

void ConfigCdsPage::applyToSettings()
{
    auto *s = m_c->settings();
    s->pcProtocol = AppSettings::PcProtocol(hmiComboCurrentData(m_pcProto).toInt());
    s->pcPort = AppSettings::PcPort(hmiComboCurrentData(m_pcPortType).toInt());
    s->pcSerialPort = m_pcSerial->currentText().trimmed();
    s->pcSerialBaud = m_pcBaud->text().toInt();
    s->machineCode = quint8(m_machineCode->text().toUInt(nullptr, 16));
}
