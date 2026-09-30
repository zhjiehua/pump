#include "ui/widgets/tablecelleditor.h"
#include "utils/hmikeys.h"

#include <QEvent>
#include <QKeyEvent>
#include <QShowEvent>
#include <cmath>

TableCellEditor::TableCellEditor(QWidget *parent, const QString &inputMask)
    : QLineEdit(parent)
    , m_inputMask(inputMask)
{
    QString temp = m_inputMask;
    temp.remove(QLatin1Char('.'));
    m_len = temp.length();
    const int dotIndex = m_inputMask.indexOf(QLatin1Char('.'));
    if (dotIndex == -1)
        m_scale = 1;
    else
        m_scale = static_cast<int>(std::pow(10.0, m_len - dotIndex));

    setFrame(true);
    setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    setStyleSheet(QStringLiteral(
        "QLineEdit{"
        "background-color:white;"
        "color:black;"
        "border:2px solid blue;"
        "outline:0;"
        "}"));
}

void TableCellEditor::showEvent(QShowEvent *event)
{
    QLineEdit::showEvent(event);
    selectAll();
    setFocus(Qt::OtherFocusReason);
}

bool TableCellEditor::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride)
    {
        const int key = static_cast<QKeyEvent *>(event)->key();
        if (key == KEY_BACKSPACE || key == Qt::Key_Escape
            || key == KEY_RETURN || key == Qt::Key_Enter)
        {
            event->accept();
            return true;
        }
    }
    return QLineEdit::event(event);
}

void TableCellEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9)
    {
        if (hasSelectedText())
            m_digits.clear();
        if (m_digits.length() == m_len)
            m_digits.clear();
        m_digits.append(event->text());
        const QString joined = m_digits.join(QString());
        const double ret = joined.toDouble() / static_cast<double>(m_scale);
        setText(QString::number(ret));
        return;
    }
    QLineEdit::keyPressEvent(event);
}
