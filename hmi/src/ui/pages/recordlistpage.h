#ifndef RECORDLISTPAGE_H
#define RECORDLISTPAGE_H

#include "core/recordstore.h"
#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class BtnCtrl;
class QTableWidget;

/** Read-only event / alarm / maintenance record list. */
class RecordListPage : public FocusPage
{
    Q_OBJECT
public:
    RecordListPage(RecordStore::Kind kind, MachineController *c, MainWindow *main,
                   QWidget *parent = nullptr);

    bool handleFocusNavKey(int key) override;

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void onClear();

private:
    void reload();

    RecordStore::Kind m_kind = RecordStore::Event;
    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QTableWidget *m_table = nullptr;
    BtnCtrl *m_clear = nullptr;
};

#endif
