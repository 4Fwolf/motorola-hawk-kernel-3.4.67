//<2012/10/05-14765-jessicatseng, [Hawk 4.0] Enable to check MOTO battery ID in power on mode
#include <linux/delay.h>

#include <mach/mt_gpio.h>
#include <mach/mt_typedefs.h>
#include <mach/pmic_mt6329_hw_bank1.h> 
#include <mach/pmic_mt6329_sw_bank1.h> 
#include <mach/pmic_mt6329_hw.h>
#include <mach/pmic_mt6329_sw.h>
#include <mach/upmu_common_sw.h>
#include <mach/upmu_hw.h>

#include "bat_id.h"

/*************************************************************************
 * Debugs
 ************************************************************************/
//#define BQ24158_DEBUG
#ifdef BAT_ID_DEBUG
#define BATIDDB printk
#else
#define BATIDDB(x,...)
#endif

//<2012/7/9-tedwu, check MOTO battery ID
//<2012/03/28-4645-jessicatseng, Modify charging code suggested by MTK
//<2012/02/07-2950-jessicatseng, Just check page4 for MOTO battery ID checked
//<2012/01/02-2106-jessicatseng, Add code to check MOTO battery ID for DS2502 and bq2024
#if defined(CHK_BATTERY_ID__MOTO_ID)
#define BATTERY_CHECK_MAX_NUM		5
//<2012/10/26-15796-jessicatseng, [Hawk 4.0] add "spin_lock" function when reading battery ID
static spinlock_t g_BatteryID_SpinLock;
//>2012/10/26-15796-jessicatseng
static kal_bool Is_battery_valid = KAL_FALSE;
static int Battery_ID_read_count = 0;
static char rom_data[8];
static char memory_data[32*4];
static char Family_Code, Max_Voltage, page2_CheckSum, page3_CheckSum, page4_Str[32+1];
static int  Custom_ID;
static const char page4_Str_MOTO[] = "COPR2012MOTOROLA E.P CHARGE ONLY";
//#endif
//>2012/01/02-2106-jessicatseng
//>2012/02/07-2950-jessicatseng
//>2012/03/28-4645-jessicatseng
//>2012/7/9-tedwu

//<2012/10/26-15796-jessicatseng, [Hawk 4.0] add "spin_lock" function when reading battery ID
void bat_id_init_spinLock(void)
{
	spin_lock_init(&g_BatteryID_SpinLock);
}
//>2012/10/26-15796-jessicatseng

