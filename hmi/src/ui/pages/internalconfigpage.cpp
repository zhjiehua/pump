#include "ui/pages/internalconfigpage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/pagescroll.h"

#include <QComboBox>
#include <QWidget>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include "platform/hmiserialport.h"
#include "utils/qtwidgetsutil.h"
#include <QVBoxLayout>

InternalConfigPage::InternalConfigPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto *scroll = makePageScroll();
    auto *inner = new QWidget;
    auto *form = new QFormLayout(inner);
    form->setContentsMargins(4, 4, 4, 4);
    form->setSpacing(4);

    m_mcuProto = new QComboBox;
    m_mcuProto->addItem(tr("Legacy MCU (0x80)"), int(AppSettings::Legacy));
    m_mcuProto->addItem(tr("QinFine (:…!)"), int(AppSettings::QinFine));

    m_mcuPort = new QComboBox;
    m_mcuPort->setEditable(true);
    m_mcuBaud = new QLineEdit;
    m_mcuAddr = new QLineEdit;
    m_pumpType = new QComboBox;
    for (const char *name : {"10mL", "50mL", "100mL", "150mL", "250mL", "300mL",
                             "500mL", "800mL", "1000mL", "2000mL", "3000mL"})
        m_pumpType->addItem(QString::fromLatin1(name));
    m_wordFactor = new QLineEdit;
    m_pressScale = new QLineEdit;

    m_pcProto = new QComboBox;
    m_pcProto->addItem(tr("CXTH legacy"), int(AppSettings::LegacyPc));
    m_pcProto->addItem(tr("Clarity"), int(AppSettings::Clarity));
    m_pcPortType = new QComboBox;
    m_pcPortType->addItem(tr("RS232"), int(AppSettings::Serial));
    m_pcPortType->addItem(tr("UDP"), int(AppSettings::Udp));
    m_pcSerial = new QComboBox;
    m_pcSerial->setEditable(true);
    m_pcBaud = new QLineEdit;
    m_localUdp = new QLineEdit;
    m_remoteIp = new QLineEdit;
    m_remotePort = new QLineEdit;
    m_machineCode = new QLineEdit;
    m_scale = new QComboBox;
    m_scale->addItems({QStringLiteral("1x"), QStringLiteral("2x"), QStringLiteral("3x")});
    m_pathLbl = new QLabel;
    m_pathLbl->setWordWrap(true);
    m_pathLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);

    form->addRow(tr("MCU protocol"), m_mcuProto);
    form->addRow(tr("MCU port"), m_mcuPort);
    form->addRow(tr("MCU baud"), m_mcuBaud);
    form->addRow(tr("MCU addr (QinFine)"), m_mcuAddr);
    form->addRow(tr("Pump type (Legacy)"), m_pumpType);
    form->addRow(tr("Word factor"), m_wordFactor);
    form->addRow(tr("Press raw scale"), m_pressScale);
    form->addRow(tr("CDS protocol"), m_pcProto);
    form->addRow(tr("CDS link"), m_pcPortType);
    form->addRow(tr("CDS serial"), m_pcSerial);
    form->addRow(tr("CDS baud"), m_pcBaud);
    form->addRow(tr("Local UDP"), m_localUdp);
    form->addRow(tr("Remote IP"), m_remoteIp);
    form->addRow(tr("Remote port"), m_remotePort);
    form->addRow(tr("Device code (hex)"), m_machineCode);
    form->addRow(tr("UI scale"), m_scale);
    form->addRow(tr("JSON config"), m_pathLbl);

    scroll->setWidget(inner);
    root->addWidget(scroll, 1);

    auto *btns = new QHBoxLayout;
    m_save = new QPushButton(tr("Save"));
    m_reconnect = new QPushButton(tr("Reconnect"));
    m_debug = new QPushButton(tr("MCU Debug"));
    m_back = new QPushButton(tr("Back"));
    btns->addWidget(m_save);
    btns->addWidget(m_reconnect);
    btns->addWidget(m_debug);
    btns->addWidget(m_back);
    root->addLayout(btns);

    refreshPortLists();
    loadFromSettings();

    connect(m_mcuProto, SIGNAL(currentIndexChanged(int)), this, SLOT(onMcuProtocolChanged(int)));
    connect(m_pumpType, SIGNAL(currentIndexChanged(int)), this, SLOT(onPumpTypeChanged(int)));
    connect(m_save, SIGNAL(clicked()), this, SLOT(onSave()));
    connect(m_reconnect, SIGNAL(clicked()), this, SLOT(onReconnect()));
    connect(m_debug, SIGNAL(clicked()), this, SLOT(onDebug()));
    connect(m_scale, SIGNAL(currentIndexChanged(int)), this, SLOT(onScaleChanged(int)));
    connect(m_back, SIGNAL(clicked()), this, SLOT(onBack()));
}

void InternalConfigPage::initFocusList()
{
    const QList<QWidget *> fields = {
        m_mcuProto, m_mcuPort,   m_mcuBaud,    m_mcuAddr,    m_pumpType, m_wordFactor,
        m_pressScale, m_pcProto, m_pcPortType, m_pcSerial,   m_pcBaud,   m_localUdp,
        m_remoteIp,   m_remotePort, m_machineCode, m_scale,
    };
    for (QWidget *w : fields)
        xList.append(w);
    xList.append(m_save);
    xList.append(m_reconnect);
    xList.append(m_debug);
    xList.append(m_back);
    yList = xList;
}

void InternalConfigPage::onPumpTypeChanged(int i)
{
    AppSettings tmp;
    tmp.pumpType = i;
    tmp.applyPumpTypeFactor();
    m_wordFactor->setText(QString::number(tmp.mcuWordFactor, 'g', 12));
}

