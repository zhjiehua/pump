#include "ui/widgets/editctrl.h"
#include "utils/hmikeys.h"

#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>

EditCtrl::EditCtrl(QWidget *parent)
    : QLineEdit(parent)
{
    setReadOnly(true);
    setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    setStyleSheet(QStringLiteral(
        "QLineEdit:focus{border:2px solid blue;outline:0;}"
        "QLineEdit:disabled{background:#e0e0e0;color:#707070;}"));
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
    if (m_textMode)
    {
        m_digitScale = 1;
        m_digitLen = 32;
        return;
    }

    m_digitScale = 1;
    for (int i = 0; i < m_decimals; ++i)
        m_digitScale *= 10;

    const double bound = qMax(qAbs(m_min), qAbs(m_max));
    QString temp = QString::number(bound, 'f', m_decimals);
    temp.remove(QLatin1Char('.'));
    temp.remove(QLatin1Char('-'));
    m_digitLen = temp.length();
    if (m_digitLen < 1)
        m_digitLen = 1;
}

void EditCtrl::loadDigitsFromText()
{
    m_digits.clear();
    const QString t = text();
    if (m_textMode)
    {
        for (int i = 0; i < t.length(); ++i)
            m_digits.append(t.at(i));
        return;
    }
    for (int i = 0; i < t.length(); ++i)
    {
        if (t.at(i).isDigit())
            m_digits.append(t.at(i));
    }
}

void EditCtrl::startEditing()
{
    if (m_editing || !isReadOnly())
        return;
    m_saved = text();
    loadDigitsFromText();
    m_editing = true;
    selectAll();
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
    setText(QString::number(scaled, 'f', m_decimals));
}

bool EditCtrl::event(QEvent *event)
{
    if (m_editing && event->type() == QEvent::ShortcutOverride)
    {
        const int key = static_cast<QKeyEvent *>(event)->key();
        if ((key >= Qt::Key_0 && key <= Qt::Key_9)
            || (m_textMode && key >= Qt::Key_A && key <= Qt::Key_Z)
            || (m_textMode && (key == Qt::Key_Period || key == Qt::Key_Colon
                               || key == Qt::Key_X))
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
        if (hasSelectedText())
        {
            clearDigitsForNewEntry();
            return;
        }
        if (!m_digits.isEmpty())
            m_digits.removeLast();
        syncDisplayFromDigits();
        return;
    }

    if (m_textMode)
    {
        const QString t = event->text();
        if (!t.isEmpty() && t.at(0).isPrint())
        {
            if (hasSelectedText())
                clearDigitsForNewEntry();
            if (m_digits.length() >= m_digitLen)
                m_digits.clear();
            m_digits.append(t);
            syncDisplayFromDigits();
            return;
        }
        event->ignore();
        return;
    }

    if (key >= Qt::Key_0 && key <= Qt::Key_9)
    {
        if (hasSelectedText())
            clearDigitsForNewEntry();

        if (m_digits.length() >= m_digitLen)
            m_digits.clear();
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
