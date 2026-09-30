#include "ui/widgets/tableitemdelegate.h"
#include "ui/widgets/tablecelleditor.h"
#include "utils/hmikeys.h"

#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>

TableItemDelegate::TableItemDelegate(const QString &inputMask, QObject *parent)
    : QItemDelegate(parent)
    , m_inputMask(inputMask)
{
}

QWidget *TableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &,
                                         const QModelIndex &) const
{
    return new TableCellEditor(parent, m_inputMask);
}

void TableItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QString text = index.model()->data(index, Qt::EditRole).toString();
    auto *lineEdit = static_cast<QLineEdit *>(editor);
    lineEdit->setText(text);
    lineEdit->selectAll();
}

void TableItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                     const QModelIndex &index) const
{
    const QString text = static_cast<TableCellEditor *>(editor)->text();
    model->setData(index, text, Qt::EditRole);
}

void TableItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                             const QModelIndex &) const
{
    editor->setGeometry(option.rect.adjusted(-2, -2, 2, 2));
}

bool TableItemDelegate::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride || event->type() == QEvent::KeyPress)
    {
        const int key = static_cast<QKeyEvent *>(event)->key();
        const bool cancel = key == KEY_BACKSPACE || key == Qt::Key_Escape;
        if (cancel)
        {
            if (event->type() == QEvent::ShortcutOverride)
            {
                event->accept();
                return true;
            }
            auto *editor = qobject_cast<QWidget *>(object);
            emit closeEditor(editor, QAbstractItemDelegate::RevertModelCache);
            return true;
        }
    }
    return QItemDelegate::eventFilter(object, event);
}
