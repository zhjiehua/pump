#ifndef MSGPAGE_H
#define MSGPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QLabel;

/** About / program info (weiduodianzi MsgPage). */
class QPushButton;

class MsgPage : public FocusPage
{
    Q_OBJECT
public:
    MsgPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void updateProgram();

private:
    void refreshLabels();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QLabel *m_version = nullptr;
    QLabel *m_license = nullptr;
    QLabel *m_serial = nullptr;
    QPushButton *m_update = nullptr;
};

#endif
