#ifndef BOTTOMBAR_H
#define BOTTOMBAR_H

#include <QWidget>
#include <QVector>

class QPushButton;
class QLabel;

/** Bottom navigator — matches weiduodianzi BottomWidget. */
class BottomBar : public QWidget
{
    Q_OBJECT
public:
    explicit BottomBar(QWidget *parent = nullptr);

    void setActiveNav(int index); // 0=Run, 1=Param, 2=Setup
    /** Highlight tab and move keyboard focus (bottom nav). */
    void focusNav(int index = -1);
    void setLinkOk(bool ok);
    void setPressWarn(int kind); // 0=none, 1=low, 2=high

signals:
    void runClicked();
    void paramClicked();
    void setupClicked();

private:
    void applyNavStyles();

    QVector<QPushButton *> m_btns;
    QLabel *m_link = nullptr;
    QLabel *m_press = nullptr;
    QLabel *m_weep = nullptr;
    int m_active = 0;
};

#endif
