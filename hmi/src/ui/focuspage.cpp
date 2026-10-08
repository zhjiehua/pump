#include "ui/focuspage.h"
#include "ui/widgets/comboctrl.h"
#include "ui/widgets/hmitablewidget.h"
#include "ui/widgets/pagescroll.h"
#include "utils/hmikeys.h"

#include <QCheckBox>
#include <QEvent>
#include <QComboBox>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>

namespace {

QWidget *asWidget(QObject *o)
{
    return qobject_cast<QWidget *>(o);
}

QWidget *resolveInList(const QObjectList &list, QWidget *w)
{
    while (w)
    {
        if (list.contains(w))
            return w;
        w = w->parentWidget();
    }
    return nullptr;
}

bool isFocusable(QWidget *w)
{
    return w && w->isVisible() && w->isEnabled();
}

QWidget *moveInList(const QObjectList &list, QWidget *current, bool next)
{
    const int n = list.size();
    if (n <= 0)
        return nullptr;

    QWidget *obj = resolveInList(list, current);
    int index = obj ? list.indexOf(obj) : -1;

    for (int i = 0; i < n; ++i)
    {
        if (next)
        {
            if (index == -1 || index == n - 1)
                index = -1;
            index++;
        }
        else
        {
            if (index == -1)
                index = 0;
            if (index == 0)
                index = n;
            index--;
        }

        if (QWidget *w = asWidget(list.at(index)))
        {
            if (isFocusable(w))
                return w;
        }
    }
    return nullptr;
}

QComboBox *comboForWidget(QWidget *w)
{
    while (w)
    {
        if (auto *cb = qobject_cast<QComboBox *>(w))
            return cb;
        w = w->parentWidget();
    }
    return nullptr;
}

} // namespace

FocusPage::FocusPage(QWidget *parent)
    : QWidget(parent)
{
}

void FocusPage::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::LanguageChange)
        retranslateUi();
}

bool FocusPage::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
        ensureWidgetInScroll(qobject_cast<QWidget *>(obj));
    return QWidget::eventFilter(obj, event);
}

bool FocusPage::handleFocusNavKey(int key)
{
    if (auto *tbl = HmiTableWidget::owningTable(focusWidget()))
    {
        if (tbl->handleInnerNavKey(key))
            return true;
    }

    QComboBox *cb = comboForWidget(focusWidget());
    if (!cb)
        return false;
    if (auto *cc = qobject_cast<ComboCtrl *>(cb))
    {
        if (!cc->isPopupOpen())
            return false;
    }
    else if (!cb->view() || !cb->view()->isVisible())
    {
        return false;
    }

    const bool up = key == KEY_UP;
    const bool down = key == KEY_DOWN;
    if (!up && !down)
        return false;

    QListView *view = qobject_cast<QListView *>(cb->view());
    if (!view || !view->selectionModel())
        return false;

    const QModelIndex index = view->selectionModel()->currentIndex();
    int row = index.row();
    const int count = cb->count();
    if (count <= 0)
        return false;

    if (up)
        row = row <= 0 ? count - 1 : row - 1;
    else
        row = (row + 1) % count;

    const QModelIndex next = cb->model()->index(row, 0);
    view->setCurrentIndex(next);
    return true;
}

void FocusPage::prepareFocusWidget(QWidget *w)
{
    if (!w)
        return;
    w->setFocusPolicy(Qt::StrongFocus);
    if (!w->styleSheet().isEmpty())
        return;
    if (auto *btn = qobject_cast<QPushButton *>(w))
        btn->setStyleSheet(QStringLiteral(
            "QPushButton{outline:0;}"
            "QPushButton:focus{border:2px solid blue;outline:0;}"));
    else if (auto *le = qobject_cast<QLineEdit *>(w))
        le->setStyleSheet(QStringLiteral(
            "QLineEdit:focus{border:2px solid blue;outline:0;}"));
    else if (auto *cb = qobject_cast<QComboBox *>(w))
        cb->setStyleSheet(QStringLiteral(
            "QComboBox:focus{border:2px solid blue;outline:0;}"));
    else if (auto *te = qobject_cast<QTextEdit *>(w))
        te->setStyleSheet(QStringLiteral(
            "QTextEdit:focus{border:2px solid blue;outline:0;}"));
    else if (auto *chk = qobject_cast<QCheckBox *>(w))
        chk->setStyleSheet(QStringLiteral(
            "QCheckBox:focus{border:2px solid blue;outline:0;}"));
}

void FocusPage::initFocus(bool grabFocus)
{
    xList.clear();
    yList.clear();
    initFocusList();
    for (int i = 0; i < xList.size(); ++i)
    {
        if (QWidget *w = asWidget(xList.at(i)))
        {
            prepareFocusWidget(w);
            w->installEventFilter(this);
        }
    }
    for (int i = 0; i < yList.size(); ++i)
    {
        if (QWidget *w = asWidget(yList.at(i)))
        {
            prepareFocusWidget(w);
            w->installEventFilter(this);
        }
    }
    if (!grabFocus)
    {
        releaseChildFocus();
        return;
    }
    QWidget *restore = lastFocusWidget();
    if (!restore)
        restore = defaultFocusWidget();
    if (restore)
        restore->setFocus();
    ensureWidgetInScroll(restore);
    // QStackedWidget may move focus back to the hidden page after show();
    // restore the entry control on the next event-loop tick.
    QTimer::singleShot(0, this, SLOT(restoreLastFocus()));
}

QWidget *FocusPage::lastFocusWidget() const
{
    QWidget *w = resolveInList(xList, focusWidget());
    if (!w)
        w = resolveInList(yList, focusWidget());
    if (!w || !w->isEnabled())
        return nullptr;
    return w;
}

void FocusPage::restoreLastFocus()
{
    QWidget *w = lastFocusWidget();
    if (!w)
        w = defaultFocusWidget();
    if (w)
        w->setFocus();
    ensureWidgetInScroll(w);
}

void FocusPage::releaseChildFocus()
{
    if (QWidget *w = focusWidget())
        w->clearFocus();
}

QWidget *FocusPage::defaultFocusWidget() const
{
    for (QObject *o : yList)
    {
        if (QWidget *w = asWidget(o))
        {
            if (w->isVisible() && w->isEnabled())
                return w;
        }
    }
    for (QObject *o : xList)
    {
        if (QWidget *w = asWidget(o))
        {
            if (w->isVisible() && w->isEnabled())
                return w;
        }
    }
    return nullptr;
}

bool FocusPage::moveSpatialFocus(int key)
{
    const bool left = key == KEY_LEFT;
    const bool right = key == KEY_RIGHT;
    const bool up = key == KEY_UP;
    const bool down = key == KEY_DOWN;

    QObjectList *list = nullptr;
    bool next = false;
    if (left)
    {
        list = &xList;
        next = false;
    }
    else if (right)
    {
        list = &xList;
        next = true;
    }
    else if (up)
    {
        list = &yList;
        next = false;
    }
    else if (down)
    {
        list = &yList;
        next = true;
    }
    else
        return false;

    if (!list || list->isEmpty())
        return false;

    QWidget *nextW = moveInList(*list, focusWidget(), next);
    if (!nextW)
        return false;
    nextW->setFocus();
    ensureWidgetInScroll(nextW);
    return true;
}
