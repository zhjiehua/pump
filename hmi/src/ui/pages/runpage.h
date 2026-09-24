#ifndef RUNPAGE_H
#define RUNPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class EditCtrl;
class QLabel;
class QComboBox;

class RunPage : public FocusPage
{
    Q_OBJECT
public:
    RunPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    bool handleFocusNavKey(int key) override;

private slots:
    void onStatActivated(int index);
    void onFlowCommitted(const QString &value);
    void refresh();

private:
    void updateTimeLabel(quint32 sec);

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    EditCtrl *m_flow = nullptr;
    QLabel *m_percent = nullptr;
    QLabel *m_press = nullptr;
    QComboBox *m_stat = nullptr;
    QLabel *m_time = nullptr;
};

#endif
