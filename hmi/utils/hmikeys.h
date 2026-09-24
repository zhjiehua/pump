#ifndef HMIKEYS_H
#define HMIKEYS_H

#include <Qt>

// PC / dev keyboard (panel F-keys still registered in MainWindow::initShortcuts).
#define KEY_UP Qt::Key_Up
#define KEY_DOWN Qt::Key_Down
#define KEY_LEFT Qt::Key_Left
#define KEY_RIGHT Qt::Key_Right
#define KEY_BACKSPACE Qt::Key_Backspace
#define KEY_RETURN Qt::Key_Return
#define KEY_PUMPSTOP Qt::Key_S
#define KEY_STARTHOLD Qt::Key_R
#define KEY_PURGE Qt::Key_P
#define KEY_SUPER (Qt::CTRL | Qt::Key_Up)

// Physical panel (weiduodianzi Common.h)
#define PANEL_KEY_UP Qt::Key_F2
#define PANEL_KEY_DOWN Qt::Key_F5
#define PANEL_KEY_LEFT Qt::Key_F1
#define PANEL_KEY_RIGHT Qt::Key_F3

#endif
