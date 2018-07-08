#ifndef TOPWIDGET_H
#define TOPWIDGET_H

#include <QWidget>
#include "ui_topwidget.h"

class TopWidget : public QWidget
{
	Q_OBJECT

public:
	TopWidget(QWidget *parent = 0);
	~TopWidget();

	void setTitle(QString title);

private:
	Ui::TopWidget ui;
};

#endif // TOPWIDGET_H
