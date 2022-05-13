
#ifndef __CONFIG_H__
#define __CONFIG_H__

//<<< Use Configuration Wizard in Context Menu >>>

//<h> Version Configuration
//<o> Hardware version
#define HW_VERSION 211212
//<o> Firmware version
#define FW_VERSION 70302
//</h>

//<e> watchdog Enable/Disable
#define _WDG_ENABLE 0
//<o> watchdog timeout(ms)
#define WDG_TIMEOUT_MS 1000
//</e>

//<e> LSI frequency calibration Enable/Disable
#define LSI_CALIB_ENABLE 1
//</e>

//<h> Utils Debug
//<o> Debug level
//<0=> off
//<1=> all
//<2=> warn+error
//<3=> error
#define _DEBUG_ENABLE 1
//</h>

//<e> NR Micro Shell Enable/Disable
#define _NR_MICRO_SHELL_ENABLE 1
//</e>

//<e> Proactively report results Enable/Disable
#define REPORT_ENABLE 1
//</e>

//<h> flex_button Configuration
//<o> How often flex_button_scan () is called
#define FLEX_BTN_SCAN_FREQ_HZ 50
//</h>

//<h> Battery voltage conversion coefficient Configuration
//<o> corrected parameter(mv)
#define BAT_DETA        46          
//<o> scale factor
#define BAT_FACTOR      11
//</h>




#if _DEBUG_ENABLE
// RESERVED FOR BCONFIGTOOL
#else
// RESERVED FOR BCONFIGTOOL
#endif

#include "deftypes.h"

#endif  // __CONFIG_H__

