#ifndef FLOWFIXPAGE_H
#define FLOWFIXPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class HmiTableWidget;
class QLineEdit;
class QPushButton;

class FlowFixPage : public FocusPage
{
    Q_OBJECT
public:
    FlowFixPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void loadTable();
    void onAdd();
    void onGet();
    void onSet();
    void onStart();
    void onStop();
    void onBack();
    void onOutOfTableFocus(int dir);

private:
    void fromUi();
    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    HmiTableWidget *m_table = nullptr;
    QLineEdit *m_flow = nullptr;
    QPushButton *m_start = nullptr;
    QPushButton *m_stop = nullptr;
    QPushButton *m_add = nullptr;
    QPushButton *m_get = nullptr;
    QPushButton *m_set = nullptr;
    QPushButton *m_back = nullptr;
};

#endif
