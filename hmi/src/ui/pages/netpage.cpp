#include "ui/pages/netpage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/editfield.h"
#include "ui/widgets/pagescroll.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

namespace {

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

QHBoxLayout *labeledRow(const QString &caption, QLayout *field, int fieldStretch = 15)
{
    auto *row = new QHBoxLayout;
    row->addStretch(1);
    auto *lab = new QLabel(caption);
    lab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    row->addWidget(lab, 2);
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

} // namespace

NetPage::NetPage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);
    root->setSpacing(0);

    m_lip1 = makeEditField();
    m_lip2 = makeEditField();
    m_lip3 = makeEditField();
    m_lip4 = makeEditField();
    m_localPort = makeEditField();
    m_rip1 = makeEditField();
    m_rip2 = makeEditField();
    m_rip3 = makeEditField();
    m_rip4 = makeEditField();
    m_remotePort = makeEditField();

    root->addStretch(1);
    root->addLayout(labeledRow(tr("Lo IP:"), ipOctetRow(m_lip1, m_lip2, m_lip3, m_lip4)), 1);
    root->addStretch(1);

    {
        auto *portField = new QHBoxLayout;
        portField->setSpacing(0);
        portField->addWidget(m_localPort, 10);
        portField->addStretch(33);
        root->addLayout(labeledRow(tr("Lo Port:"), portField), 1);
    }

    auto *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    root->addWidget(line, 2);

    root->addLayout(labeledRow(tr("Re IP:"), ipOctetRow(m_rip1, m_rip2, m_rip3, m_rip4)), 1);
    root->addStretch(1);

    {
        auto *portField = new QHBoxLayout;
        portField->setSpacing(0);
        portField->addWidget(m_remotePort, 10);
        portField->addStretch(33);
        root->addLayout(labeledRow(tr("Re Port:"), portField), 1);
    }
    root->addStretch(1);

    loadFromSettings();

    const QList<QLineEdit *> edits = QList<QLineEdit *>()
        << m_lip1 << m_lip2 << m_lip3 << m_lip4 << m_localPort
        << m_rip1 << m_rip2 << m_rip3 << m_rip4 << m_remotePort;
    for (int i = 0; i < edits.size(); ++i)
        connect(edits.at(i), SIGNAL(editingFinished()), this, SLOT(onEditFinished()));

    installPageScroll(this, inner);
}

void NetPage::initFocusList()
{
    xList.append(m_lip1);
    xList.append(m_lip2);
    xList.append(m_lip3);
    xList.append(m_lip4);
    xList.append(m_localPort);
    xList.append(m_rip1);
    xList.append(m_rip2);
    xList.append(m_rip3);
    xList.append(m_rip4);
    xList.append(m_remotePort);
    yList.append(m_lip1);
    yList.append(m_localPort);
    yList.append(m_rip1);
    yList.append(m_remotePort);
    yList.append(m_lip2);
    yList.append(m_rip2);
    yList.append(m_lip3);
    yList.append(m_rip3);
    yList.append(m_lip4);
    yList.append(m_rip4);
}

void NetPage::onEditFinished()
{
    applyToSettings();
    m_c->settings()->save();
    m_c->connectPc();
}

void NetPage::loadFromSettings()
{
    auto *s = m_c->settings();
    splitIp(s->localIp, m_lip1, m_lip2, m_lip3, m_lip4);
    m_localPort->setText(QString::number(s->localUdpPort));
    splitIp(s->remoteIp, m_rip1, m_rip2, m_rip3, m_rip4);
    m_remotePort->setText(QString::number(s->remotePort));
}

void NetPage::applyToSettings()
{
    auto *s = m_c->settings();
    s->localIp = joinIp(m_lip1, m_lip2, m_lip3, m_lip4);
    s->localUdpPort = quint16(qBound(0, m_localPort->text().toInt(), 65535));
    s->remoteIp = joinIp(m_rip1, m_rip2, m_rip3, m_rip4);
    s->remotePort = quint16(qBound(0, m_remotePort->text().toInt(), 65535));
}
