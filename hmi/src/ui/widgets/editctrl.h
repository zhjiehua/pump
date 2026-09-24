#ifndef EDITCTRL_H
#define EDITCTRL_H

#include <QLineEdit>

/** Panel numeric editor: Enter to edit, digit keys to type (weiduodianzi style). */
class EditCtrl : public QLineEdit
{
    Q_OBJECT
public:
    explicit EditCtrl(QWidget *parent = nullptr);

    void setValRange(double minVal, double maxVal, quint8 decimals = 0, bool textMode = false);

    bool isEditing() const { return m_editing; }
    bool cancelEditing();
    void startEditing();

signals:
    void valueCommitted(const QString &value);
    void editingChanged(bool editing);

protected:
    bool event(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void commitEdit();
    bool validateAndFormat(const QString &val, QString *out) const;
    void rebuildDigitScale();
    void syncDisplayFromDigits();
    void clearDigitsForNewEntry();

    double m_min = 0;
    double m_max = 100;
    quint8 m_decimals = 0;
    bool m_textMode = false;
    bool m_editing = false;
    QString m_saved;
    int m_digitLen = 0;
    int m_digitScale = 1;
    QStringList m_digits;
};

#endif
