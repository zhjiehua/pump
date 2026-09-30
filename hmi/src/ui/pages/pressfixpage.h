#ifndef PRESSFIXPAGE_H
#define PRESSFIXPAGE_H

#include "core/appsettings.h"
#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class BtnCtrl;
class EditCtrl;
class QLabel;
class HmiTableWidget;

class PressFixPage : public FocusPage
{
    Q_OBJECT
public:
    PressFixPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);
    ~PressFixPage() override;

protected:
    void initFocusList() override;
    void retranslateUi() override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void loadTable();
    void onStart();
    void onClearPress();
    void onSave();
    void onBack();
    void onPressureChanged();
    void onOutOfTableFocus(int dir);

private:
    QVector<PressPoint> collectTable() const;
    void applyFlowRange();
    void stopRun();
    void refreshStartText();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    HmiTableWidget *m_table = nullptr;
    QLabel *m_flowLabel = nullptr;
    EditCtrl *m_flow = nullptr;
    QLabel *m_flowUnit = nullptr;
    QLabel *m_pressLabel = nullptr;
    QLabel *m_press = nullptr;
    QLabel *m_pressUnit = nullptr;
    BtnCtrl *m_clear = nullptr;
    BtnCtrl *m_start = nullptr;
    BtnCtrl *m_save = nullptr;
    BtnCtrl *m_back = nullptr;
    bool m_running = false;
};

#endif
