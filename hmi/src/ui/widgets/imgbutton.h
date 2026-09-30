#ifndef IMGBUTTON_H
#define IMGBUTTON_H

#include "core/picturemanager.h"

#include <QPixmap>
#include <QPushButton>
#include <QString>

/** Decorative image button (weiduodianzi ImgButton). */
class ImgButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ImgButton(QWidget *parent = nullptr);

    void setBkImage(const QString &path);
    void setBkImage(PictureManager::Picture id);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_bkPath;
    QPixmap m_pixmap;
};

#endif
