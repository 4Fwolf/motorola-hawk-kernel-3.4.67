//<2012/10/05-14765-jessicatseng, [Hawk 4.0] Enable to check MOTO battery ID in power on mode
#ifndef _BAT_ID_H_
#define _BAT_ID_H_

#if defined(ARIMA_PROJECT_HAWK40)
#define CHK_BATTERY_ID__MOTO_ID
#define BATTERY_PACK_EG30
//<2012/11/06-tedwu, Enable battery ID checking feature.
#elif defined(ARIMA_PROJECT_HAWK35)
#define CHK_BATTERY_ID__MOTO_ID
#define BATTERY_PACK_HW4X
//>2012/11/06-tedwu
#endif

//<2012/10/26-15796-jessicatseng, [Hawk 4.0] add "spin_lock" function when reading battery ID
void bat_id_init_spinLock(void);
//>2012/10/26-15796-jessicatseng
void bat_id_init_pin(void);
void bat_id_init_variable(void);
void Read_Battery_Info(void);
kal_bool Is_MOTO_Battery(void);
//<2012/10/18-15340-jessicatseng, Show warning dialog when getting "invalid battery" and "partial charging" in power on mode
kal_bool Is_Invalid_Battery(void);
//>2012/10/18-15340-jessicatseng
//<2012/11/22-17513-jessicatseng, [Hawk 4.0] Add ZCV table for LG cell
int Get_MOTO_Battery_Vender(void);
//>2012/11/22-17513-jessicatseng
#endif //ifndef _BAT_ID_H_
//>2012/10/05-14765-jessicatseng
