#ifndef COMBOCTRL_H
#define COMBOCTRL_H

#include <QComboBox>

/** Panel combo: Enter opens the list; panel keys move items while it is open. */
class ComboCtrl : public QComboBox
{
    Q_OBJECT
public:
    explicit ComboCtrl(QWidget *parent = nullptr);

    bool isPopupOpen() const;

    /** When locked, opening the list or changing the index is blocked. */
    void setChangeLocked(bool locked);
    bool isChangeLocked() const { return m_changeLocked; }

    void showPopup() override;
    void hidePopup() override;

signals:
    void popupChanged(bool open);
    void changeBlocked();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    void installPopupFilter();
    bool isPopupObject(QObject *obj) const;
    bool movePopupRow(bool down);
    bool rejectIfLocked();

    bool m_popupOpen = false;
    bool m_changeLocked = false;
};

#endif
