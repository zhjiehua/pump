#include "ui/widgets/keyboarddialog.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QShowEvent>
#include <QVBoxLayout>

namespace {

QWidget *productPanelHost(QWidget *context)
{
    QWidget *w = context;
    while (w)
    {
        if (w->objectName() == QStringLiteral("productPanel"))
            return w;
        if (auto *mw = qobject_cast<QMainWindow *>(w))
        {
            if (QWidget *cw = mw->centralWidget())
                return cw;
        }
        w = w->parentWidget();
    }
    return nullptr;
}

} // namespace

QWidget *KeyboardDialog::hostWidget(QWidget *context)
{
    if (QWidget *panel = productPanelHost(context))
        return panel;
    QWidget *w = context;
    while (w && !w->isWindow())
        w = w->parentWidget();
    return w ? w : context;
}

void KeyboardDialog::applyCompactStyle()
{
    setStyleSheet(QStringLiteral(
        "KeyboardDialog{background-color:white;}"
        "QPushButton{min-height:20px;padding:0 2px;}"
        "QLineEdit{min-height:20px;padding:0 2px;}"));
    if (QLayout *lay = layout())
        lay->setContentsMargins(2, 2, 2, 2);
}

void KeyboardDialog::placeWithinHost(QWidget *host)
{
    if (!host)
        return;

    applyCompactStyle();
    setGeometry(QRect(QPoint(0, 0), host->size()));
}

void KeyboardDialog::showEvent(QShowEvent *event)
{
    if (QWidget *host = parentWidget())
        setGeometry(QRect(QPoint(0, 0), host->size()));
    QDialog::showEvent(event);
}

bool KeyboardDialog::prompt(QWidget *parent, const QString &initial, QString *out)
{
    if (!out)
        return false;
    QWidget *host = hostWidget(parent);
    if (!host)
        return false;
    KeyboardDialog dlg(host);
    dlg.setInitialText(initial);
    dlg.placeWithinHost(host);
    dlg.raise();
    if (dlg.exec() != QDialog::Accepted)
        return false;
    *out = dlg.result();
    return !out->isEmpty();
}

KeyboardDialog::KeyboardDialog(QWidget *parent)
    : QDialog(parent)
{
    // Child overlay on productPanel — not Qt::Dialog (separate window / screen coords).
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setModal(true);
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoFillBackground(true);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(2, 2, 2, 2);
    root->setSpacing(2);
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
            b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            connect(b, SIGNAL(clicked()), this, SLOT(onKeyClicked()));
            grid->addWidget(b, r, c);
        }
    }
    root->addLayout(grid, 1);

    auto *signRow = new QHBoxLayout;
    auto *plus = new QPushButton(QStringLiteral("+"));
    auto *minus = new QPushButton(QStringLiteral("-"));
    plus->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    minus->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(plus, SIGNAL(clicked()), this, SLOT(onPlus()));
    connect(minus, SIGNAL(clicked()), this, SLOT(onMinus()));
    signRow->addWidget(plus);
    signRow->addWidget(minus);
    root->addLayout(signRow);

    auto *actions = new QHBoxLayout;
    auto *ok = new QPushButton(tr("OK"));
    auto *cancel = new QPushButton(tr("Cancel"));
    ok->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cancel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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
