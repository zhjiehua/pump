#ifndef FOCUSPAGE_H
#define FOCUSPAGE_H

#include <QObject>
#include <QWidget>

/** Page with explicit x/y focus order (legacy weiduodianzi BasePage). */
class FocusPage : public QWidget
{
    Q_OBJECT
public:
    explicit FocusPage(QWidget *parent = nullptr);

    QObjectList xList;
    QObjectList yList;

    void initFocus(bool grabFocus = true);
    /** Drop leftover child focus (e.g. when returning to the bottom navigator). */
    void releaseChildFocus();

    QWidget *defaultFocusWidget() const;
    QWidget *lastFocusWidget() const;
    bool moveSpatialFocus(int key);

    /** Return true if the key was handled (e.g. combo prev/next). */
    virtual bool handleFocusNavKey(int key);

private slots:
    void restoreLastFocus();

protected:
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    virtual void initFocusList() {}
    virtual void retranslateUi() {}

    static void prepareFocusWidget(QWidget *w);
};

#endif
