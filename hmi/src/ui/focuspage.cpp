#include "ui/focuspage.h"
#include "utils/hmikeys.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QTextEdit>

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

QWidget *moveInList(const QObjectList &list, QWidget *current, bool next)
{
    if (list.isEmpty())
        return nullptr;

    QWidget *obj = resolveInList(list, current);
    int index = obj ? list.indexOf(obj) : -1;

    if (next)
    {
        if (index == -1 || index == list.size() - 1)
            index = -1;
        index++;
    }
    else
    {
        if (index == -1)
            index = 0;
        if (index == 0)
            index = list.size();
        index--;
    }

    return asWidget(list.at(index));
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

bool FocusPage::handleFocusNavKey(int key)
{
    QComboBox *cb = comboForWidget(focusWidget());
    if (!cb || !cb->view() || !cb->view()->isVisible())
        return false;

    const bool up = key == KEY_UP || key == PANEL_KEY_UP;
    const bool down = key == KEY_DOWN || key == PANEL_KEY_DOWN;
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

void FocusPage::initFocus()
{
    xList.clear();
    yList.clear();
    initFocusList();
    for (int i = 0; i < xList.size(); ++i)
        if (QWidget *w = asWidget(xList.at(i)))
            prepareFocusWidget(w);
    for (int i = 0; i < yList.size(); ++i)
    {
        if (QWidget *w = asWidget(yList.at(i)))
            prepareFocusWidget(w);
    }
    if (QWidget *w = defaultFocusWidget())
        w->setFocus();
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
    const bool left = key == KEY_LEFT || key == PANEL_KEY_LEFT;
    const bool right = key == KEY_RIGHT || key == PANEL_KEY_RIGHT;
    const bool up = key == KEY_UP || key == PANEL_KEY_UP;
    const bool down = key == KEY_DOWN || key == PANEL_KEY_DOWN;

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
    return true;
}
