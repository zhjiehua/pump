#include "editordelegate.h"
#include "tableeditor.h"


EditorDelegate::EditorDelegate(QString inputMask, QObject *parent)
	:  QItemDelegate(parent)
{
	m_inputMask = inputMask;
}

EditorDelegate::~EditorDelegate()
{

}

QWidget * EditorDelegate::createEditor( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QLineEdit *editor = new TableEditor(parent, m_inputMask);
	return editor;  
}

void EditorDelegate::setEditorData( QWidget * editor, const QModelIndex & index ) const
{
	QString text = index.model()->data(index, Qt::EditRole).toString();  
	QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);  
	lineEdit->setText(text);  
}

void EditorDelegate::setModelData( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
	TableEditor *lineEdit = static_cast<TableEditor*>(editor);  
	QString text = lineEdit->text();  
	model->setData(index, text, Qt::EditRole);  
}

void EditorDelegate::updateEditorGeometry( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	editor->setGeometry(option.rect);
}
