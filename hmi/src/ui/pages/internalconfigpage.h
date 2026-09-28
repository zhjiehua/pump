#ifndef INTERNALCONFIGPAGE_H
#define INTERNALCONFIGPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QComboBox;
class QLineEdit;
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

    QComboBox *m_mcuProto = nullptr;
    QComboBox *m_mcuPort = nullptr;
    QLineEdit *m_mcuBaud = nullptr;
    QLineEdit *m_mcuAddr = nullptr;
    QComboBox *m_pumpType = nullptr;
    QLineEdit *m_wordFactor = nullptr;
    QLineEdit *m_pressScale = nullptr;

    QComboBox *m_pcProto = nullptr;
    QComboBox *m_pcPortType = nullptr;
    QComboBox *m_pcSerial = nullptr;
    QLineEdit *m_pcBaud = nullptr;
    QLineEdit *m_localUdp = nullptr;
    QLineEdit *m_remoteIp = nullptr;
    QLineEdit *m_remotePort = nullptr;
    QLineEdit *m_machineCode = nullptr;
    QComboBox *m_scale = nullptr;
    QLabel *m_pathLbl = nullptr;

    QPushButton *m_save = nullptr;
    QPushButton *m_reconnect = nullptr;
    QPushButton *m_debug = nullptr;
    QPushButton *m_back = nullptr;
};

#endif
