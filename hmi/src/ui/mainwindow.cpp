#include "ui/mainwindow.h"
#include "ui/focuspage.h"
#include "ui/widgets/editctrl.h"
#include "ui/widgets/hmitablewidget.h"
#include "utils/hmikeys.h"
#include "utils/qtwidgetsutil.h"
#include "core/machinecontroller.h"
#include "core/alarmservice.h"
#include "ui/topbar.h"
#include "ui/bottombar.h"
#include "ui/pages/logopage.h"
#include "ui/pages/runpage.h"
#include "ui/pages/runparampage.h"
#include "ui/pages/setuppage.h"
#include "ui/pages/fixpage.h"
#include "ui/pages/flowfixpage.h"
#include "ui/pages/pressfixpage.h"
#include "ui/pages/pulsefixpage.h"
#include "ui/pages/presscompenpage.h"
#include "ui/pages/adminpage.h"
#include "ui/pages/netpage.h"
#include "ui/pages/internalconfigpage.h"
#include "ui/pages/languagepage.h"
#include "ui/pages/timepage.h"
#include "ui/pages/msgpage.h"
#include "ui/pages/permitpage.h"
#include "ui/pages/glpinfopage.h"
#include "ui/pages/pwdpage.h"
#include "ui/pages/gradientpage.h"
#include "ui/pages/gradienttablepage.h"
#include "ui/pages/debugmcuprotopage.h"

#include <QAbstractButton>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QShortcut>
#include <QTextEdit>
#include <QStatusBar>
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
    m_stack->addWidget(new PulseFixPage(m_ctrl, this));            // PulseFix
    m_stack->addWidget(new PressCompenPage(m_ctrl, this));         // PressCompen
    m_stack->addWidget(new AdminPage(m_ctrl, this));               // Admin
    m_stack->addWidget(new NetPage(m_ctrl, this));                 // Net
    m_stack->addWidget(new InternalConfigPage(m_ctrl, this));      // Internal
    m_stack->addWidget(new LanguagePage(m_ctrl, this));            // Language
    m_stack->addWidget(new TimePage(m_ctrl, this));                // Time
    m_stack->addWidget(new MsgPage(m_ctrl, this));                 // Msg
    m_stack->addWidget(new PermitPage(m_ctrl, this));              // Permit
    m_stack->addWidget(new GlpInfoPage(m_ctrl, this));             // Glp
    m_pwdPage = new PwdPage(m_ctrl, this, true);
    m_stack->addWidget(m_pwdPage);                                 // Pwd
    m_stack->addWidget(new GradientPage(m_ctrl, this));            // Gradient
    m_gradTable = new GradientTablePage(m_ctrl, this, 0);
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
    connect(m_stack, SIGNAL(currentChanged(int)), this, SLOT(onStackPageChanged(int)));

    rebuildScale();
    initShortcuts();
    m_stack->setCurrentIndex(int(Logo));
    syncChrome(Logo);
}

void MainWindow::initShortcuts()
{
    auto add = [&](int key, const char *slot) {
        auto *sc = new QShortcut(QKeySequence(key), this);
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
    add(PANEL_KEY_UP, SLOT(focusNextUpChild()));
    add(PANEL_KEY_DOWN, SLOT(focusNextDownChild()));
    add(PANEL_KEY_LEFT, SLOT(focusNextLeftChild()));
    add(PANEL_KEY_RIGHT, SLOT(focusNextRightChild()));
    {
        auto *sc = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up), this);
        connect(sc, SIGNAL(activated()), this, SLOT(shortCutSuper()));
        m_shortcuts.append(sc);
    }
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
    if (mode && m_bottom)
        m_bottom->focusNav(m_currentNavigator);
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
        m_bottom->focusNav(index);
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
    if (auto *ec = qobject_cast<EditCtrl *>(w))
    {
        if (ec->isReadOnly())
            ec->startEditing();
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
    }

    const Page p = Page(m_stack->currentIndex());
    if ((p == Run || p == Param || p == Setup) && !m_navigatorMode)
    {
        enterNavigatorMode();
        return;
    }
    goBack();
}

void MainWindow::enterNavigatorMode()
{
    m_navigatorMode = true;
    if (!m_bottom || m_stack->currentIndex() == int(Logo))
        return;
    const Page p = Page(m_stack->currentIndex());
    if (p == Run || p == Param || p == Setup)
        m_bottom->focusNav(m_currentNavigator);
}

