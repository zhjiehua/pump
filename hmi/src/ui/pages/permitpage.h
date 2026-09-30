#ifndef PERMITPAGE_H
#define PERMITPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class EditCtrl;
class BtnCtrl;
class ImgButton;
class QLabel;

class PermitPage : public FocusPage
{
    Q_OBJECT
public:
    PermitPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void refreshDays();
    void onRegister();
    void onSerial();

private:
    MachineController *m_c;
    MainWindow *m_main;
    QLabel *m_tips = nullptr;
    QLabel *m_probation = nullptr;
    QLabel *m_days = nullptr;
    QLabel *m_dayUnit = nullptr;
    EditCtrl *m_license;
    ImgButton *m_icon = nullptr;
    BtnCtrl *m_serialBtn = nullptr;
    BtnCtrl *m_register = nullptr;
};

#endif
