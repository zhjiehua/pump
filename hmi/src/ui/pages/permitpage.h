#ifndef PERMITPAGE_H
#define PERMITPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class EditCtrl;
class QLabel;

class QPushButton;

class PermitPage : public FocusPage
{
    Q_OBJECT
public:
    PermitPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void refreshDays();
    void onRegister();
    void onSerial();

private:
    MachineController *m_c;
    MainWindow *m_main;
    QLabel *m_days;
    EditCtrl *m_license;
    QPushButton *m_serialBtn = nullptr;
    QPushButton *m_register = nullptr;
};

#endif
