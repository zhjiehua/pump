#include "ui/mainwindow.h"
#include "ui/focuspage.h"
#include "ui/widgets/comboctrl.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/hmitablewidget.h"
#include "ui/widgets/msgbox.h"
#include "utils/hmikeys.h"
#include "utils/hmiconfig.h"
#include "utils/qtwidgetsutil.h"
#include "core/machinecontroller.h"
#include "core/alarmservice.h"
#include "domain/uicapabilities.h"
#include "ui/topbar.h"
#include "ui/bottombar.h"
#include "ui/pages/logopage.h"
#include "ui/pages/runpage.h"
#include "ui/pages/runparampage.h"
#include "ui/pages/setuppage.h"
#include "ui/pages/fixpage.h"
#include "ui/pages/flowfixpage.h"
#include "ui/pages/pressfixpage.h"
#include "ui/pages/maintenancepage.h"
#include "ui/pages/netpage.h"
#include "ui/pages/adminmaintenancepage.h"
#include "ui/pages/admindevicepage.h"
#include "ui/pages/admindatapage.h"
#include "ui/pages/recordpage.h"
#include "ui/pages/recordlistpage.h"
#include "ui/pages/configmcupage.h"
#include "ui/pages/configcdspage.h"
#include "ui/pages/configmachinepage.h"
#include "ui/pages/configsystempage.h"
#include "ui/pages/languagepage.h"
#include "ui/pages/timepage.h"
#include "ui/pages/msgpage.h"
#include "ui/pages/permitpage.h"
#include "ui/pages/glpinfopage.h"
#include "ui/pages/pwdpage.h"
#include "ui/pages/gradienttablepage.h"
#include "ui/pages/debugmcuprotopage.h"

#include <QAbstractButton>
#include <QApplication>
#include <QCloseEvent>
#include <QEvent>
#include <QKeyEvent>
#include <QComboBox>
#include <QLineEdit>
#include <QShortcut>
#include <QTextEdit>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Pump"));
    statusBar()->hide();
    m_ctrl = new MachineController(this);

    m_panel = new QWidget;
    m_panel->setObjectName(QStringLiteral("productPanel"));
    m_panel->setStyleSheet(QStringLiteral(
        "QWidget#productPanel{background-color:rgb(240,240,240);}"));
    auto *panelLay = new QVBoxLayout(m_panel);
    panelLay->setContentsMargins(0, 0, 0, 0);
    panelLay->setSpacing(0);

    m_top = new TopBar;
    m_stack = new QStackedWidget;
    m_bottom = new BottomBar;

    panelLay->addWidget(m_top, 2);
    panelLay->addWidget(m_stack, 8);
    panelLay->addWidget(m_bottom, 1);

    m_stack->addWidget(new LogoPage(this));                        // Logo
    m_stack->addWidget(new RunPage(m_ctrl, this));                 // Run
    m_stack->addWidget(new RunParamPage(m_ctrl, this));            // Param
    m_stack->addWidget(new SetupPage(this));                       // Setup
    m_stack->addWidget(new FixPage(this));                         // Fix
    m_stack->addWidget(new FlowFixPage(m_ctrl, this));             // FlowFix
    m_stack->addWidget(new PressFixPage(m_ctrl, this));            // PressFix
    m_stack->addWidget(new MaintenancePage(this));                 // Maintenance
    m_stack->addWidget(new AdminMaintenancePage(m_ctrl, this));    // AdminMaintenance
    m_stack->addWidget(new AdminDevicePage(m_ctrl, this));         // AdminDevice
    m_stack->addWidget(new AdminDataPage(m_ctrl, this));           // AdminData
    m_stack->addWidget(new RecordPage(this));                      // Records
    m_stack->addWidget(new RecordListPage(RecordStore::Event, m_ctrl, this));        // EventRecords
    m_stack->addWidget(new RecordListPage(RecordStore::Alarm, m_ctrl, this));        // AlarmRecords
    m_stack->addWidget(new RecordListPage(RecordStore::Maintenance, m_ctrl, this));  // MaintRecords
    m_stack->addWidget(new ConfigMcuPage(m_ctrl, this));           // ConfigMcu
    m_stack->addWidget(new ConfigCdsPage(m_ctrl, this));           // ConfigCds
    m_stack->addWidget(new ConfigMachinePage(m_ctrl, this));       // ConfigMachine
    m_stack->addWidget(new ConfigSystemPage(m_ctrl, this));        // ConfigSystem
    m_stack->addWidget(new NetPage(m_ctrl, this));                 // Net
    m_stack->addWidget(new LanguagePage(m_ctrl, this));            // Language
    m_stack->addWidget(new TimePage(m_ctrl, this));                // Time
    m_stack->addWidget(new MsgPage(m_ctrl, this));                 // Msg
    m_stack->addWidget(new PermitPage(m_ctrl, this));              // Permit
    m_stack->addWidget(new GlpInfoPage(m_ctrl, this));             // Glp
    m_pwdPage = new PwdPage(m_ctrl, this, true);
    m_stack->addWidget(m_pwdPage);                                 // Pwd
    m_gradTable = new GradientTablePage(m_ctrl, this);
    m_stack->addWidget(m_gradTable);                               // GradientTable
    m_stack->addWidget(new DebugMcuProtoPage(m_ctrl, this));         // DebugMcu

    setCentralWidget(m_panel);

    connect(m_bottom, SIGNAL(runClicked()), this, SLOT(onRunClicked()));
    connect(m_bottom, SIGNAL(paramClicked()), this, SLOT(onParamClicked()));
    connect(m_bottom, SIGNAL(setupClicked()), this, SLOT(onSetupClicked()));
    connect(m_ctrl, SIGNAL(logLine(QString)), this, SLOT(onLogLine(QString)));
    connect(m_ctrl, SIGNAL(statusChanged()), this, SLOT(onStatusChanged()));
    connect(m_ctrl, SIGNAL(alarmChanged()), this, SLOT(onAlarmChanged()));
    connect(m_ctrl, SIGNAL(probationExpired()), this, SLOT(onProbationExpired()));
    connect(m_ctrl, SIGNAL(commandRejected(QString)), this, SLOT(onCommandRejected(QString)));
    connect(m_stack, SIGNAL(currentChanged(int)), this, SLOT(onStackPageChanged(int)));

    rebuildScale();
    initShortcuts();
    m_stack->setCurrentIndex(int(Logo));
    syncChrome(Logo);
}

