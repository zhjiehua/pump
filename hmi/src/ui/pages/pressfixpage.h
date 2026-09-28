#ifndef PRESSFIXPAGE_H
#define PRESSFIXPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QLabel;
class QPushButton;
class HmiTableWidget;

class PressFixPage : public FocusPage
{
    Q_OBJECT
public:
    PressFixPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void loadTable();
    void onAdd();
    void onZero();
    void onGet();
    void onSet();
    void onBack();
    void onPressureChanged();
    void onOutOfTableFocus(int dir);

private:
    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    HmiTableWidget *m_table = nullptr;
    QLabel *m_press = nullptr;
    QPushButton *m_add = nullptr;
    QPushButton *m_zero = nullptr;
    QPushButton *m_get = nullptr;
    QPushButton *m_set = nullptr;
    QPushButton *m_back = nullptr;
};

#endif
