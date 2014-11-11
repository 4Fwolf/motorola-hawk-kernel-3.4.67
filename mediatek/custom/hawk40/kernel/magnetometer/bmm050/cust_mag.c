#include <linux/types.h>
//<2012/09/26, SunnyHu, modify for build error in android jelly bean
//#include <mach/mt6577_pm_ldo.h>
#include <mach/mt_pm_ldo.h>
//>2012/09/26, SunnyHu, modify for build error in android jelly bean
#include <cust_mag.h>


static struct mag_hw cust_mag_hw = {
    .i2c_num = 0,
    .direction = 1,
    .power_id = MT65XX_POWER_NONE,  /*!< LDO is not used */
    .power_vol= VOL_DEFAULT,        /*!< LDO is not used */
};
struct mag_hw* get_cust_mag_hw(void) 
{
    return &cust_mag_hw;
}