MainWindow::~MainWindow()
{
    // Pages call back into m_ctrl from their destructors. QObject deletes
    // children in insertion order, so m_ctrl (created first) would otherwise
    // die before the stacked pages.
#if QT_VERSION >= 0x050000
    delete takeCentralWidget();
#else
    setCentralWidget(0);
#endif
    m_panel = nullptr;
    m_stack = nullptr;
    m_top = nullptr;
    m_bottom = nullptr;
    m_gradTable = nullptr;
    m_pwdPage = nullptr;
}

void MainWindow::initShortcuts()
{
    // Same pattern as weiduodianzi BaseMainPage::initShotCut: QShortcut + setKey().
    auto add = [&](int key, const char *slot) {
        const QKeySequence seq(key);
        for (QShortcut *existing : m_shortcuts)
        {
            if (existing && existing->key() == seq)
                return;
        }
        auto *sc = new QShortcut(this);
        sc->setKey(key);
        connect(sc, SIGNAL(activated()), this, slot);
        m_shortcuts.append(sc);
    };

    add(KEY_UP, SLOT(focusNextUpChild()));
    add(KEY_DOWN, SLOT(focusNextDownChild()));
    add(KEY_LEFT, SLOT(focusNextLeftChild()));
    add(KEY_RIGHT, SLOT(focusNextRightChild()));
    add(KEY_BACKSPACE, SLOT(goBack()));
    add(KEY_PUMPSTOP, SLOT(shortCutPumpStop()));
    add(KEY_STARTHOLD, SLOT(shortCutStartHold()));
    add(KEY_PURGE, SLOT(shortCutPurge()));
    add(KEY_RETURN, SLOT(shortCutActivateFocus()));
    add(Qt::Key_Enter, SLOT(shortCutActivateFocus()));
    add(Qt::Key_Escape, SLOT(onEscapeKey()));
    add(KEY_SUPER, SLOT(shortCutSuper()));
}

