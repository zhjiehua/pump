#ifndef ADMINPAGE_H
#define ADMINPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QComboBox;
class QLineEdit;
class QPushButton;

/** Full admin settings (password-protected entry). */
class AdminPage : public FocusPage
{
    Q_OBJECT
public:
    AdminPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void onCancelActive();
    void onClearSys();
    void onClearPump();
    void onRestore();
    void onImportJson();
    void onSave();
    void onBack();

private:
    void loadFromSettings();
    void applyToSettings();

    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;

    QPushButton *m_cancelActive = nullptr;
    QPushButton *m_clearSys = nullptr;
    QPushButton *m_clearPump = nullptr;
    QPushButton *m_restore = nullptr;
    QPushButton *m_importJson = nullptr;
    QPushButton *m_save = nullptr;
    QPushButton *m_back = nullptr;

    QLineEdit *m_manufYear = nullptr;
    QLineEdit *m_manufMonth = nullptr;
    QLineEdit *m_manufDay = nullptr;
    QLineEdit *m_instYear = nullptr;
    QLineEdit *m_instMonth = nullptr;
    QLineEdit *m_instDay = nullptr;
    QLineEdit *m_repairYear = nullptr;
    QLineEdit *m_repairMonth = nullptr;
    QLineEdit *m_repairDay = nullptr;
    QLineEdit *m_license = nullptr;
    QLineEdit *m_serial = nullptr;
    QComboBox *m_pcProto = nullptr;
    QComboBox *m_connect = nullptr;
    QLineEdit *m_pmax = nullptr;
};

#endif
