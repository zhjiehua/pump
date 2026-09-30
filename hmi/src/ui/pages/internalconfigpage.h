#ifndef INTERNALCONFIGPAGE_H
#define INTERNALCONFIGPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class ComboCtrl;
class EditCtrl;
class QLabel;
class QPushButton;

/** Internal service page: MCU/PC protocol, ports, JSON config path. */
class InternalConfigPage : public FocusPage
{
    Q_OBJECT
public:
    InternalConfigPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onMcuProtocolChanged(int index);
    void onPumpTypeChanged(int index);
    void onSave();
    void onReconnect();
    void onDebug();
    void onScaleChanged(int index);
    void onBack();

private:
    void loadFromSettings();
    void applyToSettings();
    void refreshPortLists();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;

    ComboCtrl *m_mcuProto = nullptr;
    QLabel *m_mcuTypeLabel = nullptr;
    ComboCtrl *m_mcuPort = nullptr;
    EditCtrl *m_mcuBaud = nullptr;
    EditCtrl *m_mcuAddr = nullptr;
    ComboCtrl *m_pumpType = nullptr;
    EditCtrl *m_wordFactor = nullptr;
    EditCtrl *m_pressScale = nullptr;

    ComboCtrl *m_pcProto = nullptr;
    ComboCtrl *m_pcPortType = nullptr;
    ComboCtrl *m_pcSerial = nullptr;
    EditCtrl *m_pcBaud = nullptr;
    EditCtrl *m_localUdp = nullptr;
    EditCtrl *m_remoteIp = nullptr;
    EditCtrl *m_remotePort = nullptr;
    EditCtrl *m_machineCode = nullptr;
    ComboCtrl *m_scale = nullptr;
    QLabel *m_pathLbl = nullptr;

    QPushButton *m_save = nullptr;
    QPushButton *m_reconnect = nullptr;
    QPushButton *m_debug = nullptr;
    QPushButton *m_back = nullptr;
};

#endif
