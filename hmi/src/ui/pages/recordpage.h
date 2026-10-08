#ifndef RECORDPAGE_H
#define RECORDPAGE_H

#include "ui/focuspage.h"

class MainWindow;
class BtnCtrl;
class QLabel;

/** Records hub under Maintenance: Event / Alarm / Maintenance logs. */
class RecordPage : public FocusPage
{
    Q_OBJECT
public:
    explicit RecordPage(MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void goEvent();
    void goAlarm();
    void goMaint();

private:
    MainWindow *m_main = nullptr;

    BtnCtrl *m_event = nullptr;
    BtnCtrl *m_alarm = nullptr;
    BtnCtrl *m_maint = nullptr;

    QLabel *m_eventLabel = nullptr;
    QLabel *m_alarmLabel = nullptr;
    QLabel *m_maintLabel = nullptr;
};

#endif
