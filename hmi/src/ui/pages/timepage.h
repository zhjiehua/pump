#ifndef TIMEPAGE_H
#define TIMEPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class EditCtrl;
class QLabel;

/** System date/time editor with live refresh. */
class TimePage : public FocusPage
{
    Q_OBJECT
public:
    TimePage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void refreshFromClock();
    void applyDateTime();

private:
    bool anyFieldFocused() const;

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QLabel *m_dateCap = nullptr;
    QLabel *m_timeCap = nullptr;
    EditCtrl *m_year = nullptr;
    EditCtrl *m_month = nullptr;
    EditCtrl *m_day = nullptr;
    EditCtrl *m_hour = nullptr;
    EditCtrl *m_min = nullptr;
    EditCtrl *m_sec = nullptr;
};

#endif
