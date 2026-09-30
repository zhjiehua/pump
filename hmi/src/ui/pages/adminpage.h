#ifndef ADMINPAGE_H
#define ADMINPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class EditCtrl;
class ComboCtrl;
class BtnCtrl;
class QLabel;

/** Full admin settings (weiduodianzi AdminPage). */
class AdminPage : public FocusPage
{
    Q_OBJECT
public:
    AdminPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onCancelActive();
    void onClearSys();
    void onClearPump();
    void onRestore();
    void onSaveData();
    void onUpdateData();
    void onFieldChanged();

private:
    void loadFromSettings();
    void applyToSettings();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;

    BtnCtrl *m_cancelActive = nullptr;
    BtnCtrl *m_clearSys = nullptr;
    BtnCtrl *m_clearPump = nullptr;
    BtnCtrl *m_restore = nullptr;
    BtnCtrl *m_saveData = nullptr;
    BtnCtrl *m_updateData = nullptr;

    QLabel *m_manufCap = nullptr;
    QLabel *m_instCap = nullptr;
    QLabel *m_repairCap = nullptr;
    QLabel *m_licenCap = nullptr;
    QLabel *m_protoCap = nullptr;
    QLabel *m_serialCap = nullptr;
    QLabel *m_conneCap = nullptr;
    QLabel *m_pmaxCap = nullptr;

    EditCtrl *m_manufYear = nullptr;
    EditCtrl *m_manufMonth = nullptr;
    EditCtrl *m_manufDay = nullptr;
    EditCtrl *m_instYear = nullptr;
    EditCtrl *m_instMonth = nullptr;
    EditCtrl *m_instDay = nullptr;
    EditCtrl *m_repairYear = nullptr;
    EditCtrl *m_repairMonth = nullptr;
    EditCtrl *m_repairDay = nullptr;
    EditCtrl *m_license = nullptr;
    EditCtrl *m_serial = nullptr;
    ComboCtrl *m_pcProto = nullptr;
    ComboCtrl *m_connect = nullptr;
    EditCtrl *m_pmax = nullptr;
};

#endif
