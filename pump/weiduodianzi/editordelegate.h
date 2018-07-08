#ifndef EDITORDELEGATE_H
#define EDITORDELEGATE_H

#include <QItemDelegate>

class EditorDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	EditorDelegate(QString inputMask, QObject *parent = NULL);
	~EditorDelegate();

	QWidget *createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const; 
	void setEditorData ( QWidget * editor, const QModelIndex & index ) const;
	void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const;
	void updateEditorGeometry ( QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

private:
	QString m_inputMask;
	
};

#endif // EDITORDELEGATE_H
