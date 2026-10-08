#ifndef KEYBOARDDIALOG_H
#define KEYBOARDDIALOG_H

#include <QDialog>

class QLineEdit;
class QShowEvent;

/** On-screen numeric keyboard (weiduodianzi KeyBoardDialog). */
class KeyboardDialog : public QDialog
{
    Q_OBJECT
public:
    explicit KeyboardDialog(QWidget *parent = nullptr);

    QString result() const { return m_result; }
    void setInitialText(const QString &text);

    /** Modal numeric entry; returns false if cancelled or empty. */
    static bool prompt(QWidget *parent, const QString &initial, QString *out);

private slots:
    void onKeyClicked();
    void onPlus();
    void onMinus();
    void onOk();

protected:
    void showEvent(QShowEvent *event) override;

private:
    static QWidget *hostWidget(QWidget *context);
    void placeWithinHost(QWidget *host);
    void applyCompactStyle();

    void appendChar(const QString &ch);
    void toggleSign(bool positive);

    QLineEdit *m_edit = nullptr;
    QString m_result;
};

#endif
