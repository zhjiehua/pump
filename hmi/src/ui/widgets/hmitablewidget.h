#ifndef HMITABLEWIDGET_H
#define HMITABLEWIDGET_H

#include <QModelIndex>
#include <QTableWidget>

/** Table with legacy weiduodianzi TableCtrl key navigation. */
class HmiTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit HmiTableWidget(QWidget *parent = nullptr);

    void initIndex();
    bool capturesPanelKeys() const { return m_capturesPanelKeys; }

signals:
    /** dir: 0 up, 1 down, 2 left, 3 right */
    void outOfTableFocus(int dir);
    /** false while the table owns panel keys (legacy setShortCutDisable). */
    void panelShortcutsEnabled(bool enabled);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    QModelIndex m_currentIndex;
    bool m_capturesPanelKeys = false;
};

#endif
