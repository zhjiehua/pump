#include "ui/pages/recordpage.h"
#include "core/picturemanager.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "ui/widgets/pagescroll.h"

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace {

BtnCtrl *iconBtn(PictureManager::Picture normal, PictureManager::Picture focus)
{
    auto *b = new BtnCtrl;
    b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    b->setMinimumSize(40, 40);
    b->setStyleSheet(PictureManager::instance().iconButtonStyle(normal, focus));
    return b;
}

QWidget *iconCell(BtnCtrl *btn, QLabel **label)
{
    auto *w = new QWidget;
    w->setMinimumSize(56, 64);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(2);
    v->addWidget(btn, 5);
    *label = new QLabel;
    (*label)->setAlignment(Qt::AlignCenter);
    (*label)->setMinimumHeight(14);
    v->addWidget(*label, 1);
    return w;
}

} // namespace

RecordPage::RecordPage(MainWindow *main, QWidget *parent)
    : FocusPage(parent)
    , m_main(main)
{
    auto *inner = new QWidget;
    auto *g = new QGridLayout(inner);
    g->setContentsMargins(8, 6, 8, 6);
    g->setHorizontalSpacing(10);
    g->setVerticalSpacing(8);

    m_event = iconBtn(PictureManager::Clock, PictureManager::ClockFocus);
    m_alarm = iconBtn(PictureManager::Message, PictureManager::MessageFocus);
    m_maint = iconBtn(PictureManager::Permission, PictureManager::PermissionFocus);

    g->addWidget(iconCell(m_event, &m_eventLabel), 0, 0);
    g->addWidget(iconCell(m_alarm, &m_alarmLabel), 0, 1);
    g->addWidget(iconCell(m_maint, &m_maintLabel), 0, 2);

    installPageScroll(this, inner);
    retranslateUi();

    connect(m_event, SIGNAL(clicked()), this, SLOT(goEvent()));
    connect(m_alarm, SIGNAL(clicked()), this, SLOT(goAlarm()));
    connect(m_maint, SIGNAL(clicked()), this, SLOT(goMaint()));
}

void RecordPage::retranslateUi()
{
    m_eventLabel->setText(tr("Event"));
    m_alarmLabel->setText(tr("Alarm"));
    m_maintLabel->setText(tr("Maint"));
}

void RecordPage::initFocusList()
{
    xList.append(m_event);
    xList.append(m_alarm);
    xList.append(m_maint);
    yList = xList;
}

void RecordPage::goEvent() { m_main->go(MainWindow::EventRecords); }
void RecordPage::goAlarm() { m_main->go(MainWindow::AlarmRecords); }
void RecordPage::goMaint() { m_main->go(MainWindow::MaintRecords); }
