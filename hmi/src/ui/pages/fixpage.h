#ifndef FIXPAGE_H
#define FIXPAGE_H

#include "ui/focuspage.h"

class MainWindow;
class QPushButton;
class QSignalMapper;

/** Calibration entry: press / flow (+ optional QinFine extras). */
class FixPage : public FocusPage
{
    Q_OBJECT
public:
    explicit FixPage(MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void goPage(int page);
    void onBack();

private:
    MainWindow *m_main = nullptr;
    QSignalMapper *m_mapper = nullptr;
    QPushButton *m_pressCal = nullptr;
    QPushButton *m_flowCal = nullptr;
    QPushButton *m_pulseCal = nullptr;
    QPushButton *m_pressCompen = nullptr;
};

#endif
