#include "ui/widgets/tablecelleditor.h"

#include <QKeyEvent>
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
}

void TableCellEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9)
    {
        if (m_digits.length() == m_len)
            m_digits.clear();
        m_digits.append(event->text());
        const QString joined = m_digits.join(QString());
        const double ret = joined.toDouble() / static_cast<double>(m_scale);
        setText(QString::number(ret));
        return;
    }
    if (event->key() == Qt::Key_Backspace)
        return;
    QLineEdit::keyPressEvent(event);
}
