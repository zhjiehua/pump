#include "ui/widgets/hmitablewidget.h"
#include "ui/mainwindow.h"
#include "ui/widgets/btnctrl.h"
#include "utils/hmikeys.h"

#include <QAbstractItemDelegate>
#include <QAbstractItemView>
#include <QAbstractScrollArea>
#include <QApplication>
#include <QColor>
#include <QCoreApplication>
#include <QEvent>
#include <QFocusEvent>
#include <QFrame>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

namespace {

class TableFocusBorder : public QWidget
{
public:
    explicit TableFocusBorder(QWidget *parent)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);
        setAutoFillBackground(false);
        setFocusPolicy(Qt::NoFocus);
    }

    void setBorder(const QColor &color, int width)
    {
        m_color = color;
        m_width = qMax(1, width);
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setPen(Qt::NoPen);
        p.setBrush(m_color);
        const QRect r = rect();
        const int w = m_width;
        p.drawRect(r.x(), r.y(), r.width(), w);
        p.drawRect(r.x(), r.bottom() - w + 1, r.width(), w);
        p.drawRect(r.x(), r.y(), w, r.height());
        p.drawRect(r.right() - w + 1, r.y(), w, r.height());
    }

private:
    QColor m_color = QColor(0xa0, 0xa0, 0xa0);
    int m_width = 1;
};

class HeaderHashHighlight : public QWidget
{
public:
    explicit HeaderHashHighlight(QWidget *parent)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);
        setAutoFillBackground(false);
        setFocusPolicy(Qt::NoFocus);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(155, 200, 33));
        p.drawRect(rect());
        p.setPen(Qt::black);
        p.drawText(rect(), Qt::AlignCenter, QStringLiteral("#"));
    }
};

bool isNavKey(int key)
{
    return key == KEY_UP || key == Qt::Key_Up
        || key == KEY_DOWN || key == Qt::Key_Down
        || key == KEY_LEFT || key == Qt::Key_Left
        || key == KEY_RIGHT || key == Qt::Key_Right;
}

bool isEnterKey(int key)
{
    return key == KEY_RETURN || key == Qt::Key_Enter;
}

bool isBackKey(int key)
{
    return key == KEY_BACKSPACE || key == Qt::Key_Escape;
}

const char *kOverlayBtnStyle =
    "QPushButton{outline:0;min-width:88px;}"
    "QPushButton:focus{border:2px solid blue;outline:0;}";

} // namespace

HmiTableWidget::HmiTableWidget(QWidget *parent)
    : QTableWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setTabKeyNavigation(false);
    verticalHeader()->setVisible(false);
    horizontalHeader()->setStretchLastSection(true);
    setFrameStyle(QFrame::NoFrame);
    setLineWidth(0);
    setMidLineWidth(0);
    setAutoFillBackground(true);
    setProperty("inside", false);
    setStyleSheet(QStringLiteral(
        "QTableWidget{outline:0;border:0;}"
        "QTableWidget:focus{outline:0;"
        "selection-background-color:rgb(160,200,255);selection-color:black;}"
        "QTableWidget[inside=\"true\"]:focus{outline:0;"
        "selection-background-color:rgb(155,200,33);selection-color:black;}"));
    m_focusBorder = new TableFocusBorder(this);
    updateFocusBorder();
    horizontalHeader()->setSectionsClickable(true);
    horizontalHeader()->viewport()->installEventFilter(this);
}

HmiTableWidget *HmiTableWidget::owningTable(QWidget *w)
{
    while (w)
    {
        if (auto *t = qobject_cast<HmiTableWidget *>(w))
            return t;
        w = w->parentWidget();
    }
    return nullptr;
}

bool HmiTableWidget::isIndexMenuOpen() const
{
    return (m_indexMenu && m_indexMenu->isVisible())
        || (m_confirmMenu && m_confirmMenu->isVisible());
}

void HmiTableWidget::initIndex()
{
    if (rowCount() > 0 && columnCount() > 0)
    {
        m_onHashHeader = false;
        m_currentIndex = model()->index(0, 0);
    }
    else
    {
        m_onHashHeader = true;
        m_currentIndex = QModelIndex();
    }
}

