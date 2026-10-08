#ifndef CONFIGSYSTEMPAGE_H
#define CONFIGSYSTEMPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class ComboCtrl;
class QLabel;
class QPushButton;

class ConfigSystemPage : public FocusPage
{
    Q_OBJECT
public:
    ConfigSystemPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onSave();
    void onReconnect();
    void onDebug();
    void onScaleChanged(int index);

private:
    void loadFromSettings();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;

    QLabel *m_scaleLabel = nullptr;
    QLabel *m_jsonLabel = nullptr;
    ComboCtrl *m_scale = nullptr;
    QLabel *m_pathLbl = nullptr;
    QPushButton *m_save = nullptr;
    QPushButton *m_reconnect = nullptr;
    QPushButton *m_debug = nullptr;
};

#endif
