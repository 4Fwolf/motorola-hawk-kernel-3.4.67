#include <cust_leds.h>
#include <cust_leds_def.h>
#include <mach/mt_pwm.h>

#include <linux/kernel.h>
#include <mach/pmic_mt6329_hw_bank1.h> 
#include <mach/pmic_mt6329_sw_bank1.h> 
#include <mach/pmic_mt6329_hw.h>
#include <mach/pmic_mt6329_sw.h>
#include <mach/upmu_common_sw.h>
#include <mach/upmu_hw.h>

extern int mtkfb_set_backlight_level(unsigned int level);
extern int mtkfb_set_backlight_pwm(int div);

unsigned int brightness_mapping(unsigned int level)
{
    unsigned int mapped_level;
    
    mapped_level = level;
       
	return mapped_level;
}

unsigned int Cust_SetBacklight(int level, int div)
{
    mtkfb_set_backlight_pwm(div);
    mtkfb_set_backlight_level(brightness_mapping(level));
    return 0;
}

static struct cust_mt65xx_led cust_led_list[MT65XX_LED_TYPE_TOTAL] = {
	{"red",               MT65XX_LED_MODE_NONE, -1,{0}},
//<2012/09/09 yaotsulin-13738, Merge Hawk35 driver from Jelly Bean
#if defined(HAWK35_EP0)
	//<2012/11/2-16166-Cokychen, [8317][DRV] Modify LED control pin
	#if defined(HAWK35_DTV_EP1)
	{"green",             MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK1,{0}},
	#else
	{"green",             MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_FLASH, {0}},
	#endif
	//>2012/11/2-16166-Cokychen
#else
	{"green",             MT65XX_LED_MODE_NONE, -1,{0}},
#endif
//>2012/09/09 yaotsulin-13738
	{"blue",              MT65XX_LED_MODE_NONE, -1,{0}},
	{"jogball-backlight", MT65XX_LED_MODE_NONE, -1,{0}},
	{"keyboard-backlight",MT65XX_LED_MODE_NONE, -1,{0}},
	{"button-backlight",  MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_BUTTON,{0}},
	{"lcd-backlight",     MT65XX_LED_MODE_PWM, PWM1,{0}},
};

struct cust_mt65xx_led *get_cust_led_list(void)
{
	return cust_led_list;
}