int HmiTableWidget::dataColumnCount() const
{
    return qMax(0, columnCount() - 1);
}

void HmiTableWidget::setDataColumnCount(int n)
{
    const bool blocked = blockSignals(true);
    QTableWidget::setColumnCount(qMax(0, n) + 1);
    if (columnCount() > 0)
    {
        horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        setColumnWidth(0, 36);
    }
    blockSignals(blocked);
    setDataHeaders(QStringList());
}

void HmiTableWidget::setDataHeaders(const QStringList &labels)
{
    QStringList headers;
    headers << tr("#");
    if (!labels.isEmpty())
        headers << labels;
    else
    {
        for (int i = 0; i < dataColumnCount(); ++i)
            headers << QString();
    }
    setHorizontalHeaderLabels(headers);
}

void HmiTableWidget::setDataDelegate(int dataCol, QAbstractItemDelegate *delegate)
{
    setItemDelegateForColumn(dataColToView(dataCol), delegate);
}

void HmiTableWidget::setDataText(int row, int dataCol, const QString &text)
{
    if (row < 0 || row >= rowCount())
        return;
    const int col = dataColToView(dataCol);
    if (col <= 0 || col >= columnCount())
        return;
    const bool blocked = blockSignals(true);
    QTableWidgetItem *it = item(row, col);
    if (!it)
    {
        it = new QTableWidgetItem;
        setItem(row, col, it);
    }
    it->setText(text);
    blockSignals(blocked);
}

QString HmiTableWidget::dataText(int row, int dataCol) const
{
    const QTableWidgetItem *it = item(row, dataColToView(dataCol));
    return it ? it->text() : QString();
}

bool HmiTableWidget::isDataRowEmpty(int row) const
{
    if (row < 0 || row >= rowCount())
        return true;
    for (int c = 1; c < columnCount(); ++c)
    {
        const QTableWidgetItem *it = item(row, c);
        if (it && !it->text().trimmed().isEmpty())
            return false;
    }
    return true;
}

void HmiTableWidget::setIndexItem(int row)
{
    if (row < 0 || row >= rowCount() || columnCount() <= 0)
        return;
    QTableWidgetItem *it = item(row, 0);
    if (!it)
    {
        it = new QTableWidgetItem;
        it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        it->setTextAlignment(Qt::AlignCenter);
        it->setBackground(QColor(240, 240, 240));
        setItem(row, 0, it);
    }
    it->setText(QString::number(row + 1));
}

void HmiTableWidget::fillEmptyRow(int row)
{
    setIndexItem(row);
    for (int c = 1; c < columnCount(); ++c)
    {
        QTableWidgetItem *it = item(row, c);
        if (!it)
        {
            it = new QTableWidgetItem;
            setItem(row, c, it);
        }
        else
            it->setText(QString());
    }
}

void HmiTableWidget::refreshIndexColumn()
{
    const bool blocked = blockSignals(true);
    for (int r = 0; r < rowCount(); ++r)
        setIndexItem(r);
    blockSignals(blocked);
}

void HmiTableWidget::setFilledRowTexts(const QVector<QStringList> &rows)
{
    const bool blocked = blockSignals(true);
    setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r)
    {
        setIndexItem(r);
        for (int c = 0; c < dataColumnCount(); ++c)
        {
            const QString text = c < rows.at(r).size() ? rows.at(r).at(c) : QString();
            QTableWidgetItem *it = item(r, c + 1);
            if (!it)
            {
                it = new QTableWidgetItem;
                setItem(r, c + 1, it);
            }
            it->setText(text);
        }
    }
    blockSignals(blocked);
    refreshIndexColumn();
    if (!m_inside)
    {
        if (rowCount() <= 0)
        {
            m_onHashHeader = true;
            m_currentIndex = QModelIndex();
        }
        else
            m_onHashHeader = false;
    }
}

void HmiTableWidget::setEditAuthPage(MainWindow *main, int page)
{
    m_authMain = main;
    m_authPage = page;
}

