#include "ui/pages/netpage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/comboctrl.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/editfield.h"
#include "ui/widgets/pagescroll.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace {

void makeIpEdits(EditCtrl **a, EditCtrl **b, EditCtrl **c, EditCtrl **d)
{
    *a = makeEditField();
    *b = makeEditField();
    *c = makeEditField();
    *d = makeEditField();
    (*a)->setValRange(0, 255, 0);
    (*b)->setValRange(0, 255, 0);
    (*c)->setValRange(0, 255, 0);
    (*d)->setValRange(0, 255, 0);
}

QHBoxLayout *ipOctetRow(QLineEdit *a, QLineEdit *b, QLineEdit *c, QLineEdit *d)
{
    auto *row = new QHBoxLayout;
    row->setSpacing(0);
    row->setContentsMargins(0, 0, 0, 0);
    row->addWidget(a, 10);
    row->addWidget(makeSep(QStringLiteral(".")), 1);
    row->addWidget(b, 10);
    row->addWidget(makeSep(QStringLiteral(".")), 1);
    row->addWidget(c, 10);
    row->addWidget(makeSep(QStringLiteral(".")), 1);
    row->addWidget(d, 10);
    return row;
}

QHBoxLayout *narrowField(QWidget *w)
{
    auto *field = new QHBoxLayout;
    field->setSpacing(0);
    field->setContentsMargins(0, 0, 0, 0);
    field->addWidget(w, 10);
    field->addStretch(33);
    return field;
}

QHBoxLayout *labeledRow(QLabel **cap, QLayout *field, int fieldStretch = 15)
{
    auto *row = new QHBoxLayout;
    row->addStretch(1);
    *cap = new QLabel;
    (*cap)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    row->addWidget(*cap, 2);
    row->addLayout(field, fieldStretch);
    row->addStretch(1);
    return row;
}

void splitIp(const QString &ip, QLineEdit *a, QLineEdit *b, QLineEdit *c, QLineEdit *d)
{
    const QStringList parts = ip.split(QChar('.'));
    auto set = [](QLineEdit *e, const QString &v) { e->setText(v.isEmpty() ? QStringLiteral("0") : v); };
    set(a, parts.value(0));
    set(b, parts.value(1));
    set(c, parts.value(2));
    set(d, parts.value(3));
}

QString joinIp(QLineEdit *a, QLineEdit *b, QLineEdit *c, QLineEdit *d)
{
    return QStringLiteral("%1.%2.%3.%4")
        .arg(a->text().toInt())
        .arg(b->text().toInt())
        .arg(c->text().toInt())
        .arg(d->text().toInt());
}

QGroupBox *makeGroup()
{
    auto *box = new QGroupBox;
    box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto *lay = new QVBoxLayout(box);
    lay->setContentsMargins(6, 8, 6, 6);
    lay->setSpacing(4);
    return box;
}

} // namespace

