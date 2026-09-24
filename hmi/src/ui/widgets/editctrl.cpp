#include "ui/widgets/editctrl.h"
#include "utils/hmikeys.h"

#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>

#include <cmath>

EditCtrl::EditCtrl(QWidget *parent)
    : QLineEdit(parent)
{
    setReadOnly(true);
    setAlignment(Qt::AlignCenter);
    setStyleSheet(QStringLiteral("QLineEdit:focus{border:2px solid blue;outline:0;}"));
}

void EditCtrl::setValRange(double minVal, double maxVal, quint8 decimals, bool textMode)
{
    m_min = minVal;
    m_max = maxVal;
    m_decimals = decimals;
    m_textMode = textMode;
    rebuildDigitScale();
}

void EditCtrl::rebuildDigitScale()
{
    QString temp = QString::number(m_max, 'f', m_decimals);
    const int dotIndex = temp.indexOf(QLatin1Char('.'));
    temp.remove(QLatin1Char('.'));
    m_digitLen = temp.length();
    if (dotIndex == -1)
        m_digitScale = 1;
    else
        m_digitScale = static_cast<int>(std::pow(10.0, m_digitLen - dotIndex));
}

void EditCtrl::startEditing()
{
    if (m_editing || !isReadOnly())
        return;
    m_saved = text();
    m_digits.clear();
    const QString t = text();
    for (int i = 0; i < t.length(); ++i)
        m_digits.append(t.at(i));
    m_editing = true;
    setReadOnly(false);
    selectAll();
    emit editingChanged(true);
}

bool EditCtrl::cancelEditing()
{
    if (!m_editing)
        return false;
    setText(m_saved);
    setReadOnly(true);
    m_editing = false;
    m_digits.clear();
    emit editingChanged(false);
    return true;
}

void EditCtrl::clearDigitsForNewEntry()
{
    m_digits.clear();
    clear();
}

void EditCtrl::syncDisplayFromDigits()
{
    const QString joined = m_digits.join(QString());
    if (m_textMode)
    {
        setText(joined);
        return;
    }
    const double scaled = joined.toDouble() / static_cast<double>(m_digitScale);
    setText(QString::number(scaled));
}

bool EditCtrl::event(QEvent *event)
{
    if (m_editing && event->type() == QEvent::ShortcutOverride)
    {
        const int key = static_cast<QKeyEvent *>(event)->key();
        if ((key >= Qt::Key_0 && key <= Qt::Key_9)
            || key == KEY_LEFT || key == PANEL_KEY_LEFT
            || key == KEY_RETURN || key == Qt::Key_Enter
            || key == KEY_BACKSPACE || key == Qt::Key_Escape)
        {
            event->accept();
            return true;
        }
    }
    return QLineEdit::event(event);
}

void EditCtrl::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();

    if (key == KEY_RETURN || key == Qt::Key_Enter)
    {
        if (isReadOnly())
            startEditing();
        else
            commitEdit();
        return;
    }

    if (key == KEY_BACKSPACE || key == Qt::Key_Escape)
    {
        if (cancelEditing())
            return;
    }

    if (!m_editing || isReadOnly())
        return;

    if (key == KEY_LEFT || key == PANEL_KEY_LEFT)
    {
        QString temp;
        if (hasSelectedText())
        {
            clearDigitsForNewEntry();
            temp.clear();
        }
        else
        {
            temp = text();
            if (!temp.isEmpty())
                temp.chop(1);
            if (!m_digits.isEmpty())
                m_digits.removeLast();
        }
        setText(temp);
        return;
    }

    if (key >= Qt::Key_0 && key <= Qt::Key_9)
    {
        if (hasSelectedText())
            clearDigitsForNewEntry();

        if (m_digits.length() == m_digitLen)
            m_digits.clear();
        else
            m_digits.append(event->text());

        syncDisplayFromDigits();
        return;
    }

    event->ignore();
}

void EditCtrl::mousePressEvent(QMouseEvent *event)
{
    QLineEdit::mousePressEvent(event);
}

void EditCtrl::commitEdit()
{
    m_editing = false;
    m_digits.clear();
    QString formatted;
    if (m_textMode)
    {
        setReadOnly(true);
        emit editingChanged(false);
        emit valueCommitted(text());
        return;
    }
    if (!validateAndFormat(text(), &formatted))
    {
        setText(m_saved);
        setReadOnly(true);
        emit editingChanged(false);
        return;
    }
    setText(formatted);
    setReadOnly(true);
    emit editingChanged(false);
    emit valueCommitted(text());
}

bool EditCtrl::validateAndFormat(const QString &val, QString *out) const
{
    bool ok = false;
    const double d = val.toDouble(&ok);
    if (!ok || d < m_min || d > m_max)
        return false;
    *out = QString::number(d, 'f', m_decimals);
    return true;
}
