#include "utils/spatialfocus.h"

#include <QAbstractButton>
#include <QAbstractSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QTextEdit>
#include <Qt>

namespace {

bool isFocusCandidate(QWidget *w)
{
    if (!w || !w->isVisible() || !w->isEnabled())
        return false;
    if (w->focusPolicy() == Qt::NoFocus)
        return false;
    return qobject_cast<QAbstractButton *>(w) || qobject_cast<QLineEdit *>(w)
           || qobject_cast<QComboBox *>(w) || qobject_cast<QAbstractSpinBox *>(w)
           || qobject_cast<QTableWidget *>(w) || qobject_cast<QCheckBox *>(w)
           || qobject_cast<QTextEdit *>(w);
}

void walk(QWidget *root, QList<QWidget *> *out)
{
    if (isFocusCandidate(root))
        out->append(root);
    const QObjectList kids = root->children();
    for (QObject *o : kids)
    {
        if (auto *child = qobject_cast<QWidget *>(o))
            walk(child, out);
    }
}

QPoint centerInRoot(QWidget *w, QWidget *root)
{
    const QPoint g = w->mapToGlobal(w->rect().center());
    return root->mapFromGlobal(g);
}

} // namespace

QList<QWidget *> HmiSpatialFocus::collectFocusables(QWidget *root)
{
    QList<QWidget *> list;
    if (!root)
        return list;
    walk(root, &list);
    return list;
}

QWidget *HmiSpatialFocus::defaultWidget(QWidget *root)
{
    const QList<QWidget *> list = collectFocusables(root);
    QWidget *best = nullptr;
    QPoint bestPt(1 << 30, 1 << 30);
    for (QWidget *w : list)
    {
        const QPoint c = centerInRoot(w, root);
        if (c.y() < bestPt.y() || (c.y() == bestPt.y() && c.x() < bestPt.x()))
        {
            bestPt = c;
            best = w;
        }
    }
    return best;
}

static QWidget *resolveListedFocus(QWidget *root, QWidget *raw, const QList<QWidget *> &list)
{
    QWidget *w = raw;
    while (w && w != root)
    {
        if (list.contains(w))
            return w;
        w = w->parentWidget();
    }
    return nullptr;
}

QWidget *HmiSpatialFocus::neighbor(QWidget *root, QWidget *current, int key)
{
    const QList<QWidget *> list = collectFocusables(root);
    if (list.isEmpty())
        return nullptr;

    QWidget *curWidget = resolveListedFocus(root, current, list);
    if (!curWidget)
        return defaultWidget(root);

    const QPoint cur = centerInRoot(curWidget, root);
    QWidget *best = nullptr;
    double bestScore = 1e30;
    const int slack = 6;

    for (QWidget *w : list)
    {
        if (w == curWidget)
            continue;
        const QPoint c = centerInRoot(w, root);
        const int dx = c.x() - cur.x();
        const int dy = c.y() - cur.y();

        switch (key)
        {
        case Qt::Key_Left:
            if (dx >= -slack)
                continue;
            break;
        case Qt::Key_Right:
            if (dx <= slack)
                continue;
            break;
        case Qt::Key_Up:
            if (dy >= -slack)
                continue;
            break;
        case Qt::Key_Down:
            if (dy <= slack)
                continue;
            break;
        default:
            continue;
        }

        double score = 0;
        if (key == Qt::Key_Left || key == Qt::Key_Right)
            score = qAbs(dx) + qAbs(dy) * 2.5;
        else
            score = qAbs(dy) + qAbs(dx) * 2.5;

        if (score < bestScore)
        {
            bestScore = score;
            best = w;
        }
    }
    return best;
}
