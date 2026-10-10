#ifndef RUNPARAMPAGE_H
#define RUNPARAMPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class ComboCtrl;
class EditCtrl;

/** Run parameters: pressure limits, gradient high/low, compensation coefficient. */
class RunParamPage : public FocusPage
{
    Q_OBJECT
public:
    RunParamPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void onMaxCommitted(const QString &value);
    void onMinCommitted(const QString &value);
    void onCoeffCommitted(const QString &value);
    void onGradientActivated(int index);
    void refresh();

private:
    void loadFromSettings();
    bool editorsBusy() const;
    void applyPressLimits(double pmin, double pmax);
    double effectivePmaxCap() const;

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    EditCtrl *m_max = nullptr;
    EditCtrl *m_min = nullptr;
    EditCtrl *m_coeff = nullptr;
    ComboCtrl *m_gradient = nullptr;
};

#endif