void HmiTableWidget::refreshEditAuth()
{
    if (!m_authMain || m_authPage < 0)
    {
        m_editAuthRequired = false;
        return;
    }
    m_editAuthRequired = !m_authMain->consumeLoginOkFor(MainWindow::Page(m_authPage));
    if (m_resumeInner)
        QTimer::singleShot(0, this, SLOT(resumeInnerNow()));
}

bool HmiTableWidget::gateEditAuth()
{
    if (!m_editAuthRequired)
        return false;
    m_resumeInner = true;
    if (currentIndex().isValid())
        m_currentIndex = currentIndex();
    if (m_authMain && m_authPage >= 0)
        m_authMain->requestPasswordThen(MainWindow::Page(m_authPage), false);
    else
        emit editAuthRequested();
    return true;
}

void HmiTableWidget::resumeInnerNow()
{
    if (!m_resumeInner)
        return;
    m_inside = true;
    m_skipCellActivate = false;
    if (!hasFocus())
        setFocus(Qt::OtherFocusReason);
    restoreInnerCell();
    emit panelShortcutsEnabled(false);
    updateFocusBorder();
}

void HmiTableWidget::insertEmptyRowAfter(int row)
{
    if (columnCount() <= 0)
        return;
    int at = row + 1;
    if (at < 0)
        at = 0;
    if (at > rowCount())
        at = rowCount();
    const bool blocked = blockSignals(true);
    insertRow(at);
    fillEmptyRow(at);
    blockSignals(blocked);
    refreshIndexColumn();
    if (!m_onHashHeader)
    {
        const int stay = qBound(0, row < 0 ? 0 : row, qMax(0, rowCount() - 1));
        if (rowCount() > 0)
            m_currentIndex = model()->index(stay, 0);
        else
            m_onHashHeader = true;
    }
    if (m_inside)
        restoreInnerCell();
}

void HmiTableWidget::removeDataRow(int row)
{
    if (row < 0 || row >= rowCount())
        return;
    const bool blocked = blockSignals(true);
    removeRow(row);
    blockSignals(blocked);
    refreshIndexColumn();
    if (rowCount() <= 0)
    {
        m_onHashHeader = true;
        m_currentIndex = QModelIndex();
    }
    else if (!m_onHashHeader)
    {
        const int next = qBound(0, row, rowCount() - 1);
        m_currentIndex = model()->index(next, 0);
    }
    if (m_inside)
        restoreInnerCell();
}

void HmiTableWidget::addRowAfterCurrent()
{
    if (gateEditAuth())
        return;
    if (m_onHashHeader || rowCount() <= 0)
    {
        insertEmptyRowAfter(-1);
        return;
    }
    int row = currentRow();
    if (row < 0)
        row = qMax(0, rowCount() - 1);
    insertEmptyRowAfter(row);
}

