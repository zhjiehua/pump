#ifndef TIMEPAGE_H
#define TIMEPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QLineEdit;

/** System date/time editor with live refresh. */
class TimePage : public FocusPage
{
    Q_OBJECT
public:
    TimePage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void refreshFromClock();
    void applyDateTime();

private:
    bool anyFieldFocused() const;

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QLineEdit *m_year = nullptr;
    QLineEdit *m_month = nullptr;
    QLineEdit *m_day = nullptr;
    QLineEdit *m_hour = nullptr;
    QLineEdit *m_min = nullptr;
    QLineEdit *m_sec = nullptr;
};

#endif
