#ifndef _COMMON_H
#define _COMMON_H

//5寸
//#define SCREEN_WIDTH 640
//#define SCREEN_HEIGH 480

//320*240
#define SCREEN_WIDTH 320
#define SCREEN_HEIGH 240
#define CONTROL_OFFSET 45
//480*272
//#define SCREEN_WIDTH 480
//#define SCREEN_HEIGH 272
//#define CONTROL_OFFSET 51

#define RUNPAGE_INDEX			0
#define RUNPARAMPAGE_INDEX		1
#define SETUPPAGE_INDEX			2
#define ADMINPAGE_INDEX			3
#define PWDPAGE_INDEX			4
#define LANGUAGEPAGE_INDEX		5
#define MSGPAGE_INDEX			6
#define PERMITPAGE_INDEX		7

//xhs
#define FIXPAGE_INDEX			8
#define FLOWFIXPAGE_INDEX		9//流量修正
#define TIMEPAGE_INDEX			10
#define WAVEFIXPAGE_INDEX		11//波长(压力泵)修正
#define GRADIENTPAGE_INDEX		12//梯度选择页面;
#define GRADIENTTABLE_INDEX		13//梯度表格设置;
#define DEBUGPRESS_INDEX		14
#define LOGOPAGE_INDEX			15
#define NETPAGE_INDEX			16
#define GLPINFOPAGE_INDEX		17

#define STARTUPPAGE_INDEX		0

#define MAXPAGEINDEX 18

#define LICENSE					"01080121"
#define SERIAL					"1507218001"

#define ENGLISH					0
#define CHINESE					1

#if 0
#define KEY_UP					Qt::Key_Up
#define KEY_DOWN				Qt::Key_Down
#define KEY_LEFT				Qt::Key_Left		
#define KEY_RIGHT				Qt::Key_Right
#define KEY_BACKSPACE			Qt::Key_Backspace
#define KEY_RETURN				Qt::Key_Return
#define KEY_PUMPSTOP			Qt::Key_F6
#define KEY_STARTHOLD			Qt::Key_F7
#define KEY_PURGE				Qt::Key_F8
#define KEY_SUPER				Qt::CTRL+KEY_UP
#else
#define KEY_UP					Qt::Key_F2
#define KEY_DOWN				Qt::Key_F5
#define KEY_LEFT				Qt::Key_F1		
#define KEY_RIGHT				Qt::Key_F3
#define KEY_BACKSPACE			Qt::Key_Backspace
#define KEY_RETURN				Qt::Key_Return
#define KEY_PUMPSTOP			Qt::Key_F6
#define KEY_STARTHOLD			Qt::Key_F7
#define KEY_PURGE				Qt::Key_F8
#define KEY_SUPER				Qt::CTRL+KEY_UP
#endif

#endif