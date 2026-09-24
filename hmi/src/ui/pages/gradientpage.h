#ifndef GRADIENTPAGE_H
#define GRADIENTPAGE_H

#include "ui/focuspage.h"

#include <QVector>

class MachineController;
class MainWindow;
class QPushButton;
class QSignalMapper;

/** Gradient table picker (RG1–RG10). */
class GradientPage : public FocusPage
{
    Q_OBJECT
public:
    GradientPage(MachineController *c, MainWindow *main, QWidget *parent = nullptr);

protected:
    void initFocusList() override;

private slots:
    void onGradientSelected(int index);
    void onBack();

private:
    MachineController *m_c = nullptr;
    MainWindow *m_main = nullptr;
    QSignalMapper *m_mapper = nullptr;
    QVector<QPushButton *> m_rg;
    QPushButton *m_back = nullptr;
};

#endif