FocusPage *MainWindow::currentFocusPage() const
{
    return qobject_cast<FocusPage *>(m_stack->currentWidget());
}

bool MainWindow::panelKeysEnabled() const
{
    QWidget *w = QWidget::focusWidget();
    while (w)
    {
        if (auto *tbl = qobject_cast<HmiTableWidget *>(w))
        {
            if (tbl->capturesPanelKeys())
                return false;
        }
        if (auto *ec = qobject_cast<EditCtrl *>(w))
            return !ec->isEditing();
        if (auto *cb = qobject_cast<ComboCtrl *>(w))
            return !cb->isPopupOpen();
        w = w->parentWidget();
    }
    return true;
}

bool MainWindow::checkNavPermission() const
{
    using Stat = MachineController::Stat;
    return m_ctrl->stat() == Stat::Stop;
}

void MainWindow::setNavigatorMode(bool mode)
{
    m_navigatorMode = mode;
    if (mode)
        enterNavigatorMode();
}

void MainWindow::navigatorPageAt(int index, bool force)
{
    if (index < 0 || index >= m_navigatorCnt)
        return;
    if (!checkNavPermission() && !force)
        return;

    m_currentNavigator = index;
    static const Page navPages[] = {Run, Param, Setup};
    m_bottom->setActiveNav(index);
    navigate(navPages[index]);
    if (m_navigatorMode)
        enterNavigatorMode();
}

void MainWindow::setPanelShortcutsEnabled(bool enabled)
{
    for (QShortcut *sc : m_shortcuts)
    {
        if (!sc)
            continue;
        const QKeySequence seq = sc->key();
        // Always allow Backspace → goBack (legacy: global shortcut unless EditCtrl editing).
        if (!enabled && (seq == QKeySequence(KEY_BACKSPACE)))
            sc->setEnabled(true);
        else
            sc->setEnabled(enabled);
    }
}

void MainWindow::onEditCtrlEditingChanged(bool editing)
{
    setPanelShortcutsEnabled(!editing);
}

void MainWindow::shortCutActivateFocus()
{
    QWidget *w = QWidget::focusWidget();
    if (!w)
        return;
    if (auto *tbl = HmiTableWidget::owningTable(w))
    {
        if (qobject_cast<QAbstractButton *>(w) && tbl->isIndexMenuOpen())
        {
            auto *btn = qobject_cast<QAbstractButton *>(w);
            if (btn && btn->isEnabled())
                btn->click();
            return;
        }
        if (!tbl->isInside())
            tbl->enterInner();
        else
            tbl->activateCurrentCell();
        return;
    }
    if (auto *ec = qobject_cast<EditCtrl *>(w))
    {
        if (ec->isReadOnly())
            ec->startEditing();
        return;
    }
    if (auto *cb = qobject_cast<ComboCtrl *>(w))
    {
        if (cb->isPopupOpen())
            cb->confirmPopup();
        else
            cb->showPopup();
        return;
    }
    if (auto *cb = qobject_cast<QComboBox *>(w))
    {
        cb->showPopup();
        return;
    }
    if (auto *le = qobject_cast<QLineEdit *>(w))
    {
        le->selectAll();
        return;
    }
    if (qobject_cast<QTextEdit *>(w))
        return;
    auto *btn = qobject_cast<QAbstractButton *>(w);
    if (btn && btn->isEnabled())
        btn->click();
}

void MainWindow::onEscapeKey()
{
    if (QWidget *w = QWidget::focusWidget())
    {
        if (auto *ec = qobject_cast<EditCtrl *>(w))
        {
            if (ec->cancelEditing())
                return;
        }
        if (auto *cb = qobject_cast<ComboCtrl *>(w))
        {
            if (cb->isPopupOpen())
            {
                cb->hidePopup();
                return;
            }
        }
        if (auto *tbl = HmiTableWidget::owningTable(w))
        {
            if (tbl->handleBack())
                return;
        }
    }

    goBack();
}

