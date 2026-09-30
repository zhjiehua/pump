#ifndef TABLECELLEDITOR_H
#define TABLECELLEDITOR_H

#include <QLineEdit>

/** Masked numeric cell editor (legacy TableEditor). */
class TableCellEditor : public QLineEdit
{
    Q_OBJECT
public:
    explicit TableCellEditor(QWidget *parent = nullptr, const QString &inputMask = QString());

protected:
    bool event(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QString m_inputMask;
    int m_len = 0;
    int m_scale = 1;
    QStringList m_digits;
};

#endif
