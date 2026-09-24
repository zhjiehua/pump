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

    void initFocus();

    QWidget *defaultFocusWidget() const;
    bool moveSpatialFocus(int key);

    /** Return true if the key was handled (e.g. combo prev/next). */
    virtual bool handleFocusNavKey(int key);

protected:
    virtual void initFocusList() {}

    static void prepareFocusWidget(QWidget *w);
};

#endif
