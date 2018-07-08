#ifndef TABLEEDITOR_H
#define TABLEEDITOR_H

#include <QLineEdit>
#include <QStringList>

class TableEditor : public QLineEdit
{
	Q_OBJECT

public:
	TableEditor(QWidget *parent, QString inputMask);
	~TableEditor();

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	QString m_inputMask;
	QStringList data;
	int len;
	int bei;

private slots:

};

#endif // TABLEEDITOR_H
