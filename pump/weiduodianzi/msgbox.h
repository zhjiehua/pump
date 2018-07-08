#ifndef MSGBOX_H
#define MSGBOX_H

#include <QMessageBox>

class MsgBox : public QMessageBox
{
	Q_OBJECT

public:
	MsgBox(QWidget *parent, const QString & title, const QString & text, StandardButtons buttons = QMessageBox::Ok|QMessageBox::Cancel);
	~MsgBox();

protected:
	void keyPressEvent(QKeyEvent *event);

private:
	
};

#endif // MSGBOX_H