NetPage::NetPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);
    root->setSpacing(6);

    makeIpEdits(&m_lip1, &m_lip2, &m_lip3, &m_lip4);
    makeIpEdits(&m_sip1, &m_sip2, &m_sip3, &m_sip4);
    makeIpEdits(&m_gip1, &m_gip2, &m_gip3, &m_gip4);
    makeIpEdits(&m_rip1, &m_rip2, &m_rip3, &m_rip4);
    m_localPort = makeEditField();
    m_remotePort = makeEditField();
    m_localPort->setValRange(0, 999999, 0);
    m_remotePort->setValRange(0, 999999, 0);

    m_dhcp = new ComboCtrl;
    m_dhcp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_dhcp->addItem(QString(), 0);
    m_dhcp->addItem(QString(), 1);

    m_ethBox = makeGroup();
    auto *ethLay = qobject_cast<QVBoxLayout *>(m_ethBox->layout());
    ethLay->addLayout(labeledRow(&m_dhcpCap, narrowField(m_dhcp)), 1);
    ethLay->addLayout(labeledRow(&m_lipCap, ipOctetRow(m_lip1, m_lip2, m_lip3, m_lip4)), 1);
    ethLay->addLayout(labeledRow(&m_maskCap, ipOctetRow(m_sip1, m_sip2, m_sip3, m_sip4)), 1);
    ethLay->addLayout(labeledRow(&m_gwCap, ipOctetRow(m_gip1, m_gip2, m_gip3, m_gip4)), 1);
    root->addWidget(m_ethBox);

    m_comBox = makeGroup();
    auto *comLay = qobject_cast<QVBoxLayout *>(m_comBox->layout());
    comLay->addLayout(labeledRow(&m_lportCap, narrowField(m_localPort)), 1);
    comLay->addLayout(labeledRow(&m_ripCap, ipOctetRow(m_rip1, m_rip2, m_rip3, m_rip4)), 1);
    comLay->addLayout(labeledRow(&m_rportCap, narrowField(m_remotePort)), 1);
    root->addWidget(m_comBox);
    root->addStretch(1);

    loadFromSettings();

    const QList<EditCtrl *> edits = QList<EditCtrl *>()
        << m_lip1 << m_lip2 << m_lip3 << m_lip4
        << m_sip1 << m_sip2 << m_sip3 << m_sip4
        << m_gip1 << m_gip2 << m_gip3 << m_gip4
        << m_localPort
        << m_rip1 << m_rip2 << m_rip3 << m_rip4
        << m_remotePort;
    for (int i = 0; i < edits.size(); ++i)
    {
        connect(edits.at(i), SIGNAL(valueCommitted(QString)), this, SLOT(onEditFinished()));
        connect(edits.at(i), SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    }
    connect(m_dhcp, SIGNAL(popupChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    connect(m_dhcp, SIGNAL(currentIndexChanged(int)), this, SLOT(onEditFinished()));

    installPageScroll(this, inner);
    retranslateUi();
}

void NetPage::retranslateUi()
{
    m_ethBox->setTitle(tr("Ethernet"));
    m_comBox->setTitle(tr("COM"));
    m_dhcpCap->setText(tr("DHCP:"));
    m_lipCap->setText(tr("Lo IP:"));
    m_maskCap->setText(tr("Mask:"));
    m_gwCap->setText(tr("Gateway:"));
    m_lportCap->setText(tr("Lo Port:"));
    m_ripCap->setText(tr("Re IP:"));
    m_rportCap->setText(tr("Re Port:"));
    m_dhcp->setItemText(0, tr("Off"));
    m_dhcp->setItemText(1, tr("On"));
}

void NetPage::initFocusList()
{
    xList.append(m_dhcp);
    xList.append(m_lip1);
    xList.append(m_lip2);
    xList.append(m_lip3);
    xList.append(m_lip4);
    xList.append(m_sip1);
    xList.append(m_sip2);
    xList.append(m_sip3);
    xList.append(m_sip4);
    xList.append(m_gip1);
    xList.append(m_gip2);
    xList.append(m_gip3);
    xList.append(m_gip4);
    xList.append(m_localPort);
    xList.append(m_rip1);
    xList.append(m_rip2);
    xList.append(m_rip3);
    xList.append(m_rip4);
    xList.append(m_remotePort);

    yList.append(m_dhcp);
    yList.append(m_lip1);
    yList.append(m_sip1);
    yList.append(m_gip1);
    yList.append(m_localPort);
    yList.append(m_rip1);
    yList.append(m_remotePort);
    yList.append(m_lip2);
    yList.append(m_sip2);
    yList.append(m_gip2);
    yList.append(m_rip2);
    yList.append(m_lip3);
    yList.append(m_sip3);
    yList.append(m_gip3);
    yList.append(m_rip3);
    yList.append(m_lip4);
    yList.append(m_sip4);
    yList.append(m_gip4);
    yList.append(m_rip4);
}

void NetPage::onEditFinished()
{
    syncDhcpUi();
    applyToSettings();
    m_c->settings()->save();
    m_c->connectPc();
}

void NetPage::syncDhcpUi()
{
    const bool enableStatic = m_dhcp->currentIndex() == 0;
    const QList<EditCtrl *> staticIps = QList<EditCtrl *>()
        << m_lip1 << m_lip2 << m_lip3 << m_lip4
        << m_sip1 << m_sip2 << m_sip3 << m_sip4
        << m_gip1 << m_gip2 << m_gip3 << m_gip4;
    for (int i = 0; i < staticIps.size(); ++i)
        staticIps.at(i)->setEnabled(enableStatic);

    QWidget *f = focusWidget();
    if (f && !f->isEnabled())
        m_dhcp->setFocus();
}

void NetPage::loadFromSettings()
{
    auto *s = m_c->settings();
    m_dhcp->setCurrentIndex(s->dhcp ? 1 : 0);
    splitIp(s->localIp, m_lip1, m_lip2, m_lip3, m_lip4);
    splitIp(s->subnet, m_sip1, m_sip2, m_sip3, m_sip4);
    splitIp(s->gateway, m_gip1, m_gip2, m_gip3, m_gip4);
    m_localPort->setText(QString::number(s->localPort));
    splitIp(s->remoteIp, m_rip1, m_rip2, m_rip3, m_rip4);
    m_remotePort->setText(QString::number(s->remotePort));
    syncDhcpUi();
}

void NetPage::applyToSettings()
{
    auto *s = m_c->settings();
    s->dhcp = m_dhcp->currentIndex() != 0;
    s->localIp = joinIp(m_lip1, m_lip2, m_lip3, m_lip4);
    s->subnet = joinIp(m_sip1, m_sip2, m_sip3, m_sip4);
    s->gateway = joinIp(m_gip1, m_gip2, m_gip3, m_gip4);
    s->localPort = quint16(qBound(0, m_localPort->text().toInt(), 65535));
    s->remoteIp = joinIp(m_rip1, m_rip2, m_rip3, m_rip4);
    s->remotePort = quint16(qBound(0, m_remotePort->text().toInt(), 65535));
}