//<2012/12/26-tedwu, for power consumption and avoid ESD issue on 1-wire pin?.
void bat_id_init_pin(void)
{
    mt_set_gpio_mode(GPIO_BATTERY_DATA_PIN, GPIO_BATTERY_DATA_PIN_M_GPIO);
	mt_set_gpio_dir(GPIO_BATTERY_DATA_PIN, GPIO_DIR_OUT);
	mt_set_gpio_pull_select(GPIO_BATTERY_DATA_PIN, GPIO_PULL_UP);
	mt_set_gpio_pull_enable(GPIO_BATTERY_DATA_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_out(GPIO_BATTERY_DATA_PIN, GPIO_OUT_ONE);
    udelay(550);
}

void bat_id_deinit_pin(void)
{
	mt_set_gpio_dir(GPIO_BATTERY_DATA_PIN, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(GPIO_BATTERY_DATA_PIN, GPIO_PULL_DISABLE);
}
//>2012/12/26-tedwu

void bat_id_init_variable(void)
{
	Battery_ID_read_count = 0;
	Is_battery_valid = KAL_FALSE;
}

//<2012/7/9-tedwu, check MOTO battery ID
//<2012/01/02-2106-jessicatseng, Add code to check MOTO battery ID for DS2502 and bq2024
static int OneWire_Reset_Bus(void)
{
	int presence_pulse;
	
	mt_set_gpio_out(GPIO_BATTERY_DATA_PIN, GPIO_OUT_ZERO);
	udelay(550);
	mt_set_gpio_out(GPIO_BATTERY_DATA_PIN, GPIO_OUT_ONE);
	udelay(60);
	mt_set_gpio_dir(GPIO_BATTERY_DATA_PIN, GPIO_DIR_IN);
	udelay(40);
	presence_pulse = mt_get_gpio_in(GPIO_BATTERY_DATA_PIN);
	mt_set_gpio_dir(GPIO_BATTERY_DATA_PIN, GPIO_DIR_OUT);
	udelay(400);
	
	//BATIDDB("Presence Pulse: %d\n", presence_pulse);
	
	return presence_pulse;
}

static void OneWire_Write_Byte(char cmd)
{
	int idx;
/*
#if defined(CHK_BATTERY_ID__WITH_INTERNAL_PULL_UP_R)
	mt_set_gpio_pull_enable(GPIO_BATTERY_DATA_PIN, GPIO_PULL_DISABLE);
#endif	
*/
	//mt_set_gpio_dir(GPIO_BATTERY_DATA_PIN, GPIO_DIR_OUT);
		
	for(idx=0; idx<8; idx++)
	{
		mt_set_gpio_out(GPIO_BATTERY_DATA_PIN, 0);
		udelay(3);
		
		if((cmd >> idx) & 0x01)
		{
			mt_set_gpio_out(GPIO_BATTERY_DATA_PIN, 1);
			udelay(60);
		}
		else
		{
			mt_set_gpio_out(GPIO_BATTERY_DATA_PIN, 0);
			udelay(60);
		}			
		
		mt_set_gpio_out(GPIO_BATTERY_DATA_PIN, 1);
		udelay(10);	// Trec
	}
}	

//<2012/10/26-15796-jessicatseng, [Hawk 4.0] add "spin_lock" function when reading battery ID
static char OneWire_Read_Byte(void)
{
	int idx = 0;
	char retbyte = 0;
	unsigned long flags;
	
	//mt_set_gpio_out(GPIO_BATTERY_DATA_PIN, 1);  //to be deleted?
/*
#if defined(CHK_BATTERY_ID__WITH_INTERNAL_PULL_UP_R)
	mt_set_gpio_pull_enable(GPIO_BATTERY_DATA_PIN, GPIO_PULL_DISABLE);
#endif	
*/
	//mt_set_gpio_dir(GPIO_BATTERY_DATA_PIN, GPIO_DIR_OUT);  //to be deleted?

	spin_lock_irqsave(&g_BatteryID_SpinLock, flags);
	
	for(idx=0; idx<8; idx++)
	{
		mt_set_gpio_out(GPIO_BATTERY_DATA_PIN, 0);
		
		//This delay has to remove, because Tsu has to be less than 1us for DS2502
		//udelay(1 /*3*/);
/*		
#if defined(CHK_BATTERY_ID__WITH_INTERNAL_PULL_UP_R)
		mt_set_gpio_pull_select(GPIO_BATTERY_DATA_PIN, GPIO_PULL_UP);
		mt_set_gpio_pull_enable(GPIO_BATTERY_DATA_PIN, GPIO_PULL_ENABLE);
#endif
*/
		mt_set_gpio_dir(GPIO_BATTERY_DATA_PIN, GPIO_DIR_IN);
//<2012/02/07-2950-jessicatseng, Just check page4 for MOTO battery ID checked
		//udelay(15);
		udelay(18);
//>2012/02/07-2950-jessicatseng
		
		if(mt_get_gpio_in(GPIO_BATTERY_DATA_PIN) & 0x01)
		{
			retbyte |= (0x01 << idx);
		}	
		
		udelay(60);
		
		mt_set_gpio_out(GPIO_BATTERY_DATA_PIN, 1);
/*
#if defined(CHK_BATTERY_ID__WITH_INTERNAL_PULL_UP_R)
		mt_set_gpio_pull_enable(GPIO_BATTERY_DATA_PIN, GPIO_PULL_DISABLE);
#endif		
*/
		mt_set_gpio_dir(GPIO_BATTERY_DATA_PIN, GPIO_DIR_OUT);

		udelay(10);	// Trec
	}

	spin_unlock_irqrestore(&g_BatteryID_SpinLock, flags);
	
	return retbyte;
}
//>2012/10/26-15796-jessicatseng
	
//<2012/03/28-4645-jessicatseng, Modify charging code suggested by MTK
void Read_Battery_Info(void)
{
	char read_rom = 0x33, skip_rom = 0xcc, read_memory = 0xF0;
	int idx = 0, page_idx;
		
  BATIDDB("[BATTERY:bq24158] Read_Battery_Info Start ----------------------------\n");

//<2012/12/26-tedwu, for power consumption and avoid ESD issue on 1-wire pin?.
#if defined(ARIMA_PROJECT_HAWK35)
	bat_id_init_pin();
#endif
//>2012/12/26-tedwu
	if(OneWire_Reset_Bus() == 0)
	{	
		// Read ROM
		OneWire_Write_Byte(read_rom);
		
		for(idx=0; idx<8; idx++)
		{	
			rom_data[idx] = OneWire_Read_Byte();
		}	

		//for(idx=0; idx<(sizeof(rom_data)); idx++)
			//BATIDDB("0x%X, ", rom_data[idx]);
		
		//BATIDDB("\n=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
		
		// Read Memory		
		for(page_idx=0; page_idx<4; page_idx++)
		//for(page_idx=3; page_idx<4; page_idx++)
		{		
			OneWire_Reset_Bus();
			OneWire_Write_Byte(skip_rom);
			OneWire_Write_Byte(read_memory);
			OneWire_Write_Byte(0x00 + (0x20 * page_idx));
			OneWire_Write_Byte(0x00);						
			
			OneWire_Read_Byte();	// CRC
			
			for(idx=0; idx<32; idx++)
			{
				memory_data[idx + (0x20 * page_idx)] = OneWire_Read_Byte();
			}	
		}

		// parsing data		
		{
			Family_Code = rom_data[0];
			Custom_ID = (int)(((rom_data[6]<<8)|(rom_data[5]&0xF0))>>4);
			Max_Voltage = memory_data[2];
			page2_CheckSum = memory_data[0x20];
			page3_CheckSum = memory_data[0x40];
			memset(page4_Str, 0x00, sizeof(page4_Str));
			strncpy(page4_Str, &(memory_data[0x60]), 32);
			
			BATIDDB("Family_Code = %x, Custom_ID = %x, Max_Voltage = %x, page2_CheckSum = %x, page3_CheckSum = %x\n", \
							Family_Code, Custom_ID, Max_Voltage, page2_CheckSum, page3_CheckSum);
			BATIDDB("page4_Str = %s\n", page4_Str);					
		}		
	}
	else
	{
		BATIDDB("[BATTERY:bq24158] presence pulse = high\n");
	}		
	
//<2012/12/26-tedwu, for power consumption and avoid ESD issue on 1-wire pin?.
#if defined(ARIMA_PROJECT_HAWK35)
	bat_id_deinit_pin();
#endif
//>2012/12/26-tedwu
  BATIDDB("[BATTERY:bq24158] Read_Battery_Info End ---------------------------\n");
}

extern kal_bool upmu_is_chr_det(void);

//<2012/02/07-2950-jessicatseng, Just check page4 for MOTO battery ID checked
kal_bool Is_MOTO_Battery(void)
{
	kal_bool isValid = KAL_TRUE;
	//char Family_Code, page2_CheckSum, page3_CheckSum;
	//char page4_Str_MOTO[] = "COPR2011MOTOROLA E.P CHARGE ONLY";
	//char page4_Str[32+1];
	//int  Custom_ID;

	BATIDDB("[BATTERY:bq24158] Is_MOTO_Battery, Battery_ID_read_count = %d\n", Battery_ID_read_count);		
	if(upmu_is_chr_det() == KAL_TRUE)
	{
		//Family_Code = rom_data[0];
		//Custom_ID = (int)(((rom_data[6]<<8)|(rom_data[5]&0xF0))>>4);
		//page2_CheckSum = memory_data[0x20];
		//page3_CheckSum = memory_data[0x40];
		//memset(page4_Str, 0x00, sizeof(page4_Str));
		//strncpy(page4_Str, &(memory_data[0x60]), 32);
		//strncpy(page4_Str, &(memory_data[0x68]), 24);
		
		//BATIDDB("[BATTERY:bq24158] Is_MOTO_Battery\n");
		//BATIDDB("Family_Code = %x, Custom_ID = %x, page2_CheckSum = %x, page3_CheckSum = %x\n", \
						//Family_Code, Custom_ID, page2_CheckSum, page3_CheckSum);
		//BATIDDB("page4_Str = %s\n", page4_Str);					
		
		if((Battery_ID_read_count <= BATTERY_CHECK_MAX_NUM) && (Is_battery_valid == KAL_FALSE))
		{
           #if defined(BATTERY_PACK_HW4X)
			if((Family_Code == 0x89) && (Max_Voltage == 0x9E) && 
				(((page2_CheckSum == 0xD3) && (page3_CheckSum == 0xDC)) /*|| 
				 ((page2_CheckSum == 0x66) && (page3_CheckSum == 0x83)) ||
				 ((page2_CheckSum == 0x75) && (page3_CheckSum == 0xde)) ||
				 ((page2_CheckSum == 0x75) && (page3_CheckSum == 0x90)) */)
				&& (Custom_ID == 0x500) && (strncmp(&(page4_Str_MOTO[8]), &(page4_Str[8]), 24) == 0))
					#elif defined(BATTERY_PACK_EG30)
			if((Family_Code == 0x89) && (Max_Voltage == 0x9E) && 
				(((page2_CheckSum == 0xd2) && (page3_CheckSum == 0x71)) ||
				 ((page2_CheckSum == 0xd4) && (page3_CheckSum == 0x5a)))
				&& (Custom_ID == 0x500) && (strncmp(&(page4_Str_MOTO[8]), &(page4_Str[8]), 24) == 0))
//>2012/10/03-14605-jessicatseng
            #else
			if((strncmp(&(page4_Str_MOTO[0]), &(page4_Str[0]), 4) == 0)
					&& (strncmp(&(page4_Str_MOTO[8]), &(page4_Str[8]), 24) == 0))
					#endif
			{
				//isValid = KAL_TRUE;
				Is_battery_valid = KAL_TRUE;
			}		
			else
			{
				Read_Battery_Info();
				Battery_ID_read_count ++;
			}
		}
		else
		{
			isValid = Is_battery_valid;
		}

		BATIDDB("[BATTERY:bq24158] Is_MOTO_Battery %d, %d page4_Str = %s\n",Is_battery_valid, isValid, page4_Str);
	}	
		
	return isValid;			
}

//<2012/10/18-15340-jessicatseng, Show warning dialog when getting "invalid battery" and "partial charging" in power on mode
kal_bool Is_Invalid_Battery(void)
{
	if((Is_battery_valid == KAL_FALSE) && (Battery_ID_read_count > BATTERY_CHECK_MAX_NUM))
			return KAL_FALSE;
	
	return KAL_TRUE;	
}	
//>2012/10/18-15340-jessicatseng

//<2012/11/22-17513-jessicatseng, [Hawk 4.0] Add ZCV table for LG cell
int Get_MOTO_Battery_Vender(void)
{
	if((page2_CheckSum == 0xd4) && (page3_CheckSum == 0x5a))
		return 0x01;		
	else
		return 0x00;	
}
//>2012/11/22-17513-jessicatseng
#endif
//>2012/01/02-2106-jessicatseng
//>2012/02/07-2950-jessicatseng
//<2012/03/28-4645-jessicatseng
//>2012/7/9-tedwu

//>2012/10/05-14765-jessicatseng
