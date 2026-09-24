#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVector>

class QShortcut;
class MachineController;
class TopBar;
class BottomBar;
class GradientTablePage;
class PwdPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    enum Page {
        Logo = 0,
        Run,
        Param,
        Setup,
        Fix,
        FlowFix,
        PressFix,
        PulseFix,
        PressCompen,
        Admin,
        Net,
        Internal,
        Language,
        Time,
        Msg,
        Permit,
        Glp,
        Pwd,
        Gradient,
        GradientTable,
        DebugMcu
    };

    void go(Page p);
    void goBack();
    void navigate(Page p);
    void rebuildScale();
    void retranslateUi();
    void setPageTitle(const QString &title);
    MachineController *controller() const { return m_ctrl; }

    void tryLogin(const QString &pwd);
    void requestAdminAccess();
    bool pendingAdmin() const { return m_pendingAdmin; }
    void goGradientTable(int which);
    void setPanelShortcutsEnabled(bool enabled);

public slots:
    void onEditCtrlEditingChanged(bool editing);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *e) override;

private slots:
    void onRunClicked();
    void onParamClicked();
    void onSetupClicked();
    void onLogLine(const QString &s);
    void onStatusChanged();
    void onAlarmChanged();
    void onProbationExpired();
    void onStackPageChanged(int index);

    void focusNextLeftChild();
    void focusNextRightChild();
    void focusNextUpChild();
    void focusNextDownChild();
    void shortCutPumpStop();
    void shortCutStartHold();
    void shortCutPurge();
    void shortCutSuper();
    void shortCutActivateFocus();
    void onEscapeKey();

private:
    void initShortcuts();
    bool panelKeysEnabled() const;
    void syncChrome(Page p);
    QString titleFor(Page p) const;
    bool checkNavPermission() const;
    bool isNavigatorMode() const { return m_navigatorMode; }
    void setNavigatorMode(bool mode);
    void enterNavigatorMode();
    void navigatorPageAt(int index, bool force = false);
    class FocusPage *currentFocusPage() const;

    MachineController *m_ctrl = nullptr;
    QWidget *m_panel = nullptr;
    TopBar *m_top = nullptr;
    BottomBar *m_bottom = nullptr;
    QStackedWidget *m_stack = nullptr;
    GradientTablePage *m_gradTable = nullptr;
    PwdPage *m_pwdPage = nullptr;
    QVector<int> m_history;
    QVector<QShortcut *> m_shortcuts;
    bool m_pendingAdmin = true;
    Page m_pwdTarget = Admin;
    bool m_navigatorMode = true;
    int m_navigatorCnt = 3;
    int m_currentNavigator = 0;
};

#endif
