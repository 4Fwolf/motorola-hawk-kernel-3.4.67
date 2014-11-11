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
//<2012/10/15-15166-jessicatseng, [Hawk 4.0] Red/Green LED switch for EP1
#if defined(HAWK40_EP1)
	{"red",               MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK1,{0}},
	{"green",             MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK3,{0}},
	{"blue",              MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK2,{0}},
#else
	{"red",               MT65XX_LED_MODE_PWM,  PWM3,{0}},
	{"green",             MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK1,{0}},
	{"blue",              MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK2,{0}},
#endif
//>2012/10/15-15166-jessicatseng
	{"jogball-backlight", MT65XX_LED_MODE_NONE, -1,{0}},
	{"keyboard-backlight",MT65XX_LED_MODE_NONE, -1,{0}},
	{"button-backlight",  MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_BUTTON,{0}},
//<2012/09/27-14404-jessicatseng, [Hawk 4.0] LCM backlight is controlled by PWM1 for JB 
	{"lcd-backlight",     MT65XX_LED_MODE_PWM, PWM1,{0}},
//>2012/09/27-14404-jessicatseng
};

struct cust_mt65xx_led *get_cust_led_list(void)
{
	return cust_led_list;
}

