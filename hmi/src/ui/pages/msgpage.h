#ifndef MSGPAGE_H
#define MSGPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QLabel;
class BtnCtrl;

class MsgPage : public FocusPage
{
    Q_OBJECT
public:
    MsgPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void updateProgram();

private:
    void refreshLabels();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QLabel *m_versionCap = nullptr;
    QLabel *m_licenseCap = nullptr;
    QLabel *m_serialCap = nullptr;
    QLabel *m_version = nullptr;
    QLabel *m_license = nullptr;
    QLabel *m_serial = nullptr;
    BtnCtrl *m_update = nullptr;
};

#endif