void HmiTableWidget::commitActiveEditor()
{
    if (state() != QAbstractItemView::EditingState)
        return;
    QWidget *fw = QApplication::focusWidget();
    if (!fw || !isAncestorOf(fw))
        return;
    QKeyEvent enter(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    QCoreApplication::sendEvent(fw, &enter);
}

void HmiTableWidget::syncInsideStyle()
{
    setProperty("inside", m_inside);
    if (QStyle *s = style())
    {
        s->unpolish(this);
        s->polish(this);
    }
    updateFocusBorder();
    update();
}

void HmiTableWidget::updateFocusBorder()
{
    auto *border = static_cast<TableFocusBorder *>(m_focusBorder);
    if (!border)
        return;

    const QWidget *fw = QApplication::focusWidget();
    const bool focused = hasFocus() || (fw && (fw == this || isAncestorOf(fw)));
    QColor color(0xd8, 0xd8, 0xd8);
    int width = 1;
    if (focused && m_inside)
    {
        color = QColor(155, 200, 33);
        width = 2;
    }
    else if (focused)
    {
        color = QColor(Qt::blue);
        width = 2;
    }

    border->setBorder(color, width);
    border->setGeometry(rect());
    border->raise();
    border->show();
    updateHashHeaderHighlight();
}

void HmiTableWidget::applyOuterVisuals()
{
    syncInsideStyle();
    if (selectionModel())
        selectionModel()->clear();
    setCurrentIndex(QModelIndex());
    updateHashHeaderHighlight();
}

void HmiTableWidget::restoreInnerCell()
{
    if (m_onHashHeader || rowCount() <= 0)
    {
        selectHashHeader();
        syncInsideStyle();
        return;
    }
    if (model() && model()->rowCount() > 0 && model()->columnCount() > 0)
    {
        int row = m_currentIndex.isValid() ? m_currentIndex.row() : 0;
        int col = m_currentIndex.isValid() ? m_currentIndex.column() : 0;
        row = qBound(0, row, model()->rowCount() - 1);
        col = qBound(0, col, model()->columnCount() - 1);
        m_currentIndex = model()->index(row, col);
    }
    else
        m_currentIndex = QModelIndex();
    if (m_currentIndex.isValid())
    {
        setCurrentIndex(m_currentIndex);
        if (selectionModel())
            selectionModel()->select(m_currentIndex, QItemSelectionModel::ClearAndSelect);
    }
    updateHashHeaderHighlight();
    syncInsideStyle();
}

void HmiTableWidget::enterInner()
{
    if (m_inside)
        return;
    m_inside = true;
    // The Enter that opened cell-nav must not also open the editor/menu.
    m_skipCellActivate = true;
    restoreInnerCell();
    emit panelShortcutsEnabled(false);
}

void HmiTableWidget::activateCurrentCell()
{
    if (gateEditAuth())
        return;
    if (m_onHashHeader || rowCount() <= 0 || currentIndex().column() == 0)
        showIndexMenu();
    else if (currentIndex().isValid())
    {
        m_allowEdit = true;
        edit(currentIndex(), QAbstractItemView::AllEditTriggers, nullptr);
        m_allowEdit = false;
    }
}

void HmiTableWidget::exitInner()
{
    m_resumeInner = false;
    if (isIndexMenuOpen())
        hideIndexMenu();
    if (!m_inside)
        return;
    m_currentIndex = currentIndex();
    m_inside = false;
    applyOuterVisuals();
    emit panelShortcutsEnabled(true);
    if (!hasFocus())
        setFocus(Qt::OtherFocusReason);
}

bool HmiTableWidget::cancelCellEdit()
{
    if (state() != QAbstractItemView::EditingState)
        return false;
    QWidget *fw = QApplication::focusWidget();
    if (fw && isAncestorOf(fw))
    {
        QKeyEvent esc(QEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
        QCoreApplication::sendEvent(fw, &esc);
    }
    return true;
}

bool HmiTableWidget::handleBack()
{
    if (isIndexMenuOpen())
    {
        hideIndexMenu();
        return true;
    }
    if (cancelCellEdit())
        return true;
    if (m_inside)
    {
        exitInner();
        return true;
    }
    return false;
}

bool HmiTableWidget::handleInnerNavKey(int key)
{
    if (isIndexMenuOpen())
    {
        const bool up = key == KEY_UP || key == Qt::Key_Up;
        const bool down = key == KEY_DOWN || key == Qt::Key_Down;
        if (up || down)
            return moveIndexMenuFocus(down);
        return false;
    }
    if (!m_inside)
        return false;
    return consumeInnerKey(key);
}

void HmiTableWidget::placeOverlay(QWidget *overlay)
{
    if (!overlay)
        return;
    overlay->adjustSize();
    QPoint pos(0, 0);
    const QRect vp = viewport()->rect();
    if (m_menuRow < 0)
    {
        pos.setX(qMax(0, horizontalHeader()->sectionViewportPosition(0)));
    }
    else
    {
        if (!model() || rowCount() <= 0 || columnCount() <= 0)
            return;
        const int row = qBound(0, m_menuRow, rowCount() - 1);
        const QRect cell = visualRect(model()->index(row, 0));
        pos = cell.bottomLeft();
        if (pos.y() + overlay->height() > vp.height())
            pos.setY(qMax(0, cell.top() - overlay->height()));
    }
    if (pos.x() + overlay->width() > vp.width())
        pos.setX(qMax(0, vp.width() - overlay->width()));
    overlay->move(pos);
    overlay->raise();
    overlay->show();
}

void HmiTableWidget::ensureIndexMenu()
{
    if (m_indexMenu)
        return;
    auto *frame = new QFrame(viewport());
    frame->setObjectName(QStringLiteral("indexRowMenu"));
    frame->setFrameShape(QFrame::Box);
    frame->setFocusPolicy(Qt::StrongFocus);
    frame->setStyleSheet(QStringLiteral(
        "QFrame#indexRowMenu{background:white;border:2px solid rgb(155,200,33);}"));
    auto *lay = new QVBoxLayout(frame);
    lay->setContentsMargins(6, 6, 6, 6);
    lay->setSpacing(4);
    m_addBtn = new BtnCtrl(tr("Add"), frame);
    m_delBtn = new BtnCtrl(tr("Delete"), frame);
    m_addBtn->setStyleSheet(QLatin1String(kOverlayBtnStyle));
    m_delBtn->setStyleSheet(QLatin1String(kOverlayBtnStyle));
    lay->addWidget(m_addBtn);
    lay->addWidget(m_delBtn);
    m_indexMenu = frame;
    m_indexMenu->installEventFilter(this);
    m_addBtn->installEventFilter(this);
    m_delBtn->installEventFilter(this);
    connect(m_addBtn, SIGNAL(clicked()), this, SLOT(onIndexAdd()));
    connect(m_delBtn, SIGNAL(clicked()), this, SLOT(onIndexDelete()));
}

void HmiTableWidget::ensureConfirmMenu()
{
    if (m_confirmMenu)
        return;
    auto *frame = new QFrame(viewport());
    frame->setObjectName(QStringLiteral("indexRowMenu"));
    frame->setFrameShape(QFrame::Box);
    frame->setFocusPolicy(Qt::StrongFocus);
    frame->setStyleSheet(QStringLiteral(
        "QFrame#indexRowMenu{background:white;border:2px solid rgb(155,200,33);}"));
    auto *lay = new QVBoxLayout(frame);
    lay->setContentsMargins(6, 6, 6, 6);
    lay->setSpacing(4);
    m_confirmLabel = new QLabel(frame);
    m_confirmLabel->setWordWrap(true);
    m_confirmLabel->setAlignment(Qt::AlignCenter);
    m_yesBtn = new BtnCtrl(tr("Yes"), frame);
    m_noBtn = new BtnCtrl(tr("No"), frame);
    m_yesBtn->setStyleSheet(QLatin1String(kOverlayBtnStyle));
    m_noBtn->setStyleSheet(QLatin1String(kOverlayBtnStyle));
    lay->addWidget(m_confirmLabel);
    lay->addWidget(m_yesBtn);
    lay->addWidget(m_noBtn);
    m_confirmMenu = frame;
    m_confirmMenu->installEventFilter(this);
    m_confirmLabel->installEventFilter(this);
    m_yesBtn->installEventFilter(this);
    m_noBtn->installEventFilter(this);
    connect(m_yesBtn, SIGNAL(clicked()), this, SLOT(onConfirmDeleteAll()));
    connect(m_noBtn, SIGNAL(clicked()), this, SLOT(onCancelDeleteAll()));
}

void HmiTableWidget::showIndexMenu()
{
    m_suppressOverlayHideRestore = true;
    if (m_confirmMenu)
        m_confirmMenu->hide();
    m_suppressOverlayHideRestore = false;
    ensureIndexMenu();
    if (m_onHashHeader || rowCount() <= 0)
        m_menuRow = -1;
    else
    {
        m_menuRow = currentRow();
        if (m_menuRow < 0)
            m_menuRow = 0;
    }
    m_addBtn->setText(tr("Add"));
    if (m_menuRow < 0)
    {
        m_delBtn->setText(tr("Delete All"));
        m_delBtn->setEnabled(rowCount() > 0);
    }
    else
    {
        m_delBtn->setText(tr("Delete"));
        m_delBtn->setEnabled(m_menuRow >= 0 && m_menuRow < rowCount());
    }
    placeOverlay(m_indexMenu);
    m_addBtn->setFocus(Qt::OtherFocusReason);
}

void HmiTableWidget::showConfirmMenu()
{
    m_suppressOverlayHideRestore = true;
    if (m_indexMenu)
        m_indexMenu->hide();
    m_suppressOverlayHideRestore = false;
    ensureConfirmMenu();
    m_confirmLabel->setText(tr("Confirm to delete all?"));
    m_yesBtn->setText(tr("Yes"));
    m_noBtn->setText(tr("No"));
    placeOverlay(m_confirmMenu);
    m_noBtn->setFocus(Qt::OtherFocusReason);
}

void HmiTableWidget::hideOverlays(bool restoreFocus)
{
    m_suppressOverlayHideRestore = true;
    if (m_indexMenu && m_indexMenu->isVisible())
        m_indexMenu->hide();
    if (m_confirmMenu && m_confirmMenu->isVisible())
        m_confirmMenu->hide();
    m_suppressOverlayHideRestore = false;
    if (!restoreFocus)
        return;
    if (m_menuRow < 0)
        m_onHashHeader = true;
    else if (rowCount() <= 0)
        m_onHashHeader = true;
    else
    {
        m_onHashHeader = false;
        if (columnCount() > 0)
            m_currentIndex = model()->index(qBound(0, m_menuRow, rowCount() - 1), 0);
    }
    setFocus(Qt::OtherFocusReason);
    restoreInnerCell();
}

void HmiTableWidget::hideIndexMenu()
{
    hideOverlays(true);
}

void HmiTableWidget::clearAllDataRows()
{
    const bool blocked = blockSignals(true);
    setRowCount(0);
    blockSignals(blocked);
    m_menuRow = -1;
    m_onHashHeader = true;
    m_currentIndex = QModelIndex();
    if (m_inside)
        restoreInnerCell();
}

bool HmiTableWidget::isIndexMenuObject(QObject *obj) const
{
    return obj == m_indexMenu || obj == m_addBtn || obj == m_delBtn
        || obj == m_confirmMenu || obj == m_confirmLabel
        || obj == m_yesBtn || obj == m_noBtn;
}

bool HmiTableWidget::moveIndexMenuFocus(bool down)
{
    if (!isIndexMenuOpen())
        return false;
    if (m_confirmMenu && m_confirmMenu->isVisible())
    {
        if (down)
        {
            if (m_yesBtn->hasFocus())
                m_noBtn->setFocus();
            else
                m_yesBtn->setFocus();
        }
        else
        {
            if (m_noBtn->hasFocus())
                m_yesBtn->setFocus();
            else
                m_noBtn->setFocus();
        }
        return true;
    }
    if (down)
    {
        if (m_addBtn->hasFocus() && m_delBtn->isEnabled())
            m_delBtn->setFocus();
        else
            m_addBtn->setFocus();
    }
    else
    {
        if (m_delBtn->hasFocus())
            m_addBtn->setFocus();
        else if (m_delBtn->isEnabled())
            m_delBtn->setFocus();
    }
    return true;
}

void HmiTableWidget::onIndexAdd()
{
    if (gateEditAuth())
        return;
    const int row = m_menuRow;
    const bool fromHeader = row < 0;
    hideOverlays(false);
    m_onHashHeader = fromHeader;
    if (fromHeader)
        insertEmptyRowAfter(-1);
    else
        insertEmptyRowAfter(row);
    setFocus(Qt::OtherFocusReason);
    restoreInnerCell();
}

void HmiTableWidget::onIndexDelete()
{
    if (gateEditAuth())
        return;
    if (m_menuRow < 0)
    {
        if (rowCount() <= 0)
        {
            hideOverlays(true);
            return;
        }
        m_suppressOverlayHideRestore = true;
        if (m_indexMenu)
            m_indexMenu->hide();
        m_suppressOverlayHideRestore = false;
        showConfirmMenu();
        return;
    }
    const int row = m_menuRow;
    hideOverlays(true);
    removeDataRow(row);
}

void HmiTableWidget::onConfirmDeleteAll()
{
    hideOverlays(false);
    m_onHashHeader = true;
    clearAllDataRows();
    setFocus(Qt::OtherFocusReason);
    restoreInnerCell();
}

void HmiTableWidget::onCancelDeleteAll()
{
    hideOverlays(true);
}

bool HmiTableWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (horizontalHeader() && obj == horizontalHeader()->viewport())
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            auto *me = static_cast<QMouseEvent *>(event);
            if (horizontalHeader()->logicalIndexAt(me->pos()) == 0)
            {
                m_onHashHeader = true;
                if (!hasFocus())
                    setFocus(Qt::MouseFocusReason);
                if (!m_inside)
                    enterInner();
                else
                    restoreInnerCell();
                return true;
            }
        }
        if (event->type() == QEvent::Resize)
            updateHashHeaderHighlight();
        return QTableWidget::eventFilter(obj, event);
    }

    if ((obj == m_indexMenu || obj == m_confirmMenu) && event->type() == QEvent::Hide)
    {
        if (m_inside && !m_suppressOverlayHideRestore && !isIndexMenuOpen())
        {
            setFocus(Qt::OtherFocusReason);
            restoreInnerCell();
        }
        return QTableWidget::eventFilter(obj, event);
    }

    if (!isIndexMenuObject(obj))
        return QTableWidget::eventFilter(obj, event);

    if (event->type() != QEvent::KeyPress && event->type() != QEvent::ShortcutOverride)
        return QTableWidget::eventFilter(obj, event);

    const int key = static_cast<QKeyEvent *>(event)->key();
    const bool up = key == KEY_UP || key == Qt::Key_Up;
    const bool down = key == KEY_DOWN || key == Qt::Key_Down;
    if (!up && !down && !isBackKey(key) && !isEnterKey(key))
        return QTableWidget::eventFilter(obj, event);

    if (event->type() == QEvent::ShortcutOverride)
    {
        event->accept();
        return true;
    }

    if (up || down)
    {
        moveIndexMenuFocus(down);
        return true;
    }
    if (isBackKey(key))
    {
        hideIndexMenu();
        return true;
    }
    return QTableWidget::eventFilter(obj, event);
}

