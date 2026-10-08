#ifndef CONFIGCDSPAGE_H
#define CONFIGCDSPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class ComboCtrl;
class EditCtrl;
class QLabel;
class QPushButton;

class ConfigCdsPage : public FocusPage
{
    Q_OBJECT
public:
    ConfigCdsPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onPcProtocolChanged(int index);
    void onPcPortTypeChanged(int index);
    void onSave();

private:
    void loadFromSettings();
    void applyToSettings();
    void updateCdsFieldVisibility();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;

    QLabel *m_cdsProtoLabel = nullptr;
    ComboCtrl *m_pcProto = nullptr;
    QLabel *m_cdsLinkLabel = nullptr;
    ComboCtrl *m_pcPortType = nullptr;
    QLabel *m_pcSerialLabel = nullptr;
    ComboCtrl *m_pcSerial = nullptr;
    QLabel *m_pcBaudLabel = nullptr;
    EditCtrl *m_pcBaud = nullptr;
    QLabel *m_machineCodeLabel = nullptr;
    EditCtrl *m_machineCode = nullptr;
    QPushButton *m_save = nullptr;
};

#endif