void MainWindow::enterNavigatorMode()
{
    m_navigatorMode = true;
    // Backspace is left enabled during combo/edit, so goBack() can land here
    // with Left/Right still disabled. Restore panel keys for tab switching.
    setPanelShortcutsEnabled(true);
    applyNavigatorFocus();
    // QStackedWidget restores the previous page child after setCurrentIndex
    // returns; re-apply navi focus on the next event-loop tick.
    QTimer::singleShot(0, this, SLOT(applyNavigatorFocus()));
}

void MainWindow::applyNavigatorFocus()
{
    if (!m_navigatorMode || !m_bottom || m_stack->currentIndex() == int(Logo))
        return;
    const Page p = Page(m_stack->currentIndex());
    if (p != Run && p != Param && p != Setup)
        return;
    if (auto *fp = currentFocusPage())
        fp->releaseChildFocus();
    m_bottom->focusNav(m_currentNavigator);
}

bool MainWindow::moveModalMsgBoxFocus()
{
    auto *box = qobject_cast<MsgBox *>(QApplication::activeModalWidget());
    if (!box)
        return false;
    box->cycleFocus();
    return true;
}

void MainWindow::focusNextLeftChild()
{
    if (moveModalMsgBoxFocus())
        return;
    if (m_stack->currentIndex() == int(Logo))
        return;

    if (isNavigatorMode())
    {
        if (m_navigatorCnt <= 0)
            return;
        const int tIndex = m_currentNavigator == 0 ? m_navigatorCnt - 1 : m_currentNavigator - 1;
        navigatorPageAt(tIndex);
        return;
    }

    if (!panelKeysEnabled())
        return;

    if (FocusPage *page = currentFocusPage())
    {
        if (page->handleFocusNavKey(KEY_LEFT))
            return;
        page->moveSpatialFocus(KEY_LEFT);
    }
}

void MainWindow::focusNextRightChild()
{
    if (moveModalMsgBoxFocus())
        return;
    if (m_stack->currentIndex() == int(Logo))
        return;

    if (isNavigatorMode())
    {
        if (m_navigatorCnt <= 0)
            return;
        const int tIndex = (m_currentNavigator + 1) % m_navigatorCnt;
        navigatorPageAt(tIndex);
        return;
    }

    if (!panelKeysEnabled())
        return;

    if (FocusPage *page = currentFocusPage())
    {
        if (page->handleFocusNavKey(KEY_RIGHT))
            return;
        page->moveSpatialFocus(KEY_RIGHT);
    }
}

void MainWindow::focusNextUpChild()
{
    if (moveModalMsgBoxFocus())
        return;
    if (!panelKeysEnabled() || m_stack->currentIndex() == int(Logo))
        return;

    FocusPage *page = currentFocusPage();
    if (!page)
        return;

    if (isNavigatorMode())
        m_navigatorMode = false;

    if (page->handleFocusNavKey(KEY_UP))
        return;
    page->moveSpatialFocus(KEY_UP);
}

void MainWindow::focusNextDownChild()
{
    if (moveModalMsgBoxFocus())
        return;
    if (!panelKeysEnabled() || m_stack->currentIndex() == int(Logo))
        return;

    FocusPage *page = currentFocusPage();
    if (!page)
        return;

    if (isNavigatorMode())
        m_navigatorMode = false;

    if (page->handleFocusNavKey(KEY_DOWN))
        return;
    page->moveSpatialFocus(KEY_DOWN);
}

void MainWindow::shortCutPumpStop()
{
    if (!panelKeysEnabled())
        return;
    using Stat = MachineController::Stat;
    if (m_ctrl->stat() != Stat::Stop)
        m_ctrl->stop();
    else
        m_ctrl->pump();
}

void MainWindow::shortCutStartHold()
{
    if (!panelKeysEnabled())
        return;
    using Stat = MachineController::Stat;
    const Stat cur = m_ctrl->stat();
    if (cur == Stat::Stop || cur == Stat::Pause)
        m_ctrl->start();
    else if (cur == Stat::Running)
        m_ctrl->pause();
}

