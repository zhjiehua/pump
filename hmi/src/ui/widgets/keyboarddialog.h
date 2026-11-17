#ifndef KEYBOARDDIALOG_H
#define KEYBOARDDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;
class QResizeEvent;
class QShowEvent;

/** On-screen numeric keyboard (touch). Equal-size 4x4 key grid. */
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
    void onDigitOrDot();
    void onClear();
    void onBackspace();
    void onToggleSign();
    void onOk();

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    enum KeyRole { Digit, Func, Ok, Cancel };

    static QWidget *hostWidget(QWidget *context);
    void placeWithinHost(QWidget *host);
    void applyStyle();
    void syncKeyMetrics();

    QPushButton *makeKey(const QString &text, KeyRole role);
    void appendChar(const QString &ch);

    QLineEdit *m_edit = nullptr;
    QString m_result;
};

#endif
