#ifndef RUNPARAMPAGE_H
#define RUNPARAMPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QComboBox;
class QLineEdit;
class QPushButton;

/** Run parameters: pressure limits, coefficient, gradient table. */
class RunParamPage : public FocusPage
{
    Q_OBJECT
public:
    RunParamPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void onSave();
    void onGrad();
    void onBack();

private:
    void loadFromSettings();
    double effectivePmaxCap() const;

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QLineEdit *m_min = nullptr;
    QLineEdit *m_max = nullptr;
    QLineEdit *m_coeff = nullptr;
    QComboBox *m_gradient = nullptr;
    QPushButton *m_save = nullptr;
    QPushButton *m_grad = nullptr;
    QPushButton *m_back = nullptr;
};

#endif
