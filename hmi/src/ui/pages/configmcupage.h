#ifndef CONFIGMCUPAGE_H
#define CONFIGMCUPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class ComboCtrl;
class EditCtrl;
class QLabel;
class QPushButton;

class ConfigMcuPage : public FocusPage
{
    Q_OBJECT
public:
    ConfigMcuPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onMcuProtocolChanged(int index);
    void onSave();

private:
    void loadFromSettings();
    void applyToSettings();
    void updateMcuFieldVisibility();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;

    QLabel *m_mcuTypeLabel = nullptr;
    ComboCtrl *m_mcuProto = nullptr;
    QLabel *m_mcuPortLabel = nullptr;
    ComboCtrl *m_mcuPort = nullptr;
    QLabel *m_mcuBaudLabel = nullptr;
    EditCtrl *m_mcuBaud = nullptr;
    QLabel *m_mcuAddrLabel = nullptr;
    EditCtrl *m_mcuAddr = nullptr;
    QPushButton *m_save = nullptr;
};

#endif
