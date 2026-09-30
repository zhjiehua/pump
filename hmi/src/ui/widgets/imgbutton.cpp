#include "ui/widgets/imgbutton.h"

#include <QPainter>
#include <QPaintEvent>

ImgButton::ImgButton(QWidget *parent)
    : QPushButton(parent)
{
}

void ImgButton::setBkImage(const QString &path)
{
    m_bkPath = path;
    m_pixmap = QPixmap(path);
    update();
}

void ImgButton::setBkImage(PictureManager::Picture id)
{
    setBkImage(PictureManager::instance().url(id));
}

void ImgButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    if (m_pixmap.isNull())
    {
        painter.setPen(Qt::white);
        painter.drawText(0, 0, width(), height(), Qt::AlignCenter, text());
        return;
    }
    painter.drawPixmap(0, 0, width(), height(), m_pixmap);
}
