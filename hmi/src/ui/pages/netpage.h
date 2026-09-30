#ifndef NETPAGE_H
#define NETPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class ComboCtrl;
class EditCtrl;
class QGroupBox;
class QLabel;

/** Network configuration: Ethernet (DHCP/IP/mask/gateway) and COM link. */
class NetPage : public FocusPage
{
    Q_OBJECT
public:
    NetPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onEditFinished();

private:
    void loadFromSettings();
    void applyToSettings();
    void syncDhcpUi();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;

    QGroupBox *m_ethBox = nullptr;
    QGroupBox *m_comBox = nullptr;
    QLabel *m_dhcpCap = nullptr;
    QLabel *m_lipCap = nullptr;
    QLabel *m_maskCap = nullptr;
    QLabel *m_gwCap = nullptr;
    QLabel *m_lportCap = nullptr;
    QLabel *m_ripCap = nullptr;
    QLabel *m_rportCap = nullptr;
    ComboCtrl *m_dhcp = nullptr;
    EditCtrl *m_lip1 = nullptr;
    EditCtrl *m_lip2 = nullptr;
    EditCtrl *m_lip3 = nullptr;
    EditCtrl *m_lip4 = nullptr;
    EditCtrl *m_sip1 = nullptr;
    EditCtrl *m_sip2 = nullptr;
    EditCtrl *m_sip3 = nullptr;
    EditCtrl *m_sip4 = nullptr;
    EditCtrl *m_gip1 = nullptr;
    EditCtrl *m_gip2 = nullptr;
    EditCtrl *m_gip3 = nullptr;
    EditCtrl *m_gip4 = nullptr;
    EditCtrl *m_localPort = nullptr;
    EditCtrl *m_rip1 = nullptr;
    EditCtrl *m_rip2 = nullptr;
    EditCtrl *m_rip3 = nullptr;
    EditCtrl *m_rip4 = nullptr;
    EditCtrl *m_remotePort = nullptr;
};

#endif