bool HmiTableWidget::event(QEvent *event)
{
    if ((m_inside || isIndexMenuOpen()) && event->type() == QEvent::ShortcutOverride)
    {
        const int key = static_cast<QKeyEvent *>(event)->key();
        if (isNavKey(key) || isBackKey(key) || isEnterKey(key))
        {
            event->accept();
            return true;
        }
    }
    return QTableWidget::event(event);
}

void HmiTableWidget::selectHashHeader()
{
    m_onHashHeader = true;
    m_currentIndex = QModelIndex();
    setCurrentIndex(QModelIndex());
    if (selectionModel())
        selectionModel()->clear();
    updateHashHeaderHighlight();
}

void HmiTableWidget::selectCell(int row, int col)
{
    if (!model() || rowCount() <= 0 || columnCount() <= 0)
    {
        selectHashHeader();
        return;
    }
    row = qBound(0, row, rowCount() - 1);
    col = qBound(0, col, columnCount() - 1);
    m_onHashHeader = false;
    m_currentIndex = model()->index(row, col);
    setCurrentIndex(m_currentIndex);
    if (selectionModel())
        selectionModel()->select(m_currentIndex, QItemSelectionModel::ClearAndSelect);
    updateHashHeaderHighlight();
}

void HmiTableWidget::updateHashHeaderHighlight()
{
    QHeaderView *hdr = horizontalHeader();
    if (!hdr || !hdr->viewport())
        return;
    if (!m_hashHeaderHi)
        m_hashHeaderHi = new HeaderHashHighlight(hdr->viewport());
    m_hashHeaderHi->setGeometry(hdr->sectionViewportPosition(0), 0,
                                hdr->sectionSize(0), hdr->viewport()->height());
    m_hashHeaderHi->setVisible(m_inside && m_onHashHeader);
    if (m_inside && m_onHashHeader)
    {
        m_hashHeaderHi->raise();
        m_hashHeaderHi->update();
    }
}

