#include "topwidget.h"

TopWidget::TopWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

TopWidget::~TopWidget()
{

}

void TopWidget::setTitle( QString title )
{
	ui.label->setText(title);
}