void MainWindow::shortCutPurge()
{
    if (!panelKeysEnabled())
        return;
    using Stat = MachineController::Stat;
    const Stat cur = m_ctrl->stat();
    if (cur != Stat::Purge && cur == Stat::Stop)
        m_ctrl->purge();
}

void MainWindow::shortCutSuper()
{
    requestMaintenanceAccess();
}

QString MainWindow::titleFor(Page p) const
{
    switch (p)
    {
    case Logo: return tr("Pump");
    case Run: return tr("Run");
    case Param: return tr("Parameters");
    case Setup: return tr("Setup");
    case Fix: return tr("Calibration");
    case FlowFix: return tr("Flow Calibration");
    case PressFix: return tr("Press Calibration");
    case Maintenance: return tr("Maintenance");
    case AdminMaintenance: return tr("Service");
    case AdminDevice: return tr("Device");
    case AdminData: return tr("Data");
    case Records: return tr("Records");
    case EventRecords: return tr("Event Records");
    case AlarmRecords: return tr("Alarm Records");
    case MaintRecords: return tr("Maintenance Records");
    case ConfigMcu: return tr("MCU");
    case ConfigCds: return tr("CDS");
    case ConfigMachine: return tr("Machine");
    case ConfigSystem: return tr("System");
    case Net: return tr("Network Configuration");
    case Language: return tr("Language");
    case Time: return tr("Time");
    case Msg: return tr("About");
    case Permit: return tr("Permission");
    case Glp: return tr("GLP Information");
    case Pwd: return tr("Password");
    case GradientTable: return tr("Gradient");
    case DebugMcu: return tr("MCU Debug");
    }
    return QStringLiteral("Pump");
}

void MainWindow::syncChrome(Page p)
{
    m_top->setTitle(titleFor(p));
    const bool splash = (p == Logo);
    m_top->setVisible(!splash);
    m_bottom->setVisible(!splash);
    if (p == Run)
    {
        m_bottom->setActiveNav(0);
        m_currentNavigator = 0;
    }
    else if (p == Param || p == GradientTable)
    {
        m_bottom->setActiveNav(1);
        m_currentNavigator = 1;
    }
    else if (p != Logo)
    {
        m_bottom->setActiveNav(2);
        m_currentNavigator = 2;
    }
}

void MainWindow::onStackPageChanged(int)
{
    setPanelShortcutsEnabled(true);

    const Page p = Page(m_stack->currentIndex());
    const bool nav = m_navigatorMode && m_bottom && (p == Run || p == Param || p == Setup);
    if (auto *fp = currentFocusPage())
        fp->initFocus(!nav);
    if (nav)
        applyNavigatorFocus();
}

void MainWindow::setPageTitle(const QString &title)
{
    m_top->setTitle(title);
}

void MainWindow::rebuildScale()
{
    const int s = qBound(1, m_ctrl->settings()->scale, 3);
    const int w = 320 * s;
    const int h = 240 * s;
    const int topH = h * 2 / 11;
    const int botH = h * 1 / 11;
    m_top->setFixedHeight(topH);
    m_bottom->setFixedHeight(botH);
    m_panel->setFixedSize(w, h);
    setFixedSize(w, h);
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    if (e->type() == QEvent::LanguageChange)
        retranslateUi();
}

void MainWindow::retranslateUi()
{
    if (m_bottom)
        m_bottom->updateLanguage();
    syncChrome(Page(m_stack->currentIndex()));
}

void MainWindow::go(Page p)
{
    const Page cur = Page(m_stack->currentIndex());
    // Password is a gate, not a destination: never push it onto the back stack.
    if (cur != Pwd)
        m_history.append(int(cur));
    if (p != Run && p != Param && p != Setup)
        m_navigatorMode = false;
    m_stack->setCurrentIndex(int(p));
    syncChrome(p);
}

void MainWindow::navigate(Page p)
{
    m_history.clear();
    m_stack->setCurrentIndex(int(p));
    syncChrome(p);
}

