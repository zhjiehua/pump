#ifndef NETPAGE_H
#define NETPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QLineEdit;

/** Network configuration: Lo/Re IP octets + ports (weiduodianzi layout). */
class NetPage : public FocusPage
{
    Q_OBJECT
public:
    NetPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void onEditFinished();

private:
    void loadFromSettings();
    void applyToSettings();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QLineEdit *m_lip1 = nullptr;
    QLineEdit *m_lip2 = nullptr;
    QLineEdit *m_lip3 = nullptr;
    QLineEdit *m_lip4 = nullptr;
    QLineEdit *m_localPort = nullptr;
    QLineEdit *m_rip1 = nullptr;
    QLineEdit *m_rip2 = nullptr;
    QLineEdit *m_rip3 = nullptr;
    QLineEdit *m_rip4 = nullptr;
    QLineEdit *m_remotePort = nullptr;
};

#endif
