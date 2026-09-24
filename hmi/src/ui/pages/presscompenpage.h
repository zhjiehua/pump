#ifndef PRESSCOMPENPAGE_H
#define PRESSCOMPENPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QLineEdit;
class QCheckBox;
class QPushButton;

class PressCompenPage : public FocusPage
{
    Q_OBJECT
public:
    PressCompenPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void fillTable();
    void onSet();
    void onBack();

private:
    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QCheckBox *m_en = nullptr;
    QLineEdit *m_rate = nullptr;
    QLineEdit *m_real = nullptr;
    QLineEdit *m_press = nullptr;
    QPushButton *m_set = nullptr;
    QPushButton *m_back = nullptr;
};

#endif
