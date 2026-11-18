#include "ui/widgets/keyboarddialog.h"

#include <QFont>
#include <QGridLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QResizeEvent>
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

QPushButton *KeyboardDialog::makeKey(const QString &text, KeyRole role)
{
    auto *b = new QPushButton(text);
    b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // Ignore text-driven preferred size so every cell stays equal.
    b->setMinimumSize(0, 0);
    b->setFocusPolicy(Qt::NoFocus);
    b->setAutoDefault(false);
    b->setDefault(false);
    switch (role)
    {
    case Digit:
        b->setObjectName(QStringLiteral("kbdDigit"));
        break;
    case Func:
        b->setObjectName(QStringLiteral("kbdFunc"));
        break;
    case Ok:
        b->setObjectName(QStringLiteral("kbdOk"));
        break;
    case Cancel:
        b->setObjectName(QStringLiteral("kbdCancel"));
        break;
    }
    return b;
}

void KeyboardDialog::applyStyle()
{
    setStyleSheet(QStringLiteral(
        "KeyboardDialog{background-color:#1e2936;}"
        "QLineEdit#kbdDisplay{"
        "  background-color:#0f1720;color:#f8fafc;"
        "  border:1px solid #334155;border-radius:4px;"
        "  padding:0 8px;min-height:0;"
        "  selection-background-color:#2563eb;selection-color:#ffffff;}"
        "QPushButton{"
        "  min-width:0;min-height:0;padding:0;margin:0;"
        "  border:1px solid #334155;border-radius:4px;"
        "  color:#f8fafc;background-color:#334155;}"
        "QPushButton:pressed{background-color:#475569;}"
        "QPushButton#kbdFunc{background-color:#475569;}"
        "QPushButton#kbdFunc:pressed{background-color:#64748b;}"
        "QPushButton#kbdOk{background-color:#2563eb;border-color:#1d4ed8;}"
        "QPushButton#kbdOk:pressed{background-color:#1d4ed8;}"
        "QPushButton#kbdCancel{background-color:#7f1d1d;border-color:#991b1b;}"
        "QPushButton#kbdCancel:pressed{background-color:#991b1b;}"));
}

void KeyboardDialog::syncKeyMetrics()
{
    // Font scales with the shorter panel side so 1x/2x/3x stay readable.
    const int side = qMin(width(), height());
    const int px = qBound(12, side / 14, 28);
    QFont f = font();
    f.setPixelSize(px);
    f.setBold(true);
    const auto buttons = findChildren<QPushButton *>();
    for (QPushButton *b : buttons)
        b->setFont(f);

    QFont editFont = f;
    editFont.setPixelSize(qBound(14, side / 12, 32));
    m_edit->setFont(editFont);
}

void KeyboardDialog::placeWithinHost(QWidget *host)
{
    if (!host)
        return;
    setGeometry(QRect(QPoint(0, 0), host->size()));
    syncKeyMetrics();
}

void KeyboardDialog::showEvent(QShowEvent *event)
{
    if (QWidget *host = parentWidget())
        setGeometry(QRect(QPoint(0, 0), host->size()));
    syncKeyMetrics();
    QDialog::showEvent(event);
    // Select after show so replace/BS act on the whole initial value.
    m_edit->selectAll();
}

void KeyboardDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    syncKeyMetrics();
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
    applyStyle();

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(6, 6, 6, 6);
    root->setSpacing(6);

    m_edit = new QLineEdit;
    m_edit->setObjectName(QStringLiteral("kbdDisplay"));
    m_edit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_edit->setReadOnly(true);
    m_edit->setFocusPolicy(Qt::NoFocus);
    m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_edit->setMinimumSize(0, 0);
    root->addWidget(m_edit, 1);

    auto *grid = new QGridLayout;
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(4);
    grid->setVerticalSpacing(4);
    for (int i = 0; i < 4; ++i)
    {
        grid->setColumnStretch(i, 1);
        grid->setRowStretch(i, 1);
    }

    // Equal 4x4 pad — every key same cell size.
    //  7  8  9  BS
    //  4  5  6  CE
    //  1  2  3  +/-
    //  0  .  OK Esc
    QPushButton *b7 = makeKey(QStringLiteral("7"), Digit);
    QPushButton *b8 = makeKey(QStringLiteral("8"), Digit);
    QPushButton *b9 = makeKey(QStringLiteral("9"), Digit);
    QPushButton *bs = makeKey(QStringLiteral("BS"), Func);
    QPushButton *b4 = makeKey(QStringLiteral("4"), Digit);
    QPushButton *b5 = makeKey(QStringLiteral("5"), Digit);
    QPushButton *b6 = makeKey(QStringLiteral("6"), Digit);
    QPushButton *ce = makeKey(QStringLiteral("CE"), Func);
    QPushButton *b1 = makeKey(QStringLiteral("1"), Digit);
    QPushButton *b2 = makeKey(QStringLiteral("2"), Digit);
    QPushButton *b3 = makeKey(QStringLiteral("3"), Digit);
    QPushButton *sign = makeKey(QStringLiteral("+/-"), Func);
    QPushButton *b0 = makeKey(QStringLiteral("0"), Digit);
    QPushButton *dot = makeKey(QStringLiteral("."), Digit);
    QPushButton *ok = makeKey(QStringLiteral("OK"), Ok);
    QPushButton *esc = makeKey(QStringLiteral("Esc"), Cancel);

    grid->addWidget(b7, 0, 0);
    grid->addWidget(b8, 0, 1);
    grid->addWidget(b9, 0, 2);
    grid->addWidget(bs, 0, 3);
    grid->addWidget(b4, 1, 0);
    grid->addWidget(b5, 1, 1);
    grid->addWidget(b6, 1, 2);
    grid->addWidget(ce, 1, 3);
    grid->addWidget(b1, 2, 0);
    grid->addWidget(b2, 2, 1);
    grid->addWidget(b3, 2, 2);
    grid->addWidget(sign, 2, 3);
    grid->addWidget(b0, 3, 0);
    grid->addWidget(dot, 3, 1);
    grid->addWidget(ok, 3, 2);
    grid->addWidget(esc, 3, 3);

    connect(b7, SIGNAL(clicked()), this, SLOT(onDigitOrDot()));
    connect(b8, SIGNAL(clicked()), this, SLOT(onDigitOrDot()));
    connect(b9, SIGNAL(clicked()), this, SLOT(onDigitOrDot()));
    connect(b4, SIGNAL(clicked()), this, SLOT(onDigitOrDot()));
    connect(b5, SIGNAL(clicked()), this, SLOT(onDigitOrDot()));
    connect(b6, SIGNAL(clicked()), this, SLOT(onDigitOrDot()));
    connect(b1, SIGNAL(clicked()), this, SLOT(onDigitOrDot()));
    connect(b2, SIGNAL(clicked()), this, SLOT(onDigitOrDot()));
    connect(b3, SIGNAL(clicked()), this, SLOT(onDigitOrDot()));
    connect(b0, SIGNAL(clicked()), this, SLOT(onDigitOrDot()));
    connect(dot, SIGNAL(clicked()), this, SLOT(onDigitOrDot()));
    connect(bs, SIGNAL(clicked()), this, SLOT(onBackspace()));
    connect(ce, SIGNAL(clicked()), this, SLOT(onClear()));
    connect(sign, SIGNAL(clicked()), this, SLOT(onToggleSign()));
    connect(ok, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(esc, SIGNAL(clicked()), this, SLOT(reject()));

    root->addLayout(grid, 5);
}

void KeyboardDialog::setInitialText(const QString &text)
{
    m_edit->setText(text);
    m_edit->selectAll();
}

void KeyboardDialog::onDigitOrDot()
{
    auto *b = qobject_cast<QPushButton *>(sender());
    if (!b)
        return;
    appendChar(b->text());
}

void KeyboardDialog::onClear()
{
    m_edit->clear();
}

void KeyboardDialog::onBackspace()
{
    if (m_edit->hasSelectedText())
    {
        const int start = m_edit->selectionStart();
        QString t = m_edit->text();
        t.remove(start, m_edit->selectedText().size());
        m_edit->setText(t);
        m_edit->setCursorPosition(start);
        return;
    }
    const int pos = m_edit->cursorPosition();
    if (pos <= 0)
        return;
    QString t = m_edit->text();
    t.remove(pos - 1, 1);
    m_edit->setText(t);
    m_edit->setCursorPosition(pos - 1);
}

void KeyboardDialog::onToggleSign()
{
    QString text = m_edit->text();
    if (text.isEmpty())
    {
        m_edit->setText(QStringLiteral("-"));
        return;
    }
    if (text.startsWith(QLatin1Char('-')))
        m_edit->setText(text.mid(1));
    else if (text.startsWith(QLatin1Char('+')))
        m_edit->setText(QLatin1Char('-') + text.mid(1));
    else
        m_edit->setText(QLatin1Char('-') + text);
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
    QString t = m_edit->text();
    int start = m_edit->cursorPosition();
    int len = 0;
    if (m_edit->hasSelectedText())
    {
        start = m_edit->selectionStart();
        len = m_edit->selectedText().size();
    }
    const QString after = t.left(start) + ch + t.mid(start + len);
    if (ch == QStringLiteral(".") && after.count(QLatin1Char('.')) > 1)
        return;
    m_edit->setText(after);
    m_edit->setCursorPosition(start + ch.size());
}
