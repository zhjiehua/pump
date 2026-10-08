#ifndef CONFIGMACHINEPAGE_H
#define CONFIGMACHINEPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class ComboCtrl;
class EditCtrl;
class QLabel;
class QPushButton;
class QWidget;

class ConfigMachinePage : public FocusPage
{
    Q_OBJECT
public:
    ConfigMachinePage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onPumpTypeChanged(int index);
    void onSave();

private:
    void loadFromSettings();
    void applyToSettings();
    void updateMcuDrivenFields();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;

    QLabel *m_pumpTypeLabel = nullptr;
    ComboCtrl *m_pumpType = nullptr;
    QLabel *m_wordFactorLabel = nullptr;
    EditCtrl *m_wordFactor = nullptr;
    QLabel *m_pressScaleLabel = nullptr;
    EditCtrl *m_pressScale = nullptr;
    QLabel *m_pmaxLabel = nullptr;
    EditCtrl *m_pmax = nullptr;
    QWidget *m_pmaxField = nullptr;
    QPushButton *m_save = nullptr;
};

#endif
