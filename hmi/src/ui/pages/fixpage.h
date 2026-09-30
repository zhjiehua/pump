#ifndef FIXPAGE_H
#define FIXPAGE_H

#include "ui/focuspage.h"

class MainWindow;
class BtnCtrl;
class ImgButton;
class QSignalMapper;

/** Calibration entry: press / flow (weiduodianzi FixPage). */
class FixPage : public FocusPage
{
    Q_OBJECT
public:
    explicit FixPage(MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;
    void retranslateUi() override;

private slots:
    void goPage(int page);

private:
    MainWindow *m_main = nullptr;
    QSignalMapper *m_mapper = nullptr;
    ImgButton *m_icon = nullptr;
    BtnCtrl *m_pressCal = nullptr;
    BtnCtrl *m_flowCal = nullptr;
};

#endif
