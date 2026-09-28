#ifndef GRADIENTTABLEPAGE_H
#define GRADIENTTABLEPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QPushButton;
class HmiTableWidget;

/** Single gradient table editor (time / flow points). */
class GradientTablePage : public FocusPage
{
    Q_OBJECT
public:
    GradientTablePage(MachineController *c, MainWindow *main, int which = 0,
                      QWidget *parent = nullptr);

    void setWhich(int which);
    void reload();

protected:
    void initFocusList() override;

private slots:
    void loadTable();
    void saveTable();
    void addRow();
    void onBack();
    void onOutOfTableFocus(int dir);

private:
    MachineController *m_c;
    MainWindow *m_main;
    HmiTableWidget *m_table = nullptr;
    QPushButton *m_add = nullptr;
    QPushButton *m_save = nullptr;
    QPushButton *m_back = nullptr;
    int m_which;
};

#endif
