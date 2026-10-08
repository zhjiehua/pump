#ifndef MSGBOX_H
#define MSGBOX_H

#include <QMessageBox>

class QKeyEvent;
class QShowEvent;

/** Panel-key message box (weiduodianzi MsgBox): WASD cycles Yes/No (or Ok). */
class MsgBox : public QMessageBox
{
    Q_OBJECT
public:
    MsgBox(QWidget *parent, const QString &title, const QString &text,
           StandardButtons buttons = Ok | Cancel);

    static int question(QWidget *parent, const QString &title, const QString &text);
    static void information(QWidget *parent, const QString &title, const QString &text);
    static void warning(QWidget *parent, const QString &title, const QString &text);

    void cycleFocus();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    bool handleNavKey(int key);
};

#endif
