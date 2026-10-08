#ifndef ADMINDEVICEPAGE_H
#define ADMINDEVICEPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class EditCtrl;
class BtnCtrl;
class QLabel;

class AdminDevicePage : public FocusPage
{
    Q_OBJECT
public:
    AdminDevicePage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onSave();
    void onFieldChanged();

private:
    void loadFromSettings();
    void applyToSettings();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;

    QLabel *m_manufCap = nullptr;
    QLabel *m_instCap = nullptr;
    QLabel *m_licenCap = nullptr;
    QLabel *m_serialCap = nullptr;

    EditCtrl *m_manufYear = nullptr;
    EditCtrl *m_manufMonth = nullptr;
    EditCtrl *m_manufDay = nullptr;
    EditCtrl *m_instYear = nullptr;
    EditCtrl *m_instMonth = nullptr;
    EditCtrl *m_instDay = nullptr;
    EditCtrl *m_license = nullptr;
    EditCtrl *m_serial = nullptr;
    BtnCtrl *m_save = nullptr;
};

#endif
