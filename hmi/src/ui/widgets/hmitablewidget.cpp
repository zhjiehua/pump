#include "ui/widgets/hmitablewidget.h"
#include "utils/hmikeys.h"

#include <QAbstractItemView>
#include <QFocusEvent>
#include <QKeyEvent>

HmiTableWidget::HmiTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    setStyleSheet(QStringLiteral("QTableWidget:focus{border:2px solid rgb(155,200,33);}"));
    setEditTriggers(QAbstractItemView::AnyKeyPressed);
}

void HmiTableWidget::initIndex()
{
    m_currentIndex = moveCursor(QAbstractItemView::MoveNext, Qt::NoModifier);
}

void HmiTableWidget::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    const bool up = key == KEY_UP || key == PANEL_KEY_UP;
    const bool down = key == KEY_DOWN || key == PANEL_KEY_DOWN;
    const bool left = key == KEY_LEFT || key == PANEL_KEY_LEFT;
    const bool right = key == KEY_RIGHT || key == PANEL_KEY_RIGHT;

    if (up)
    {
        const QModelIndex index = currentIndex();
        int row = index.row();
        if (row > 0)
        {
            row--;
            const QModelIndex next = model()->index(row, index.column());
            m_currentIndex = next;
            setCurrentIndex(next);
        }
        else
        {
            emit outOfTableFocus(0);
            if (selectionModel())
                selectionModel()->clear();
        }
        return;
    }
    if (down)
    {
        const QModelIndex index = currentIndex();
        int row = index.row();
        if (row < model()->rowCount() - 1)
        {
            row++;
            const QModelIndex next = model()->index(row, index.column());
            m_currentIndex = next;
            setCurrentIndex(next);
        }
        else
        {
            emit outOfTableFocus(1);
            if (selectionModel())
                selectionModel()->clear();
        }
        return;
    }
    if (left)
    {
        const QModelIndex index = currentIndex();
        int col = index.column();
        if (col > 0)
        {
            col--;
            const QModelIndex next = model()->index(index.row(), col);
            m_currentIndex = next;
            setCurrentIndex(next);
        }
        else if (index.row() > 0)
        {
            const int row = index.row() - 1;
            col = model()->columnCount() - 1;
            const QModelIndex next = model()->index(row, col);
            m_currentIndex = next;
            setCurrentIndex(next);
        }
        else
        {
            emit outOfTableFocus(2);
            if (selectionModel())
                selectionModel()->clear();
        }
        return;
    }
    if (right)
    {
        const QModelIndex index = currentIndex();
        int col = index.column();
        if (col < model()->columnCount() - 1 && col >= 0)
        {
            col++;
            const QModelIndex next = model()->index(index.row(), col);
            m_currentIndex = next;
            setCurrentIndex(next);
        }
        else
        {
            emit outOfTableFocus(3);
            if (selectionModel())
                selectionModel()->clear();
        }
        return;
    }
    if (key == KEY_BACKSPACE)
    {
        const int row = currentIndex().row();
        if (row >= 0)
            removeRow(row);
        return;
    }
    if (key == KEY_RETURN || key == Qt::Key_Enter)
    {
        if (currentIndex().row() == model()->rowCount() - 1)
            insertRow(model()->rowCount());
        return;
    }
    QTableWidget::keyPressEvent(event);
}

void HmiTableWidget::focusInEvent(QFocusEvent *event)
{
    QTableWidget::focusInEvent(event);

    const bool currentValid = m_currentIndex.isValid();
    if (selectionModel() && !currentValid)
    {
        const QModelIndex index = moveCursor(QAbstractItemView::MoveNext, Qt::NoModifier);
        if (index.isValid() && event->reason() != Qt::MouseFocusReason)
            selectionModel()->setCurrentIndex(index, QItemSelectionModel::NoUpdate);
        setCurrentIndex(index);
        m_currentIndex = index;
    }
    else if (selectionModel() && currentValid)
    {
        selectionModel()->setCurrentIndex(m_currentIndex, QItemSelectionModel::NoUpdate);
        setCurrentIndex(m_currentIndex);
    }

    m_capturesPanelKeys = true;
    emit panelShortcutsEnabled(false);
}

void HmiTableWidget::focusOutEvent(QFocusEvent *event)
{
    m_capturesPanelKeys = false;
    emit panelShortcutsEnabled(true);
    QTableWidget::focusOutEvent(event);
}
