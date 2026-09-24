#ifndef KEYBOARDDIALOG_H
#define KEYBOARDDIALOG_H

#include <QDialog>

class QLineEdit;

/** On-screen numeric keyboard (weiduodianzi KeyBoardDialog). */
class KeyboardDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KeyboardDialog(QWidget *parent = nullptr);

    QString result() const { return m_result; }
    void setInitialText(const QString &text);

private slots:
    void onKeyClicked();
    void onPlus();
    void onMinus();
    void onOk();

private:
    void appendChar(const QString &ch);
    void toggleSign(bool positive);

    QLineEdit *m_edit = nullptr;
    QString m_result;
};

#endif
