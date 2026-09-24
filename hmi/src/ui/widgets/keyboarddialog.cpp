#include "ui/widgets/keyboarddialog.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

KeyboardDialog::KeyboardDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setModal(true);

    auto *root = new QVBoxLayout(this);
    m_edit = new QLineEdit;
    m_edit->setAlignment(Qt::AlignRight);
    root->addWidget(m_edit);

    auto *grid = new QGridLayout;
    const char *keys[] = {"7", "8", "9", "4", "5", "6", "1", "2", "3", "0", ".", "CE"};
    int idx = 0;
    for (int r = 0; r < 4; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            auto *b = new QPushButton(QString::fromLatin1(keys[idx++]));
            connect(b, SIGNAL(clicked()), this, SLOT(onKeyClicked()));
            grid->addWidget(b, r, c);
        }
    }
    root->addLayout(grid);

    auto *signRow = new QHBoxLayout;
    auto *plus = new QPushButton(QStringLiteral("+"));
    auto *minus = new QPushButton(QStringLiteral("-"));
    connect(plus, SIGNAL(clicked()), this, SLOT(onPlus()));
    connect(minus, SIGNAL(clicked()), this, SLOT(onMinus()));
    signRow->addWidget(plus);
    signRow->addWidget(minus);
    root->addLayout(signRow);

    auto *actions = new QHBoxLayout;
    auto *ok = new QPushButton(tr("OK"));
    auto *cancel = new QPushButton(tr("Cancel"));
    connect(ok, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
    actions->addWidget(ok);
    actions->addWidget(cancel);
    root->addLayout(actions);
}

void KeyboardDialog::setInitialText(const QString &text)
{
    m_edit->setText(text);
}

void KeyboardDialog::onKeyClicked()
{
    auto *b = qobject_cast<QPushButton *>(sender());
    if (!b)
        return;
    if (b->text() == QStringLiteral("CE"))
        m_edit->clear();
    else
        appendChar(b->text());
}

void KeyboardDialog::onPlus()
{
    toggleSign(true);
}

void KeyboardDialog::onMinus()
{
    toggleSign(false);
}

void KeyboardDialog::onOk()
{
    m_result = m_edit->text();
    if (m_result.isEmpty())
        reject();
    else
        accept();
}

void KeyboardDialog::appendChar(const QString &ch)
{
    if (ch == QStringLiteral("."))
    {
        if (m_edit->text().contains(QLatin1Char('.')))
            return;
    }
    m_edit->insert(ch);
}

void KeyboardDialog::toggleSign(bool positive)
{
    QString text = m_edit->text();
    if (text.isEmpty())
    {
        m_edit->setText(positive ? QStringLiteral("+") : QStringLiteral("-"));
        return;
    }
    if (positive)
    {
        if (text.startsWith(QLatin1Char('-')))
            m_edit->setText(text.mid(1));
    }
    else if (!text.startsWith(QLatin1Char('-')))
    {
        m_edit->setText(QLatin1Char('-') + text);
    }
}