bool HmiTableWidget::consumeInnerKey(int key)
{
    const bool up = key == KEY_UP || key == Qt::Key_Up;
    const bool down = key == KEY_DOWN || key == Qt::Key_Down;
    const bool left = key == KEY_LEFT || key == Qt::Key_Left;
    const bool right = key == KEY_RIGHT || key == Qt::Key_Right;

    if (m_onHashHeader)
    {
        if (rowCount() > 0 && columnCount() > 0)
        {
            if (down)
                selectCell(0, 0);
            else if (right)
                selectCell(0, qMin(1, columnCount() - 1));
        }
        return up || down || left || right;
    }

    if (!model() || model()->rowCount() <= 0 || model()->columnCount() <= 0)
    {
        selectHashHeader();
        return up || down || left || right;
    }

    const int rows = model()->rowCount();
    const int cols = model()->columnCount();

    QModelIndex index = currentIndex();
    if (!index.isValid())
        index = m_currentIndex.isValid() ? m_currentIndex : model()->index(0, 0);

    int row = index.row();
    int col = index.column();

    if (up)
    {
        if (row == 0 && col == 0)
        {
            selectHashHeader();
            return true;
        }
        if (row > 0)
            --row;
    }
    else if (down)
    {
        if (row < rows - 1)
            ++row;
    }
    else if (left)
    {
        if (col > 0)
            --col;
        else if (row > 0)
        {
            --row;
            col = cols - 1;
        }
    }
    else if (right)
    {
        if (col < cols - 1)
            ++col;
        else if (row < rows - 1)
        {
            ++row;
            col = 0;
        }
    }
    else
        return false;

    selectCell(row, col);
    return true;
}

