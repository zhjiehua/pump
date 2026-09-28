#ifndef GLPINFOPAGE_H
#define GLPINFOPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QComboBox;
class QLabel;

/** GLP pump info: type, dates, usage counters. */
class GlpInfoPage : public FocusPage
{
    Q_OBJECT
public:
    GlpInfoPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void onPumpChanged(int index);
    void onBack();
    void onTick();

private:
    void loadFromSettings();
    void updateUsedTime();
    void updateFluidLabels();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QComboBox *m_pump = nullptr;
    QLabel *m_manuf = nullptr;
    QLabel *m_inst = nullptr;
    QLabel *m_repair = nullptr;
    QLabel *m_used = nullptr;
    QLabel *m_bugle = nullptr;
    QLabel *m_totalFluid = nullptr;
};

#endif
