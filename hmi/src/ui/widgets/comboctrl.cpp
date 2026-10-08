#include "ui/widgets/comboctrl.h"
#include "core/picturemanager.h"
#include "utils/hmikeys.h"

#include <QAbstractItemView>
#include <QEvent>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

ComboCtrl::ComboCtrl(QWidget *parent)
    : QComboBox(parent)
{
    const QString arrow = PictureManager::instance().url(PictureManager::DownArrow);
    setStyleSheet(QStringLiteral(
                      "QComboBox::drop-down{width:20px;height:20px;border-image:url(%1);}"
                      "QComboBox::drop-down:focus{width:15px;height:15px;border-image:url(%1);}"
                      "QComboBox:focus{border:2px solid blue;outline:0;}")
                      .arg(arrow));
}

bool ComboCtrl::isPopupOpen() const
{
    // Do not call view() here: QComboBox::view() lazily creates the popup
    // widget and re-enters event() via setParent, which overflows the stack.
    return m_popupOpen;
}

void ComboCtrl::setChangeLocked(bool locked)
{
    m_changeLocked = locked;
    if (locked && isPopupOpen())
        hidePopup();
}

bool ComboCtrl::rejectIfLocked()
{
    if (!m_changeLocked)
        return false;
    emit changeBlocked();
    return true;
}

void ComboCtrl::installPopupFilter()
{
    QAbstractItemView *v = view();
    if (!v)
        return;
    v->installEventFilter(this);
    if (v->viewport())
        v->viewport()->installEventFilter(this);
    if (v->parentWidget())
        v->parentWidget()->installEventFilter(this);
}

void ComboCtrl::showPopup()
{
    if (rejectIfLocked())
        return;
    QComboBox::showPopup();
    installPopupFilter();
    if (!m_popupOpen)
    {
        m_popupOpen = true;
        emit popupChanged(true);
    }
}

void ComboCtrl::hidePopup()
{
    QComboBox::hidePopup();
    if (m_popupOpen)
    {
        m_popupOpen = false;
        emit popupChanged(false);
    }
}

void ComboCtrl::confirmPopup()
{
    int row = currentIndex();
    if (QAbstractItemView *v = view())
    {
        if (v->selectionModel())
        {
            const int highlighted = v->selectionModel()->currentIndex().row();
            if (highlighted >= 0 && highlighted < count())
                row = highlighted;
        }
    }
    if (row >= 0 && row < count())
        setCurrentIndex(row);
    hidePopup();
    if (row >= 0 && row < count())
        emit activated(row);
}

bool ComboCtrl::event(QEvent *event)
{
    // Popup is a separate window; ShortcutOverride often hits the combo itself.
    if (isPopupOpen() && event->type() == QEvent::ShortcutOverride)
    {
        const int key = static_cast<QKeyEvent *>(event)->key();
        if (key == KEY_UP || key == KEY_DOWN
            || key == KEY_RETURN || key == Qt::Key_Enter
            || key == KEY_BACKSPACE || key == Qt::Key_Escape)
        {
            event->accept();
            return true;
        }
    }
    return QComboBox::event(event);
}

bool ComboCtrl::isPopupObject(QObject *obj) const
{
    QAbstractItemView *v = view();
    if (!v || !obj)
        return false;
    return obj == v || obj == v->viewport() || obj == v->parentWidget();
}

bool ComboCtrl::movePopupRow(bool down)
{
    QAbstractItemView *v = view();
    if (!v || !v->model() || !v->selectionModel() || count() <= 0)
        return false;

    int row = v->selectionModel()->currentIndex().row();
    if (row < 0)
        row = currentIndex();
    if (down)
        row = (row + 1) % count();
    else
        row = row <= 0 ? count() - 1 : row - 1;

    const QModelIndex next = v->model()->index(row, 0);
    v->setCurrentIndex(next);
    v->selectionModel()->setCurrentIndex(next, QItemSelectionModel::ClearAndSelect);
    return true;
}

bool ComboCtrl::eventFilter(QObject *obj, QEvent *event)
{
    if (isPopupObject(obj) && event->type() == QEvent::Hide)
    {
        if (m_popupOpen)
        {
            m_popupOpen = false;
            emit popupChanged(false);
        }
        return QComboBox::eventFilter(obj, event);
    }

    if (!isPopupObject(obj))
        return QComboBox::eventFilter(obj, event);

    if (event->type() != QEvent::KeyPress && event->type() != QEvent::ShortcutOverride)
        return QComboBox::eventFilter(obj, event);

    const int key = static_cast<QKeyEvent *>(event)->key();
    const bool up = key == KEY_UP;
    const bool down = key == KEY_DOWN;
    const bool enter = key == KEY_RETURN || key == Qt::Key_Enter;
    const bool cancel = key == KEY_BACKSPACE || key == Qt::Key_Escape;

    if (!up && !down && !enter && !cancel)
        return QComboBox::eventFilter(obj, event);

    if (event->type() == QEvent::ShortcutOverride)
    {
        event->accept();
        return true;
    }

    if (up || down)
    {
        movePopupRow(down);
        return true;
    }
    if (cancel)
    {
        hidePopup();
        return true;
    }
    confirmPopup();
    return true;
}

void ComboCtrl::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    const bool opensOrChanges = key == KEY_RETURN || key == Qt::Key_Enter
            || key == Qt::Key_Up || key == Qt::Key_Down
            || key == Qt::Key_PageUp || key == Qt::Key_PageDown
            || key == Qt::Key_Home || key == Qt::Key_End;
    if (opensOrChanges && rejectIfLocked())
        return;
    if (isPopupOpen() && (key == KEY_BACKSPACE || key == Qt::Key_Escape))
    {
        hidePopup();
        return;
    }
    if (key == KEY_RETURN || key == Qt::Key_Enter)
    {
        if (isPopupOpen())
            confirmPopup();
        else
            showPopup();
        return;
    }
    QComboBox::keyPressEvent(event);
}

void ComboCtrl::wheelEvent(QWheelEvent *event)
{
    if (rejectIfLocked())
    {
        event->accept();
        return;
    }
    QComboBox::wheelEvent(event);
}

void ComboCtrl::mousePressEvent(QMouseEvent *event)
{
    if (rejectIfLocked())
    {
        event->accept();
        return;
    }
    QComboBox::mousePressEvent(event);
}
