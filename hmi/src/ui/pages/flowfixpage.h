#ifndef FLOWFIXPAGE_H
#define FLOWFIXPAGE_H

#include "core/appsettings.h"
#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class HmiTableWidget;
class BtnCtrl;
class ComboCtrl;
class EditCtrl;
class QLabel;
class QTimer;

class FlowFixPage : public FocusPage
{
    Q_OBJECT
public:
    FlowFixPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);
    ~FlowFixPage() override;

protected:
    void initFocusList() override;
    void retranslateUi() override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void loadTable();
    void onStart();
    void onSave();
    void onBack();
    void onTimeout();
    void onPressureChanged();
    void onOutOfTableFocus(int dir);

private:
    QVector<RatePoint> collectTable() const;
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
    QLabel *m_timeLabel = nullptr;
    ComboCtrl *m_time = nullptr;
    QLabel *m_timeUnit = nullptr;
    BtnCtrl *m_start = nullptr;
    BtnCtrl *m_save = nullptr;
    BtnCtrl *m_back = nullptr;
    QTimer *m_timer = nullptr;
    bool m_running = false;
};

#endif
