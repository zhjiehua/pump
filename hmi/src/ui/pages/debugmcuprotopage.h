#ifndef DEBUGMCUPROTOPAGE_H
#define DEBUGMCUPROTOPAGE_H

#include "ui/focuspage.h"

class MachineController;
class MainWindow;
class QTextEdit;
class BtnCtrl;

class DebugMcuProtoPage : public FocusPage
{
    Q_OBJECT
public:
    DebugMcuProtoPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

public slots:
    void appendLog(const QString &line);

private slots:
    void onReconnect();
    void onBack();

private:
    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QTextEdit *m_log = nullptr;
    BtnCtrl *m_reconnect = nullptr;
    BtnCtrl *m_back = nullptr;
};

#endif
