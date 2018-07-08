#include "imgbutton.h"
#include <QPainter>

ImgButton::ImgButton(QWidget *parent)
	: QPushButton(parent)
	, m_bkPath("")
{

}

ImgButton::~ImgButton()
{

}

void ImgButton::paintEvent(QPaintEvent * event)
{
	 QPainter painter(this);
	 if(m_bkPath.isEmpty())
	 {
		 painter.setPen(Qt::white);
		 painter.drawText(0, 0, width(), height(), Qt::AlignCenter, text());
	 }
	 else//如果有背景图片显示;
	 {
		 QPixmap pixmap(m_bkPath);
		 painter.drawPixmap(0,0,width(),height(),pixmap );
	 }
}

void ImgButton::setBkImage(QString strTemp)
{
	m_bkPath = strTemp;
	repaint();
}
