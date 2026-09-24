#ifndef PULSEFIXPAGE_H
#define PULSEFIXPAGE_H

#include "core/appsettings.h"
#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QPushButton;
class HmiTableWidget;
class QLineEdit;

class PulseFixPage : public FocusPage
{
    Q_OBJECT
public:
    PulseFixPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void loadTable();
    void onGenerate();
    void onGet();
    void onSet();
    void onSave();
    void onClear();
    void onBack();
    void onOutOfTableFocus(int dir);

private:
    QVector<PulsePoint> collectTable() const;
    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    HmiTableWidget *m_table = nullptr;
    QLineEdit *m_steps = nullptr;
    QLineEdit *m_div = nullptr;
    QPushButton *m_gen = nullptr;
    QPushButton *m_get = nullptr;
    QPushButton *m_set = nullptr;
    QPushButton *m_save = nullptr;
    QPushButton *m_clr = nullptr;
    QPushButton *m_back = nullptr;
};

#endif
