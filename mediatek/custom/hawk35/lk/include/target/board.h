#ifndef __BOARD_H
#define __BOARD_H

#define CONFIG_CFB_CONSOLE 
#define CFB_CONSOLE_ON

#define CONFIG_SYS_PROMPT               "LK> "
#define CONFIG_SYS_CBSIZE               256  		 /* Console I/O Buffer Size */

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE               (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

//Project specific header file
#define CFG_DISPLAY_WIDTH       (DISP_GetScreenWidth())
#define CFG_DISPLAY_HEIGHT      (DISP_GetScreenHeight())
#define CFG_DISPLAY_BPP         (16)

#define CFG_POWER_CHARGING
//<2012/06/20-tedwu, add MOTO charging animation on powerOff charging.
//#define ARIMA_CHARGE_ANIMATION_FOR_MOTO
#define CAPACITY_CHART_WIDTH     98
#define CAPACITY_CHART_HEIGHT    32
#define CAPACITY_CHART_LEFT     111
#define CAPACITY_CHART_TOP      359
//<2012/12/06-tedwu, change for new CXD charging animation.
//#define CXD_ANIMATION_NEW
//>2012/12/06-tedwu
//>2012/06/20-tedwu

//<2012/7/9-tedwu, check MOTO battery ID
#define CHK_BATTERY_ID__MOTO_ID
#if defined(CHK_BATTERY_ID__MOTO_ID)
    #define CHK_BATTERY_ID__WITH_INTERNAL_PULL_UP_R
    #define BATTERY_PACK_HW4X  // for projct Hawk35
    //#define BATTERY_PACK_EG30  // for projct Hawk40
    //#define BATTERY_PACK_BP6X  // for projct ArgonMini
#endif
//>2012/7/9-tedwu

//<2012/11/21-tedwu, Integrate SOP-0296 partial charging
#define MOTO_SOP_0296_PARTIAL_CHARGE
//>2012/11/21-tedwu

#endif
