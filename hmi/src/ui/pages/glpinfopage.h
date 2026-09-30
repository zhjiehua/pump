#ifndef GLPINFOPAGE_H
#define GLPINFOPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class ComboCtrl;
class QLabel;

/** GLP pump info: type, dates, usage counters (weiduodianzi GlpInfoPage). */
class GlpInfoPage : public FocusPage
{
    Q_OBJECT
public:
    GlpInfoPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onPumpChanged(int index);
    void onTick();
    void onPumpAuthNeeded();
    void resumePumpEdit();

private:
    void loadFromSettings();
    void updateUsedTime();
    void updateFluidLabels();
    void requestPumpAuth();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    ComboCtrl *m_pump = nullptr;
    QLabel *m_manufCap = nullptr;
    QLabel *m_instCap = nullptr;
    QLabel *m_repairCap = nullptr;
    QLabel *m_usedCap = nullptr;
    QLabel *m_bugleCap = nullptr;
    QLabel *m_fluidCap = nullptr;
    QLabel *m_pumpCap = nullptr;
    QLabel *m_manuf = nullptr;
    QLabel *m_inst = nullptr;
    QLabel *m_repair = nullptr;
    QLabel *m_used = nullptr;
    QLabel *m_bugle = nullptr;
    QLabel *m_totalFluid = nullptr;
    bool m_pwdNeed = true;
    bool m_resumePump = false;
};

#endif
