#ifndef BTNCTRL_H
#define BTNCTRL_H

#include <QPushButton>
#include <QString>

/** Panel button: Enter activates clicked() (weiduodianzi BtnCtrl). */
class BtnCtrl : public QPushButton
{
    Q_OBJECT
public:
    explicit BtnCtrl(QWidget *parent = nullptr);
    explicit BtnCtrl(const QString &text, QWidget *parent = nullptr);

protected:
    bool event(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

#endif
