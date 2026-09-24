#ifndef TOPBAR_H
#define TOPBAR_H

#include <QWidget>
class QLabel;

/** Title strip — matches weiduodianzi TopWidget (blue bar). */
class TopBar : public QWidget
{
    Q_OBJECT
public:
    explicit TopBar(QWidget *parent = nullptr);
    void setTitle(const QString &title);

private:
    QLabel *m_title = nullptr;
};

#endif
