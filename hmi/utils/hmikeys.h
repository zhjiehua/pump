#ifndef HMIKEYS_H
#define HMIKEYS_H

#include "utils/hmiconfig.h"

#include <Qt>

#if HMI_EMBEDDED
// Physical panel scan codes (weiduodianzi Common.h).
#define KEY_UP Qt::Key_F2
#define KEY_DOWN Qt::Key_F5
#define KEY_LEFT Qt::Key_F1
#define KEY_RIGHT Qt::Key_F3
#else
// PC / desktop keyboard.
#define KEY_UP Qt::Key_W
#define KEY_DOWN Qt::Key_S
#define KEY_LEFT Qt::Key_A
#define KEY_RIGHT Qt::Key_D
#endif

#define KEY_BACKSPACE Qt::Key_Backspace
#define KEY_RETURN Qt::Key_Return
#define KEY_PUMPSTOP Qt::Key_F6
#define KEY_STARTHOLD Qt::Key_F7
#define KEY_PURGE Qt::Key_F8
#define KEY_SUPER (Qt::CTRL | KEY_UP)

#endif
