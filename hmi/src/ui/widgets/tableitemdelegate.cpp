#include "ui/widgets/tableitemdelegate.h"
#include "ui/widgets/tablecelleditor.h"

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
    static_cast<QLineEdit *>(editor)->setText(text);
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
    editor->setGeometry(option.rect);
}
