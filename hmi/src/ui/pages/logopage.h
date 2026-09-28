#ifndef LOGOPAGE_H
#define LOGOPAGE_H

#include <QWidget>

class MainWindow;

class LogoPage : public QWidget
{
    Q_OBJECT
public:
    LogoPage(MainWindow *main, QWidget *parent = nullptr);

private slots:
    void onTimeout();

private:
    MainWindow *m_main;
};

#endif
