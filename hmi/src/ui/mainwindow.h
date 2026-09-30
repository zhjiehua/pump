#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVector>

class QEvent;
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
        Admin,
        Net,
        Internal,
        Language,
        Time,
        Msg,
        Permit,
        Glp,
        Pwd,
        GradientTable,
        DebugMcu
    };

    void go(Page p);
    void navigate(Page p);
    void rebuildScale();
    void retranslateUi();
    void setPageTitle(const QString &title);
    MachineController *controller() const { return m_ctrl; }

    void tryLogin(const QString &pwd);
    void requestAdminAccess();
    void requestPasswordThen(Page returnPage, bool admin = true);
    bool pendingAdmin() const { return m_pendingAdmin; }
    bool consumeLoginOkFor(Page p);
    void goGradientTable();

public slots:
    void goBack();
    void setPanelShortcutsEnabled(bool enabled);
    void onEditCtrlEditingChanged(bool editing);

protected:
    void changeEvent(QEvent *e) override;
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
    void applyNavigatorFocus();

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
    bool m_loginOk = false;
    Page m_pwdTarget = Admin;
    bool m_navigatorMode = true;
    int m_navigatorCnt = 3;
    int m_currentNavigator = 0;
};

#endif