void MainWindow::focusNextLeftChild()
{
    if (!panelKeysEnabled() || m_stack->currentIndex() == int(Logo))
        return;

    if (isNavigatorMode())
    {
        if (m_navigatorCnt <= 0)
            return;
        const int tIndex = m_currentNavigator == 0 ? m_navigatorCnt - 1 : m_currentNavigator - 1;
        navigatorPageAt(tIndex);
        return;
    }

    if (FocusPage *page = currentFocusPage())
    {
        if (page->handleFocusNavKey(KEY_LEFT))
            return;
        page->moveSpatialFocus(KEY_LEFT);
    }
}

void MainWindow::focusNextRightChild()
{
    if (!panelKeysEnabled() || m_stack->currentIndex() == int(Logo))
        return;

    if (isNavigatorMode())
    {
        if (m_navigatorCnt <= 0)
            return;
        const int tIndex = (m_currentNavigator + 1) % m_navigatorCnt;
        navigatorPageAt(tIndex);
        return;
    }

    if (FocusPage *page = currentFocusPage())
    {
        if (page->handleFocusNavKey(KEY_RIGHT))
            return;
        page->moveSpatialFocus(KEY_RIGHT);
    }
}

void MainWindow::focusNextUpChild()
{
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
    requestAdminAccess();
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
    case PulseFix: return tr("Pulse Compen");
    case PressCompen: return tr("Press Compen");
    case Admin: return tr("Admin");
    case Net: return tr("Network Configuration");
    case Internal: return tr("Internal");
    case Language: return tr("Language");
    case Time: return tr("Time");
    case Msg: return tr("About");
    case Permit: return tr("Permission");
    case Glp: return tr("GLP Information");
    case Pwd: return tr("Password");
    case Gradient: return tr("Gradient");
    case GradientTable: return tr("Gradient");
    case DebugMcu: return tr("MCU Debug");
    }
    return QStringLiteral("Pump");
}

void MainWindow::syncChrome(Page p)
{
    m_top->setTitle(titleFor(p));
    if (p == Logo)
        m_bottom->hide();
    else
        m_bottom->show();
    if (p == Run)
    {
        m_bottom->setActiveNav(0);
        m_currentNavigator = 0;
    }
    else if (p == Param || p == Gradient || p == GradientTable)
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
    if (m_navigatorMode && m_bottom && (p == Run || p == Param || p == Setup))
    {
        m_bottom->focusNav(m_currentNavigator);
        return;
    }
    if (auto *fp = currentFocusPage())
        fp->initFocus();
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

void MainWindow::retranslateUi()
{
    syncChrome(Page(m_stack->currentIndex()));
}

void MainWindow::go(Page p)
{
    m_history.append(m_stack->currentIndex());
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
    if (m_history.isEmpty())
    {
        m_navigatorMode = true;
        navigate(Run);
        enterNavigatorMode();
        return;
    }
    const int i = hmiVectorTakeLast(&m_history);
    const Page p = Page(i);
    if (p == Run || p == Param || p == Setup)
        m_navigatorMode = true;
    m_stack->setCurrentIndex(i);
    syncChrome(p);
    if (p == Run || p == Param || p == Setup)
        enterNavigatorMode();
}

void MainWindow::requestAdminAccess()
{
    m_pendingAdmin = true;
    m_pwdTarget = Admin;
    go(Pwd);
}

void MainWindow::tryLogin(const QString &pwd)
{
    auto *s = m_ctrl->settings();
    const QString expect = m_pendingAdmin ? s->adminPwd : s->userPwd;
    if (pwd == expect)
    {
        go(m_pwdTarget);
        return;
    }
    QMessageBox::warning(this, tr("Warning"), tr("Pwd error!!!"));
}

void MainWindow::goGradientTable(int which)
{
    if (m_gradTable)
    {
        m_gradTable->setWhich(which);
        m_gradTable->reload();
    }
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
    m_bottom->setLinkOk(m_ctrl->linkOk());
}

void MainWindow::onAlarmChanged()
{
    m_bottom->setLinkOk(m_ctrl->linkOk());
    m_bottom->setPressWarn(m_ctrl->alarms()->pressWarnLevel());
}

void MainWindow::onProbationExpired()
{
    navigate(Permit);
}
