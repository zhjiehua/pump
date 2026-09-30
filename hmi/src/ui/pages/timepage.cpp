#include "ui/pages/timepage.h"
#include "core/machinecontroller.h"
#include "ui/mainwindow.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/editfield.h"
#include "ui/widgets/pagescroll.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QTimer>
#include <QVBoxLayout>

TimePage::TimePage(MachineController *c, MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_c(c)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *root = new QVBoxLayout(inner);
    root->setContentsMargins(4, 4, 4, 4);
    root->setSpacing(0);

    root->addStretch(1);

    auto *dateRow = new QHBoxLayout;
    dateRow->setSpacing(2);
    m_dateCap = new QLabel;
    m_dateCap->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_year = makeEditField();
    m_year->setValRange(2015, 2050, 0);
    m_month = makeEditField();
    m_month->setValRange(1, 12, 0);
    m_day = makeEditField();
    m_day->setValRange(1, 31, 0);
    dateRow->addWidget(m_dateCap, 20);
    dateRow->addWidget(m_year, 30);
    dateRow->addWidget(makeSep(QStringLiteral("-")), 1);
    dateRow->addWidget(m_month, 30);
    dateRow->addWidget(makeSep(QStringLiteral("-")), 1);
    dateRow->addWidget(m_day, 30);
    root->addLayout(dateRow, 1);

    root->addStretch(1);

    auto *timeRow = new QHBoxLayout;
    timeRow->setSpacing(2);
    m_timeCap = new QLabel;
    m_timeCap->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_hour = makeEditField();
    m_hour->setValRange(0, 23, 0);
    m_min = makeEditField();
    m_min->setValRange(0, 59, 0);
    m_sec = makeEditField();
    m_sec->setValRange(0, 59, 0);
    timeRow->addWidget(m_timeCap, 20);
    timeRow->addWidget(m_hour, 30);
    timeRow->addWidget(makeSep(QStringLiteral(":")), 1);
    timeRow->addWidget(m_min, 30);
    timeRow->addWidget(makeSep(QStringLiteral(":")), 1);
    timeRow->addWidget(m_sec, 30);
    root->addLayout(timeRow, 1);

    root->addStretch(1);
    root->addStretch(1);
    root->addStretch(1);

    refreshFromClock();

    auto *tick = new QTimer(this);
    connect(tick, SIGNAL(timeout()), this, SLOT(refreshFromClock()));
    tick->start(1000);

    const QList<EditCtrl *> edits = QList<EditCtrl *>()
        << m_year << m_month << m_day << m_hour << m_min << m_sec;
    for (int i = 0; i < edits.size(); ++i)
    {
        connect(edits.at(i), SIGNAL(valueCommitted(QString)), this, SLOT(applyDateTime()));
        connect(edits.at(i), SIGNAL(editingChanged(bool)), m_main, SLOT(onEditCtrlEditingChanged(bool)));
    }

    installPageScroll(this, inner);
    retranslateUi();
}

void TimePage::retranslateUi()
{
    m_dateCap->setText(tr("Current Date:"));
    m_timeCap->setText(tr("Current Time:"));
}

void TimePage::initFocusList()
{
    xList.append(m_year);
    xList.append(m_month);
    xList.append(m_day);
    xList.append(m_hour);
    xList.append(m_min);
    xList.append(m_sec);
    yList.append(m_year);
    yList.append(m_hour);
    yList.append(m_month);
    yList.append(m_min);
    yList.append(m_day);
    yList.append(m_sec);
}

bool TimePage::anyFieldFocused() const
{
    return m_year->isEditing() || m_month->isEditing() || m_day->isEditing()
           || m_hour->isEditing() || m_min->isEditing() || m_sec->isEditing();
}

void TimePage::refreshFromClock()
{
    if (anyFieldFocused())
        return;

    const QDateTime now = QDateTime::currentDateTime();
    const QDate d = now.date();
    const QTime t = now.time();
    m_year->setText(QString::number(d.year()));
    m_month->setText(QString::number(d.month()));
    m_day->setText(QString::number(d.day()));
    m_hour->setText(QString::number(t.hour()));
    m_min->setText(QString::number(t.minute()));
    m_sec->setText(QString::number(t.second()));
}

void TimePage::applyDateTime()
{
    const int y = m_year->text().toInt();
    const int mo = m_month->text().toInt();
    const int d = m_day->text().toInt();
    const int h = m_hour->text().toInt();
    const int mi = m_min->text().toInt();
    const int s = m_sec->text().toInt();
    if (y < 2015 || y > 2050 || mo < 1 || mo > 12 || d < 1 || d > 31
        || h < 0 || h > 23 || mi < 0 || mi > 59 || s < 0 || s > 59)
        return;

    const QString stamp = QStringLiteral("%1-%2-%3 %4:%5:%6")
                              .arg(y, 4, 10, QChar('0'))
                              .arg(mo, 2, 10, QChar('0'))
                              .arg(d, 2, 10, QChar('0'))
                              .arg(h, 2, 10, QChar('0'))
                              .arg(mi, 2, 10, QChar('0'))
                              .arg(s, 2, 10, QChar('0'));

#if defined(Q_OS_LINUX)
    QProcess::execute(QStringLiteral("date"), {QStringLiteral("-s"), stamp});
    QProcess::execute(QStringLiteral("hwclock"), {QStringLiteral("-w")});
#else
    Q_UNUSED(stamp);
#endif
}
