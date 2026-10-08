#ifndef ADMINDATAPAGE_H
#define ADMINDATAPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class BtnCtrl;

class AdminDataPage : public FocusPage
{
    Q_OBJECT
public:
    AdminDataPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onSaveFactory();
    void onRestoreFactory();
    void onRestoreDefaults();
    void onExportData();
    void onImportData();
    void onExportRecords();

private:
    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;

    BtnCtrl *m_saveFactory = nullptr;
    BtnCtrl *m_restoreFactory = nullptr;
    BtnCtrl *m_restoreDefaults = nullptr;
    BtnCtrl *m_exportData = nullptr;
    BtnCtrl *m_importData = nullptr;
    BtnCtrl *m_exportRecords = nullptr;
};

#endif
