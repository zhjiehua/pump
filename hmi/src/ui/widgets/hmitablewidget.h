#ifndef HMITABLEWIDGET_H
#define HMITABLEWIDGET_H

#include <QModelIndex>
#include <QStringList>
#include <QTableWidget>
#include <QVector>

class BtnCtrl;
class MainWindow;
class QAbstractItemDelegate;
class QLabel;
class QWidget;

/** Table with an index column and nested cell-nav. */
class HmiTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit HmiTableWidget(QWidget *parent = nullptr);

    void initIndex();
    bool capturesPanelKeys() const { return m_inside; }
    bool isInside() const { return m_inside; }
    bool isIndexMenuOpen() const;
    static HmiTableWidget *owningTable(QWidget *w);

    void enterInner();
    void exitInner();
    void activateCurrentCell();
    bool handleInnerNavKey(int key);
    /** Close index menu, cancel cell edit, or leave cell-nav. Returns true if consumed. */
    bool handleBack();
    bool cancelCellEdit();

    void setDataColumnCount(int n);
    void setDataHeaders(const QStringList &labels);
    void setDataDelegate(int dataCol, QAbstractItemDelegate *delegate);
    void setDataText(int row, int dataCol, const QString &text);
    QString dataText(int row, int dataCol) const;
    int dataColumnCount() const;

    bool isDataRowEmpty(int row) const;
    void setFilledRowTexts(const QVector<QStringList> &rows);
    void insertEmptyRowAfter(int row);
    void removeDataRow(int row);
    void addRowAfterCurrent();
    /** Commit an open cell editor so dataText() sees the typed value. */
    void commitActiveEditor();

    /** Require user password before cell edit / add / delete. page is MainWindow::Page. */
    void setEditAuthPage(MainWindow *main, int page);
    void refreshEditAuth();

signals:
    /** dir: 0 up, 1 down, 2 left, 3 right */
    void outOfTableFocus(int dir);
    /** false while the table owns panel keys (legacy setShortCutDisable). */
    void panelShortcutsEnabled(bool enabled);
    void editAuthRequested();

protected:
    bool event(QEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event) override;

private slots:
    void onIndexAdd();
    void onIndexDelete();
    void onConfirmDeleteAll();
    void onCancelDeleteAll();
    void resumeInnerNow();

private:
    void applyOuterVisuals();
    void restoreInnerCell();
    void syncInsideStyle();
    void updateFocusBorder();
    bool consumeInnerKey(int key);
    void fillEmptyRow(int row);
    void setIndexItem(int row);
    void refreshIndexColumn();
    int dataColToView(int dataCol) const { return dataCol + 1; }
    void ensureIndexMenu();
    void showIndexMenu();
    void hideIndexMenu();
    void ensureConfirmMenu();
    void showConfirmMenu();
    void hideOverlays(bool restoreFocus);
    void placeOverlay(QWidget *overlay);
    void clearAllDataRows();
    void selectHashHeader();
    void selectCell(int row, int col);
    void updateHashHeaderHighlight();
    bool isIndexMenuObject(QObject *obj) const;
    bool moveIndexMenuFocus(bool down);
    bool gateEditAuth();

    QModelIndex m_currentIndex;
    MainWindow *m_authMain = nullptr;
    int m_authPage = -1;
    bool m_editAuthRequired = false;
    bool m_allowEdit = false;
    bool m_inside = false;
    bool m_onHashHeader = false;
    bool m_skipCellActivate = false;
    bool m_resumeInner = false;
    QWidget *m_focusBorder = nullptr;
    QWidget *m_hashHeaderHi = nullptr;
    QWidget *m_indexMenu = nullptr;
    QWidget *m_confirmMenu = nullptr;
    BtnCtrl *m_addBtn = nullptr;
    BtnCtrl *m_delBtn = nullptr;
    QLabel *m_confirmLabel = nullptr;
    BtnCtrl *m_yesBtn = nullptr;
    BtnCtrl *m_noBtn = nullptr;
    int m_menuRow = 0;
    bool m_suppressOverlayHideRestore = false;
};

#endif
