#ifndef ADMINMAINTENANCEPAGE_H
#define ADMINMAINTENANCEPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class BtnCtrl;
class EditCtrl;
class QLabel;

class AdminMaintenancePage : public FocusPage
{
    Q_OBJECT
public:
    AdminMaintenancePage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onSave();
    void onFieldChanged();
    void onCancelActive();
    void onClearSys();
    void onClearPump();

private:
    void loadFromSettings();
    void applyToSettings();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;

    QLabel *m_repairCap = nullptr;
    EditCtrl *m_repairYear = nullptr;
    EditCtrl *m_repairMonth = nullptr;
    EditCtrl *m_repairDay = nullptr;
    BtnCtrl *m_save = nullptr;
    BtnCtrl *m_cancelActive = nullptr;
    BtnCtrl *m_clearSys = nullptr;
    BtnCtrl *m_clearPump = nullptr;
};

#endif
