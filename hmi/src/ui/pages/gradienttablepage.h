#ifndef GRADIENTTABLEPAGE_H
#define GRADIENTTABLEPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class BtnCtrl;
class HmiTableWidget;

/** Single gradient table editor (weiduodianzi GradientTable). */
class GradientTablePage : public FocusPage
{
    Q_OBJECT
public:
    GradientTablePage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

    void reload();

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void loadTable();
    void saveTable();
    void onBack();
    void onOutOfTableFocus(int dir);

private:
    MachineController *m_c;
    MainWindow *m_main;
    HmiTableWidget *m_table = nullptr;
    BtnCtrl *m_save = nullptr;
    BtnCtrl *m_back = nullptr;
};

#endif
