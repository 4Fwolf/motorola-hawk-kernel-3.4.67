//#include <platform/cust_leds.h>
#include <cust_leds.h>
//#include <asm/arch/mt6577_pwm.h>
//<2012/09/27-14404-jessicatseng, [Hawk 4.0] LCM backlight is controlled by PWM1 for JB 
#include <platform/mt_pwm.h>
//>2012/09/27-14404-jessicatseng	

extern int DISP_SetBacklight(int level);

static struct cust_mt65xx_led cust_led_list[MT65XX_LED_TYPE_TOTAL] = {
	{"red",               MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK1,{0}},
	{"green",             MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK3,{0}},
	{"blue",              MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_NLED_ISINK2,{0}},
	{"jogball-backlight", MT65XX_LED_MODE_NONE, -1,{0}},
	{"keyboard-backlight",MT65XX_LED_MODE_NONE, -1,{0}},
	{"button-backlight",  MT65XX_LED_MODE_PMIC, MT65XX_LED_PMIC_BUTTON,{0}},
//<2012/09/27-14404-jessicatseng, [Hawk 4.0] LCM backlight is controlled by PWM1 for JB 
	{"lcd-backlight",     MT65XX_LED_MODE_PWM, PWM1, {0}},	
//>2012/09/27-14404-jessicatseng	
};

struct cust_mt65xx_led *get_cust_led_list(void)
{
	return cust_led_list;
}