void InternalConfigPage::onSave()
{
    applyToSettings();
    if (m_c->settings()->save())
        m_c->postLog(tr("Config saved (JSON + .bak)"));
    else
        m_c->postLog(tr("Config save failed"));
    auto *s = m_c->settings();
    m_pathLbl->setText(s->deviceInfoPath() + QStringLiteral("\n")
                       + s->configPath() + QStringLiteral("\n")
                       + s->dataPath());
    if (m_main)
        m_main->rebuildScale();
}

void InternalConfigPage::onReconnect()
{
    applyToSettings();
    m_c->settings()->save();
    if (m_main)
        m_main->rebuildScale();
    m_c->disconnectMcu();
    m_c->disconnectPc();
    m_c->connectMcu();
    m_c->connectPc();
}

void InternalConfigPage::onDebug()
{
    m_main->go(MainWindow::DebugMcu);
}

void InternalConfigPage::onScaleChanged(int i)
{
    m_c->settings()->scale = i + 1;
    if (m_main)
        m_main->rebuildScale();
}

void InternalConfigPage::onBack()
{
    m_main->goBack();
}

void InternalConfigPage::refreshPortLists()
{
    const QString mcuCur = m_mcuPort->currentText();
    const QString pcCur = m_pcSerial->currentText();
    m_mcuPort->clear();
    m_pcSerial->clear();
    foreach (const QString &p, HmiSerialPortInfo::availablePortNames())
    {
        m_mcuPort->addItem(p);
        m_pcSerial->addItem(p);
    }
    if (!mcuCur.isEmpty())
        hmiComboSetCurrentText(m_mcuPort, mcuCur);
    if (!pcCur.isEmpty())
        hmiComboSetCurrentText(m_pcSerial, pcCur);
}

void InternalConfigPage::loadFromSettings()
{
    auto *s = m_c->settings();
    m_mcuProto->setCurrentIndex(int(s->mcuProtocol));
    hmiComboSetCurrentText(m_mcuPort, s->mcuPort);
    m_mcuBaud->setText(QString::number(s->mcuBaud));
    m_mcuAddr->setText(QStringLiteral("0x%1").arg(s->mcuAddress, 2, 16, QChar('0')));
    m_pumpType->setCurrentIndex(qBound(0, s->pumpType, 10));
    m_wordFactor->setText(QString::number(s->mcuWordFactor, 'g', 12));
    m_pressScale->setText(QString::number(s->pressRawScale, 'g', 8));
    m_pcProto->setCurrentIndex(int(s->pcProtocol));
    m_pcPortType->setCurrentIndex(int(s->pcPort));
    hmiComboSetCurrentText(m_pcSerial, s->pcSerialPort);
    m_pcBaud->setText(QString::number(s->pcSerialBaud));
    m_localUdp->setText(QString::number(s->localUdpPort));
    m_remoteIp->setText(s->remoteIp);
    m_remotePort->setText(QString::number(s->remotePort));
    m_machineCode->setText(QString::number(s->machineCode, 16));
    m_scale->setCurrentIndex(qBound(0, s->scale - 1, 2));
    m_pathLbl->setText(s->deviceInfoPath() + QStringLiteral("\n")
                       + s->configPath() + QStringLiteral("\n")
                       + s->dataPath());
    onMcuProtocolChanged(m_mcuProto->currentIndex());
}

void InternalConfigPage::onMcuProtocolChanged(int index)
{
    const bool qf = (index == int(AppSettings::QinFine));
    m_mcuAddr->setEnabled(qf);
    m_pumpType->setEnabled(!qf);
    m_wordFactor->setEnabled(!qf);
    m_pressScale->setEnabled(!qf);
    // Suggest baud when user switches protocol (do not override if they already typed).
    if (qf && m_mcuBaud->text() == QStringLiteral("9600"))
        m_mcuBaud->setText(QStringLiteral("115200"));
    else if (!qf && m_mcuBaud->text() == QStringLiteral("115200"))
        m_mcuBaud->setText(QStringLiteral("9600"));
}

void InternalConfigPage::applyToSettings()
{
    auto *s = m_c->settings();
    s->mcuProtocol = AppSettings::McuProtocol(hmiComboCurrentData(m_mcuProto).toInt());
    s->mcuPort = m_mcuPort->currentText().trimmed();
    s->mcuBaud = m_mcuBaud->text().toInt();
    bool ok = false;
    s->mcuAddress = quint8(m_mcuAddr->text().trimmed().toUInt(&ok, 0));
    if (!ok)
        s->mcuAddress = 0x01;
    s->pumpType = m_pumpType->currentIndex();
    s->mcuWordFactor = m_wordFactor->text().toDouble();
    s->pressRawScale = m_pressScale->text().toDouble();
    if (s->pressRawScale <= 0)
        s->pressRawScale = 0.0128;

    s->pcProtocol = AppSettings::PcProtocol(hmiComboCurrentData(m_pcProto).toInt());
    s->pcPort = AppSettings::PcPort(hmiComboCurrentData(m_pcPortType).toInt());
    s->pcSerialPort = m_pcSerial->currentText().trimmed();
    s->pcSerialBaud = m_pcBaud->text().toInt();
    s->localUdpPort = quint16(m_localUdp->text().toUInt());
    s->remoteIp = m_remoteIp->text().trimmed();
    s->remotePort = quint16(m_remotePort->text().toUInt());
    s->machineCode = quint8(m_machineCode->text().toUInt(nullptr, 16));
    s->scale = m_scale->currentIndex() + 1;
}
