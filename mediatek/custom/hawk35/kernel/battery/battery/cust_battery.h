#ifndef _CUST_BAT_H_
#define _CUST_BAT_H_

typedef enum
{
	Cust_CC_1600MA = 0x0,
	Cust_CC_1500MA = 0x1,
	Cust_CC_1400MA = 0x2,
	Cust_CC_1300MA = 0x3,
	Cust_CC_1200MA = 0x4,
	Cust_CC_1100MA = 0x5,
	Cust_CC_1000MA = 0x6,
	Cust_CC_900MA  = 0x7,
	Cust_CC_800MA  = 0x8,
	Cust_CC_700MA  = 0x9,
	Cust_CC_650MA  = 0xA,
	Cust_CC_550MA  = 0xB,
	Cust_CC_450MA  = 0xC,
	Cust_CC_400MA  = 0xD,
	Cust_CC_200MA  = 0xE,
	Cust_CC_70MA   = 0xF,
	Cust_CC_0MA	   = 0xDD
}cust_charging_current_enum;

typedef struct{	
	unsigned int BattVolt;
	unsigned int BattPercent;
}VBAT_TO_PERCENT;

/* Battery Temperature Protection */
//<2012/11/21-tedwu, Integrate SOP-0296 partial charging
#define MAX_CHARGE_TEMPERATURE  60	//50
//>2012/11/21-tedwu
#define MIN_CHARGE_TEMPERATURE  0
#define ERR_CHARGE_TEMPERATURE  0xFF

/* Recharging Battery Voltage */
#define RECHARGING_VOLTAGE      4110

/* Charging Current Setting */
#define CONFIG_USB_IF 						0   
#define USB_CHARGER_CURRENT_SUSPEND			Cust_CC_0MA		// def CONFIG_USB_IF
#define USB_CHARGER_CURRENT_UNCONFIGURED	Cust_CC_70MA	// def CONFIG_USB_IF
#define USB_CHARGER_CURRENT_CONFIGURED		Cust_CC_450MA	// def CONFIG_USB_IF
#define USB_CHARGER_CURRENT					Cust_CC_450MA
#define AC_CHARGER_CURRENT					Cust_CC_700MA	

/* Battery Meter Solution */
#define CONFIG_ADC_SOLUTION 	1

/* Battery Voltage and Percentage Mapping Table */
VBAT_TO_PERCENT Batt_VoltToPercent_Table[] = {
	/*BattVolt,BattPercent*/
//<2012/10/22-tedwu, Modify the voltage value to percentage.
//#if defined(BATTERY_PACK_HW4X)
	{3350,0},
	{3685,10},
	{3746,20},
	{3782,30},
	{3812,40},
	{3858,50},
	{3942,60},
	{4024,70},
	{4115,80},
	{4225,90},
	{4335,100},
//#else
//    #error Please modify to proper values.
/*	{3400,0},
	{3641,10},
	{3708,20},
	{3741,30},
	{3765,40},
	{3793,50},
	{3836,60},
	{3891,70},
	{3960,80},
	{4044,90},
	{4183,100},
*/
//#endif
//>2012/10/22-tedwu
};

/* Precise Tunning */
//#define BATTERY_AVERAGE_SIZE 	30
#define BATTERY_AVERAGE_SIZE   12

/* Common setting */
#define R_CURRENT_SENSE 2				// 0.2 Ohm
#define R_BAT_SENSE 4					// times of voltage
#define R_I_SENSE 4						// times of voltage
#define R_CHARGER_1 330
#define R_CHARGER_2 39
#define R_CHARGER_SENSE ((R_CHARGER_1+R_CHARGER_2)/R_CHARGER_2)	// times of voltage
//#define V_CHARGER_MAX 6000				// 6 V
#define V_CHARGER_MAX 6500				// 6.5 V
#define V_CHARGER_MIN 4400				// 4.4 V
#define V_CHARGER_ENABLE 0				// 1:ON , 0:OFF

/* Teperature related setting */
#define RBAT_PULL_UP_R             24000
//#define RBAT_PULL_UP_VOLT          2500
#define RBAT_PULL_UP_VOLT          1200
//#define TBAT_OVER_CRITICAL_LOW     68237
#define TBAT_OVER_CRITICAL_LOW     483954
//<2012/12/05-tedwu, Enable the BAT_TEMP_PROTECT_ENABLE.
#define BAT_TEMP_PROTECT_ENABLE    1 //0
//>2012/12/05-tedwu
#define BAT_NTC_10 1
#define BAT_NTC_47 0

//<2012/11/21-tedwu, Integrate SOP-0296 partial charging
#define MOTO_SOP_0296_PARTIAL_CHARGE

#if defined(MOTO_SOP_0296_PARTIAL_CHARGE)
#define CHR_PARTIAL													0x1006
#define PARTIAL_CHARGE_MIN_TEMPERATURE  		45	
#define PARTIAL_CHARGE_MAX_TEMPERATURE 			60
#define PARTIAL_MAX_VOLTAGE									4000
#define PARTIAL_RECHARGING_VOLTAGE      		3900
#endif
//>2012/11/21-tedwu

/* Battery Notify */
#define BATTERY_NOTIFY_CASE_0001
#define BATTERY_NOTIFY_CASE_0002
#define BATTERY_NOTIFY_CASE_0003
#define BATTERY_NOTIFY_CASE_0004
#define BATTERY_NOTIFY_CASE_0005

#endif /* _CUST_BAT_H_ */ 