void HmiTableWidget::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();

    if (!m_inside)
    {
        if (isEnterKey(key))
            enterInner();
        return;
    }

    if (isEnterKey(key) && m_skipCellActivate)
    {
        m_skipCellActivate = false;
        return;
    }
    if (!isEnterKey(key))
        m_skipCellActivate = false;

    if (isIndexMenuOpen())
    {
        if (isBackKey(key))
            hideIndexMenu();
        else if (key == KEY_UP || key == Qt::Key_Up
                 || key == KEY_DOWN || key == Qt::Key_Down)
            moveIndexMenuFocus(key == KEY_DOWN || key == Qt::Key_Down);
        return;
    }

    if (isBackKey(key))
    {
        if (cancelCellEdit())
            return;
        exitInner();
        return;
    }

    if (consumeInnerKey(key))
        return;

    if (isEnterKey(key))
        activateCurrentCell();
}

void HmiTableWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (isEnterKey(event->key()))
        m_skipCellActivate = false;
    QTableWidget::keyReleaseEvent(event);
}

bool HmiTableWidget::edit(const QModelIndex &index, EditTrigger trigger, QEvent *event)
{
    Q_UNUSED(trigger);
    if (index.column() == 0 || !m_allowEdit)
        return false;
    return QTableWidget::edit(index, QAbstractItemView::AllEditTriggers, event);
}