void MainWindow::goBack()
{
    if (QWidget *w = QWidget::focusWidget())
    {
        if (auto *cb = qobject_cast<ComboCtrl *>(w))
        {
            if (cb->isPopupOpen())
            {
                cb->hidePopup();
                return;
            }
        }
        if (auto *ec = qobject_cast<EditCtrl *>(w))
        {
            if (ec->cancelEditing())
                return;
        }
        if (auto *tbl = HmiTableWidget::owningTable(w))
        {
            if (tbl->handleBack())
                return;
        }
    }

    const Page p = Page(m_stack->currentIndex());
    // Top-level nav pages: Backspace returns focus to the bottom bar
    // only when the pump is Stop (weiduodianzi checkPermission).
    if (p == Run || p == Param || p == Setup)
    {
        if (checkNavPermission())
            enterNavigatorMode();
        return;
    }

    if (m_history.isEmpty())
    {
        m_navigatorMode = true;
        navigate(Run);
        enterNavigatorMode();
        return;
    }
    int i = hmiVectorTakeLast(&m_history);
    while (i == int(Pwd) && !m_history.isEmpty())
        i = hmiVectorTakeLast(&m_history);
    if (i == int(Pwd))
    {
        m_navigatorMode = true;
        navigate(Run);
        enterNavigatorMode();
        return;
    }
    const Page prev = Page(i);
    // Stay on the page's entry control; Backspace on Run/Param/Setup goes to navi.
    if (prev == Run || prev == Param || prev == Setup)
        m_navigatorMode = false;
    m_stack->setCurrentIndex(i);
    syncChrome(prev);
}

void MainWindow::requestMaintenanceAccess()
{
#if !HMI_EMBEDDED
    go(Maintenance);
    return;
#endif
    requestPasswordThen(Maintenance, true);
}

void MainWindow::requestPasswordThen(Page returnPage, bool admin)
{
    m_pendingAdmin = admin;
    m_pwdTarget = returnPage;
    m_loginOk = false;
    go(Pwd);
}

bool MainWindow::consumeLoginOkFor(Page p)
{
    if (!m_loginOk || m_pwdTarget != p)
        return false;
    m_loginOk = false;
    return true;
}

void MainWindow::tryLogin(const QString &pwd)
{
    auto *s = m_ctrl->settings();
    const QString expect = m_pendingAdmin ? s->adminPwd : s->userPwd;
    if (pwd == expect)
    {
        m_loginOk = true;
        const bool returning = !m_history.isEmpty() && m_history.last() == int(m_pwdTarget);
        if (returning)
        {
            m_stack->setCurrentIndex(int(m_pwdTarget));
            syncChrome(m_pwdTarget);
        }
        else
            go(m_pwdTarget);
        return;
    }
    MsgBox::warning(this, tr("Warning"), tr("Pwd error!!!"));
}

void MainWindow::goGradientTable()
{
    if (!m_ctrl->capabilities().has(UiPageKey::kLocalGradient))
        return;
    if (m_gradTable)
        m_gradTable->reload();
    go(GradientTable);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
    {
        onEscapeKey();
        return;
    }
    QMainWindow::keyPressEvent(e);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    m_ctrl->settings()->save();
    m_ctrl->disconnectMcu();
    m_ctrl->disconnectPc();
    QMainWindow::closeEvent(e);
}

void MainWindow::onRunClicked()
{
    m_navigatorMode = true;
    navigatorPageAt(0);
}

void MainWindow::onParamClicked()
{
    m_navigatorMode = true;
    navigatorPageAt(1);
}

void MainWindow::onSetupClicked()
{
    m_navigatorMode = true;
    navigatorPageAt(2);
}

void MainWindow::onLogLine(const QString &s)
{
    setWindowTitle(QStringLiteral("Pump — %1").arg(s));
}

void MainWindow::onStatusChanged()
{
    const PumpSession::Snap snap = m_ctrl->session()->copy();
    m_bottom->setLinkOk(snap.linkOk);
}

void MainWindow::onAlarmChanged()
{
    const PumpSession::Snap snap = m_ctrl->session()->copy();
    m_bottom->setLinkOk(snap.linkOk);
    m_bottom->setPressWarn(snap.pressWarnLevel);
}

void MainWindow::onCommandRejected(const QString &reason)
{
    MsgBox::warning(this, tr("Warning"), reason);
}

void MainWindow::onProbationExpired()
{
    navigate(Permit);
}
