#ifndef HMICONFIG_H
#define HMICONFIG_H

/**
 * HMI build-time features (set in hmi.pro):
 *   CONFIG+=embedded  -> EMBEDDED_LINUX  (fullscreen, IoModule, panel key map)
 *   CONFIG+=touch     -> HMI_INPUT_TOUCH   (on-screen numeric keyboard for edits)
 */

#if defined(EMBEDDED_LINUX)
#  define HMI_EMBEDDED 1
#else
#  define HMI_EMBEDDED 0
#endif

#if defined(HMI_INPUT_TOUCH)
#  define HMI_USE_ONSCREEN_KEYBOARD 1
#else
#  define HMI_USE_ONSCREEN_KEYBOARD 0
#endif

#endif