void HmiTableWidget::focusInEvent(QFocusEvent *event)
{
    QAbstractScrollArea::focusInEvent(event);
    if (m_resumeInner)
    {
        resumeInnerNow();
        return;
    }
    if (m_inside)
        restoreInnerCell();
    else
        applyOuterVisuals();
}

void HmiTableWidget::focusOutEvent(QFocusEvent *event)
{
    QWidget *fw = QApplication::focusWidget();
    if (isIndexMenuOpen() || (fw && (fw == this || isAncestorOf(fw))))
    {
        QTableWidget::focusOutEvent(event);
        updateFocusBorder();
        return;
    }
    if (m_inside && !m_onHashHeader)
        m_currentIndex = currentIndex();
    m_inside = false;
    emit panelShortcutsEnabled(true);
    QTableWidget::focusOutEvent(event);
    updateFocusBorder();
    updateHashHeaderHighlight();
}

void HmiTableWidget::mousePressEvent(QMouseEvent *event)
{
    m_onHashHeader = false;
    QTableWidget::mousePressEvent(event);
    m_currentIndex = currentIndex();
    if (!m_inside)
        enterInner();
    else
        updateHashHeaderHighlight();
}

void HmiTableWidget::resizeEvent(QResizeEvent *event)
{
    QTableWidget::resizeEvent(event);
    updateFocusBorder();
    updateHashHeaderHighlight();
}
