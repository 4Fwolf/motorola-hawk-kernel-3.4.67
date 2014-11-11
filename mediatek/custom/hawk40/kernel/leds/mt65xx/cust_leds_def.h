#ifndef _CUST_LEDS_DEF_H
#define _CUST_LEDS_DEF_H

//#define CUST_LEDS_BACKLIGHT_PMIC_PARA /* parallel */
//#define CUST_LEDS_BACKLIGHT_PMIC_SERI /* series */
enum mt65xx_led_mode
{
	MT65XX_LED_MODE_NONE,
	MT65XX_LED_MODE_PWM,
	MT65XX_LED_MODE_GPIO,
	MT65XX_LED_MODE_PMIC,
	MT65XX_LED_MODE_CUST_LCM,
	MT65XX_LED_MODE_CUST_BLS_PWM
};

enum mt65xx_led_pmic
{
	MT65XX_LED_PMIC_BUTTON=0,
	MT65XX_LED_PMIC_LCD,
	MT65XX_LED_PMIC_LCD_ISINK,
	MT65XX_LED_PMIC_LCD_BOOST,
	//MT65XX_LED_PMIC_NLED_ISINK3,
	MT65XX_LED_PMIC_NLED_ISINK4,
	MT65XX_LED_PMIC_NLED_ISINK5
	//<2012/10/19 ShermanWei, Implement NotificationLED while charging in off mode
	,MT65XX_LED_PMIC_FLASH
	//>2012/10/19 ShermanWei
//<2012/09/28-14478-jessicatseng, [Hawk 4.0] Porting RGB LED for JB	
	,MT65XX_LED_PMIC_NLED_ISINK1
	,MT65XX_LED_PMIC_NLED_ISINK2
//>2012/09/28-14478-jessicatseng	
//<2012/11/19-17190-jessicatseng, [Hawk 4.0] Add ISINK3 function
	,MT65XX_LED_PMIC_NLED_ISINK3
//>2012/11/19-17190-jessicatseng
};
struct PWM_config
{
	int clock_source;
	int div;
	int low_duration;
	int High_duration;
	BOOL pmic_pad;
};
typedef int (*cust_brightness_set)(int level, int div);
typedef int (*cust_set_brightness)(int level);

/*
 * name : must the same as lights HAL
 * mode : control mode
 * data :
 *    PWM:  pwm number
 *    GPIO: gpio id
 *    PMIC: enum mt65xx_led_pmic
 *    CUST: custom set brightness function pointer
*/
struct cust_mt65xx_led {
	char                 *name;
	enum mt65xx_led_mode  mode;
	int                   data;
 struct PWM_config config_data;
};

extern struct cust_mt65xx_led *get_cust_led_list(void);
#endif /* _CUST_LEDS_DEF_H */
