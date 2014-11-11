#include <linux/types.h>
#include <cust_acc.h>
//<2012/09/26, SunnyHu, modify for build error in android jelly bean
//#include <mach/mt6577_pm_ldo.h>
#include <mach/mt_pm_ldo.h>
//>2012/09/26, SunnyHu, modify for build error in android jelly bean

/*
// copy from argonmini, to modify if needed
int cust_acc_power(struct acc_hw *hw, unsigned int on, char* devname)
{
    if (hw->power_id == MT65XX_POWER_NONE)
        return 0;

    if (on)
    {
        hwPowerOn(MT65XX_POWER_LDO_VIO28, VOL_2800,devname);

        return hwPowerOn(hw->power_id, hw->power_vol, devname);
    }
    else
        return hwPowerDown(hw->power_id, devname); 
}
*/
/*---------------------------------------------------------------------------*/
static struct acc_hw cust_acc_hw = {
    .i2c_num = 0,
    .direction = 1,
    .power_id = MT65XX_POWER_NONE,  /*!< LDO is not used */
    .power_vol= VOL_DEFAULT,        /*!< LDO is not used */
    .firlen = 5,                   /*!< don't enable low pass fileter */
    //.power = cust_acc_power,
};
/*---------------------------------------------------------------------------*/
struct acc_hw* get_cust_acc_hw(void) 
{
    return &cust_acc_hw;
}
