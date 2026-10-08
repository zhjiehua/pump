#ifndef MAINTENANCEPAGE_H
#define MAINTENANCEPAGE_H

#include "ui/focuspage.h"

class MainWindow;
class BtnCtrl;
class QLabel;

/** Maintenance area hub (sub-pages like Setup). Enter via Ctrl+Up + admin password. */
class MaintenancePage : public FocusPage
{
    Q_OBJECT
public:
    explicit MaintenancePage(MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void goMaintenance();
    void goDevice();
    void goData();
    void goMcu();
    void goCds();
    void goMachine();
    void goSystem();
    void goRecords();

private:
    MainWindow *m_main = nullptr;

    BtnCtrl *m_maint = nullptr;
    BtnCtrl *m_device = nullptr;
    BtnCtrl *m_data = nullptr;
    BtnCtrl *m_mcu = nullptr;
    BtnCtrl *m_cds = nullptr;
    BtnCtrl *m_machine = nullptr;
    BtnCtrl *m_system = nullptr;
    BtnCtrl *m_records = nullptr;

    QLabel *m_maintLabel = nullptr;
    QLabel *m_deviceLabel = nullptr;
    QLabel *m_dataLabel = nullptr;
    QLabel *m_mcuLabel = nullptr;
    QLabel *m_cdsLabel = nullptr;
    QLabel *m_machineLabel = nullptr;
    QLabel *m_systemLabel = nullptr;
    QLabel *m_recordsLabel = nullptr;
};

#endif
