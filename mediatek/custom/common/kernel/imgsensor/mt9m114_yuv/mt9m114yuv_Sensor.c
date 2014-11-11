/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   mt9m114yuv_sensor.c
 *
 * Project:
 * --------
 *   ALPS
 *
 * Description:
 * ------------
 *   Source code of Sensor driver
 *
 *
 * Author:
 * -------
 * 
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Revision:$
 * $Modtime:$
 * $Log:$
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"

#include "mt9m114yuv_Sensor.h"
#include "mt9m114yuv_Camera_Sensor_para.h"
#include "mt9m114yuv_CameraCustomized.h"

#define MT9M114YUV_DEBUG
#ifdef MT9M114YUV_DEBUG
	#define SENSORDB(fmt, arg...) printk( "[MT9M114YUV] "  fmt, ##arg)
#else
	#define SENSORDB(x,...)
#endif

typedef enum {
    SENSOR_MODE_INIT = 0,
    SENSOR_MODE_PREVIEW,
    SENSOR_MODE_CAPTURE
}MT9M114_SENSOR_MODE;

typedef struct {
    kal_uint16 pvPCLK; // x10 240 for 24 MHZ 
    kal_uint16 capPCLK; // x10

    kal_uint16 iFrameRate;
    kal_uint16 iNightMode;
    kal_uint16 iWB;
    kal_uint16 iEffect;
    kal_uint16 iEV;
    kal_uint16 iBanding;
    kal_uint16 iMirror;

    MT9M114_SENSOR_MODE sensorMode;
}MT9M114_PARA_STRUCT, *PMT9M114_PARA_STRUCT; 


MSDK_SCENARIO_ID_ENUM MT9M114_CurrentScenarioId = MSDK_SCENARIO_ID_CAMERA_PREVIEW;
MT9M114_PARA_STRUCT MT9M114_para;
MSDK_SENSOR_CONFIG_STRUCT MT9M114_SensorConfigData;

//<2012/09/28-14462-alberthsiao,update mt9m114 driver for hawk40
u16 MT9M114_READ_ID = MT9M114_READ_ID_1;
u16 MT9M114_WRITE_ID = MT9M114_WRITE_ID_1;
//>2012/09/28-14462

static DEFINE_SPINLOCK(mt9m114yuv_drv_lock);

extern int iReadRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u8 * a_pRecvData, u16 a_sizeRecvData, u16 i2cId);
extern int iWriteRegI2C(u8 *a_pSendData , u16 a_sizeSendData, u16 i2cId);

kal_uint16 MT9M114_read_cmos_sensor(kal_uint32 addr)
{
    kal_uint16 get_byte = 0;
    char puSendCmd[2] = {(char)(addr >> 8), (char)(addr & 0xFF)};
    iReadRegI2C(puSendCmd, 2, (u8*)&get_byte, 2, MT9M114_READ_ID);
    return ((get_byte << 8) & 0xFF00) | ((get_byte >> 8) & 0x00FF);
}

void MT9M114_write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
    char puSendCmd[4] = {(char)(addr >> 8), (char)(addr & 0xFF), (char)(para >> 8), (char)(para & 0xFF)};
    iWriteRegI2C(puSendCmd, 4, MT9M114_WRITE_ID);
}

kal_uint16 MT9M114_read_cmos_sensor_8(kal_uint32 addr)
{
    kal_uint16 get_byte = 0;
    char puSendCmd[2] = {(char)(addr >> 8), (char)(addr & 0xFF)};
    iReadRegI2C(puSendCmd, 2, (u8*)&get_byte, 1, MT9M114_READ_ID);
    return get_byte;
}

void MT9M114_write_cmos_sensor_8(kal_uint32 addr, kal_uint32 para)
{
    char puSendCmd[4] = {(char)(addr >> 8), (char)(addr & 0xFF), (char)(para & 0xFF)};
    iWriteRegI2C(puSendCmd, 3, MT9M114_WRITE_ID);
}

void MT9M114_change_config_command()
{
    kal_uint16 command_register_value, time = 0;
    kal_uint16 change_config_shift = 1, change_config_mask = 0x0002;
    kal_uint16 command_ok_shift = 15, command_ok_mask = 0x8000;

    command_register_value = MT9M114_read_cmos_sensor(0x0080);
    
    if ((command_register_value & change_config_mask) >> change_config_shift != 0)
    {
        SENSORDB("[Error]: State change cmd bit is already set!! \n");
        return;
    }

    MT9M114_write_cmos_sensor(0x098E, 0xDC00);		// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xDC00, 0x28);		// sysmgr_next_state
    MT9M114_write_cmos_sensor(0x0080, 0x8002);		// COMMAND_REGISTER

    // wait HOST_COMMAND_1
    time = 0;
    do {
        command_register_value = MT9M114_read_cmos_sensor(0x0080);
        if ((command_register_value & change_config_mask) >> change_config_shift == 0)
            break;
        mDELAY(10);
        time++;
    } while (time < 10);

    // check HOST_COMMAND_OK
    if ((command_register_value & command_ok_mask) >> command_ok_shift != 1)
    {
        SENSORDB("[Error]: Change-Config failed!! \n");
        return;
    }
}

void MT9M114_wait_command_0()
{
    kal_uint16 command_register_value, time = 0;

    // wait HOST_COMMAND_0
    time = 0;
    do {
        command_register_value = MT9M114_read_cmos_sensor(0x0080);

        // time out 100 ms
        time += 10;
        if (time > 100)
            break;
            
        mDELAY(10);
    } while (command_register_value & 0x0001 != 0x0000);
}



kal_uint16 MT9M114GetSensorID(void)
{
    kal_uint16 sensor_id = 0xFFFF;

    sensor_id = MT9M114_read_cmos_sensor(0x0000);
//<2012/09/28-14462-alberthsiao,update mt9m114 driver for hawk40
	if (sensor_id != MT9M114_SENSOR_ID)
	{
		MT9M114_READ_ID = MT9M114_READ_ID_2;
		MT9M114_WRITE_ID = MT9M114_WRITE_ID_2;
		sensor_id = MT9M114_read_cmos_sensor(0x0000);
	}
//>2012/09/28-14462
    return sensor_id;
}


/*************************************************************************
* FUNCTION
*   MT9M114CheckSensorID
*
* DESCRIPTION
*   This function get the sensor ID 
*
* PARAMETERS
*   *sensorID : return the sensor ID 
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 MT9M114CheckSensorID(UINT32 *sensorID)
{
    int  retry = 3;

    //SENSORDB("[Enter]: %s \n", __FUNCTION__);
    
    // check if sensor ID correct
    do {
        *sensorID = MT9M114GetSensorID();
        
        if (*sensorID == MT9M114_SENSOR_ID)
        {
            SENSORDB("MT9M114CheckSensorID SensorID = 0x%04x \n", __FUNCTION__, *sensorID);
            break; 
        }
        SENSORDB("Read Sensor ID Fail = 0x%04x\n", *sensorID); 
        retry--; 
    } while (retry > 0);

    if (*sensorID != MT9M114_SENSOR_ID) {
        *sensorID = 0xFFFFFFFF; 
        return ERROR_SENSOR_CONNECT_FAIL;
    }

    //SENSORDB("[Exit]: %s \n", __FUNCTION__);
    
    return ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*	MT9M114InitialPara
*
* DESCRIPTION
*	This function initialize the global status of  MT9M114
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void MT9M114InitialPara(void)
{
  /*Initial status setting 
   Can be better by sync with MT9M114InitialSetting*/
  spin_lock(&mt9m114yuv_drv_lock);
  MT9M114_para.iNightMode = 0xFFFF;
  MT9M114_para.iWB = AWB_MODE_AUTO;
  MT9M114_para.iEffect = MEFFECT_OFF;
  MT9M114_para.iBanding = AE_FLICKER_MODE_50HZ;
  MT9M114_para.iEV = AE_EV_COMP_00;
  MT9M114_para.iMirror = IMAGE_NORMAL;
  MT9M114_para.iFrameRate = 0; //No Fix FrameRate

  MT9M114_para.sensorMode = SENSOR_MODE_INIT;
  spin_unlock(&mt9m114yuv_drv_lock);
}

//<2012/11/28 alberthsiao-17860,camera quality tuning for Hawk40
/*************************************************************************
* FUNCTION
*   MT9M114InitialSetting
*
* DESCRIPTION
*   This function set the sensor initial register setting.
*
* PARAMETERS
*  
* 
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 MT9M114InitialSetting()
{
    SENSORDB("[Enter]: %s \n", __FUNCTION__);
#ifdef NONE_MTK_CAMERA_SETTING
    if(MT9M114_READ_ID == MT9M114_READ_ID_1)
        MT9M114_LiteOn_InitialSetting();
    else
        MT9M114_LiteOn_InitialSetting();
#else
    // soft reset
    MT9M114_write_cmos_sensor(0x001A, 0x0001);     // RESET_AND_MISC_CONTROL
    MT9M114_write_cmos_sensor(0x001A, 0x0000);     // RESET_AND_MISC_CONTROL
    MT9M114_write_cmos_sensor(0x301A, 0x8234);     // RESET_REGISTER

    // must delay min 44.5ms
    mDELAY(50);

    // PLL Setting
    MT9M114_write_cmos_sensor(0x098E, 0x1000);      // LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor(0xC97E, 0x01BA);		//cam_sysctl_pll_enable = 1
    MT9M114_write_cmos_sensor(0xC980, 0x0120);		//cam_sysctl_pll_divider_m_n = 288
    MT9M114_write_cmos_sensor(0xC982, 0x0700);		//cam_sysctl_pll_divider_p = 1792
    MT9M114_write_cmos_sensor(0xC984, 0x8000);		//cam_port_output_control = 32768

    //Startx/y  Endx/y
    MT9M114_write_cmos_sensor(0xC800, 0x0004);		//cam_sensor_cfg_y_addr_start = 4
    MT9M114_write_cmos_sensor(0xC802, 0x0004);		//cam_sensor_cfg_x_addr_start = 4
    MT9M114_write_cmos_sensor(0xC804, 0x03CB);		//cam_sensor_cfg_y_addr_end = 971
    MT9M114_write_cmos_sensor(0xC806, 0x050B);		//cam_sensor_cfg_x_addr_end = 1291

    //FixMe:
    //pixclk 48MHZ
    MT9M114_write_cmos_sensor(0xC808, 0x02DC);		//cam_sensor_cfg_pixclk = 48000000
    MT9M114_write_cmos_sensor(0xC80A, 0x6C00);

    MT9M114_write_cmos_sensor(0xC80C, 0x0001);		//cam_sensor_cfg_row_speed = 1
    MT9M114_write_cmos_sensor(0xC80E, 0x00DB);		//cam_sensor_cfg_fine_integ_time_min = 219
    MT9M114_write_cmos_sensor(0xC810, 0x05C1);		//cam_sensor_cfg_fine_integ_time_max = 1637
    
    MT9M114_write_cmos_sensor(0xC812, 0x03F3);		//cam_sensor_cfg_frame_length_lines = 1011
    MT9M114_write_cmos_sensor(0xC814, 0x0644);		//cam_sensor_cfg_line_length_pck = 1604
    
    MT9M114_write_cmos_sensor(0xC816, 0x0060);		//cam_sensor_cfg_fine_correction = 96
    MT9M114_write_cmos_sensor(0xC818, 0x03C3);		//cam_sensor_cfg_cpipe_last_row = 963
    MT9M114_write_cmos_sensor(0xC826, 0x0020);		//cam_sensor_cfg_reg_0_data = 32

    //camera read mode
    //vertical normal, horizontal normal, no flip and no mirror
    MT9M114_write_cmos_sensor(0xC834, 0x0000);		//cam_sensor_control_read_mode = 0

    // crop window
    MT9M114_write_cmos_sensor(0xC854, 0x0000);		//cam_crop_window_xoffset = 0
    MT9M114_write_cmos_sensor(0xC856, 0x0000);		//cam_crop_window_yoffset = 0
    MT9M114_write_cmos_sensor(0xC858, 0x0500);		//cam_crop_window_width = 1280
    MT9M114_write_cmos_sensor(0xC85A, 0x03C0);		//cam_crop_window_height = 960

    // AWB and AE stats window auto resize
    MT9M114_write_cmos_sensor(0xC85C, 0x0342);		//cam_crop_cropmode = 3

    // output size
    MT9M114_write_cmos_sensor(0xC868, 0x0500);		//cam_output_width = 1280
    MT9M114_write_cmos_sensor(0xC86A, 0x03C0);		//cam_output_height = 960

    // AE target setting
    MT9M114_write_cmos_sensor(0xC878, 0x0001);		//cam_aet_aemode = 0
    MT9M114_write_cmos_sensor(0xC88C, 0x1D99);		//cam_aet_max_frame_rate = 4754
    MT9M114_write_cmos_sensor(0xC88E, 0x0F00);		//cam_aet_min_frame_rate = 4754

    // AWB and AE window
    MT9M114_write_cmos_sensor(0xC914, 0x0000);		//cam_stat_awb_clip_window_xstart = 0
    MT9M114_write_cmos_sensor(0xC916, 0x0000);		//cam_stat_awb_clip_window_ystart = 0
    MT9M114_write_cmos_sensor(0xC918, 0x04FF);		//cam_stat_awb_clip_window_xend = 1279
    MT9M114_write_cmos_sensor(0xC91A, 0x03BF);		//cam_stat_awb_clip_window_yend = 959
    
    MT9M114_write_cmos_sensor(0xC91C, 0x0000);		//cam_stat_ae_initial_window_xstart = 0
    MT9M114_write_cmos_sensor(0xC91E, 0x0000);		//cam_stat_ae_initial_window_ystart = 0
    MT9M114_write_cmos_sensor(0xC920, 0x00FF);		//cam_stat_ae_initial_window_xend = 255
    MT9M114_write_cmos_sensor(0xC922, 0x00BF);		//cam_stat_ae_initial_window_yend = 191
    
    // awb pixel threshold count
    MT9M114_write_cmos_sensor(0xAC18, 0x0000);
    MT9M114_write_cmos_sensor(0xAC1A, 0x0100);
    
    ///RECOMMAND
    MT9M114_write_cmos_sensor(0x316A, 0x82F0);     // DAC_TXLO_ROW
    MT9M114_write_cmos_sensor(0x316C, 0x82F0);     // DAC_TXLO
    MT9M114_write_cmos_sensor(0x3ED0, 0x5705);     // DAC_LD_4_5
    MT9M114_write_cmos_sensor(0x3180, 0x87FF);     // DELTA_DK_CONTROL
    MT9M114_write_cmos_sensor(0xA802, 0x0008);     // AE_TRACK_MODE
    MT9M114_write_cmos_sensor(0x3E14, 0xFF39);     // SAMP_COL_PUP2
    MT9M114_write_cmos_sensor(0x301A, 0x0234);     // RESET_REGISTER

    MT9M114_write_cmos_sensor(0x0982, 0x0001);     // ACCESS_CTL_STAT
    MT9M114_write_cmos_sensor(0x098A, 0x5000); 	   // PHYSICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor(0xD000, 0x70CF);
    MT9M114_write_cmos_sensor(0xD002, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD004, 0xC5D4);
    MT9M114_write_cmos_sensor(0xD006, 0x903A);
    MT9M114_write_cmos_sensor(0xD008, 0x2144);
    MT9M114_write_cmos_sensor(0xD00A, 0x0C00);
    MT9M114_write_cmos_sensor(0xD00C, 0x2186);
    MT9M114_write_cmos_sensor(0xD00E, 0x0FF3);
    MT9M114_write_cmos_sensor(0xD010, 0xB844);
    MT9M114_write_cmos_sensor(0xD012, 0xB948);
    MT9M114_write_cmos_sensor(0xD014, 0xE082);
    MT9M114_write_cmos_sensor(0xD016, 0x20CC);
    MT9M114_write_cmos_sensor(0xD018, 0x80E2);
    MT9M114_write_cmos_sensor(0xD01A, 0x21CC);
    MT9M114_write_cmos_sensor(0xD01C, 0x80A2);
    MT9M114_write_cmos_sensor(0xD01E, 0x21CC);
    MT9M114_write_cmos_sensor(0xD020, 0x80E2);
    MT9M114_write_cmos_sensor(0xD022, 0xF404);
    MT9M114_write_cmos_sensor(0xD024, 0xD801);
    MT9M114_write_cmos_sensor(0xD026, 0xF003);
    MT9M114_write_cmos_sensor(0xD028, 0xD800);
    MT9M114_write_cmos_sensor(0xD02A, 0x7EE0);
    MT9M114_write_cmos_sensor(0xD02C, 0xC0F1);
    MT9M114_write_cmos_sensor(0xD02E, 0x08BA);
    MT9M114_write_cmos_sensor(0xD030, 0x0600);
    MT9M114_write_cmos_sensor(0xD032, 0xC1A1);
    MT9M114_write_cmos_sensor(0xD034, 0x76CF);
    MT9M114_write_cmos_sensor(0xD036, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD038, 0xC130);
    MT9M114_write_cmos_sensor(0xD03A, 0x6E04);
    MT9M114_write_cmos_sensor(0xD03C, 0xC040);
    MT9M114_write_cmos_sensor(0xD03E, 0x71CF);
    MT9M114_write_cmos_sensor(0xD040, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD042, 0xC790);
    MT9M114_write_cmos_sensor(0xD044, 0x8103);
    MT9M114_write_cmos_sensor(0xD046, 0x77CF);
    MT9M114_write_cmos_sensor(0xD048, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD04A, 0xC7C0);
    MT9M114_write_cmos_sensor(0xD04C, 0xE001);
    MT9M114_write_cmos_sensor(0xD04E, 0xA103);
    MT9M114_write_cmos_sensor(0xD050, 0xD800);
    MT9M114_write_cmos_sensor(0xD052, 0x0C6A);
    MT9M114_write_cmos_sensor(0xD054, 0x04E0);
    MT9M114_write_cmos_sensor(0xD056, 0xB89E);
    MT9M114_write_cmos_sensor(0xD058, 0x7508);
    MT9M114_write_cmos_sensor(0xD05A, 0x8E1C);
    MT9M114_write_cmos_sensor(0xD05C, 0x0809);
    MT9M114_write_cmos_sensor(0xD05E, 0x0191);
    MT9M114_write_cmos_sensor(0xD060, 0xD801);
    MT9M114_write_cmos_sensor(0xD062, 0xAE1D);
    MT9M114_write_cmos_sensor(0xD064, 0xE580);
    MT9M114_write_cmos_sensor(0xD066, 0x20CA);
    MT9M114_write_cmos_sensor(0xD068, 0x0022);
    MT9M114_write_cmos_sensor(0xD06A, 0x20CF);
    MT9M114_write_cmos_sensor(0xD06C, 0x0522);
    MT9M114_write_cmos_sensor(0xD06E, 0x0C5C);
    MT9M114_write_cmos_sensor(0xD070, 0x04E2);
    MT9M114_write_cmos_sensor(0xD072, 0x21CA);
    MT9M114_write_cmos_sensor(0xD074, 0x0062);
    MT9M114_write_cmos_sensor(0xD076, 0xE580);
    MT9M114_write_cmos_sensor(0xD078, 0xD901);
    MT9M114_write_cmos_sensor(0xD07A, 0x79C0);
    MT9M114_write_cmos_sensor(0xD07C, 0xD800);
    MT9M114_write_cmos_sensor(0xD07E, 0x0BE6);
    MT9M114_write_cmos_sensor(0xD080, 0x04E0);
    MT9M114_write_cmos_sensor(0xD082, 0xB89E);
    MT9M114_write_cmos_sensor(0xD084, 0x70CF);
    MT9M114_write_cmos_sensor(0xD086, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD088, 0xC8D4);
    MT9M114_write_cmos_sensor(0xD08A, 0x9002);
    MT9M114_write_cmos_sensor(0xD08C, 0x0857);
    MT9M114_write_cmos_sensor(0xD08E, 0x025E);
    MT9M114_write_cmos_sensor(0xD090, 0xFFDC);
    MT9M114_write_cmos_sensor(0xD092, 0xE080);
    MT9M114_write_cmos_sensor(0xD094, 0x25CC);
    MT9M114_write_cmos_sensor(0xD096, 0x9022);
    MT9M114_write_cmos_sensor(0xD098, 0xF225);
    MT9M114_write_cmos_sensor(0xD09A, 0x1700);
    MT9M114_write_cmos_sensor(0xD09C, 0x108A);
    MT9M114_write_cmos_sensor(0xD09E, 0x73CF);
    MT9M114_write_cmos_sensor(0xD0A0, 0xFF00);
    MT9M114_write_cmos_sensor(0xD0A2, 0x3174);
    MT9M114_write_cmos_sensor(0xD0A4, 0x9307);
    MT9M114_write_cmos_sensor(0xD0A6, 0x2A04);
    MT9M114_write_cmos_sensor(0xD0A8, 0x103E);
    MT9M114_write_cmos_sensor(0xD0AA, 0x9328);
    MT9M114_write_cmos_sensor(0xD0AC, 0x2942);
    MT9M114_write_cmos_sensor(0xD0AE, 0x7140);
    MT9M114_write_cmos_sensor(0xD0B0, 0x2A04);
    MT9M114_write_cmos_sensor(0xD0B2, 0x107E);
    MT9M114_write_cmos_sensor(0xD0B4, 0x9349);
    MT9M114_write_cmos_sensor(0xD0B6, 0x2942);
    MT9M114_write_cmos_sensor(0xD0B8, 0x7141);
    MT9M114_write_cmos_sensor(0xD0BA, 0x2A04);
    MT9M114_write_cmos_sensor(0xD0BC, 0x10BE);
    MT9M114_write_cmos_sensor(0xD0BE, 0x934A);
    MT9M114_write_cmos_sensor(0xD0C0, 0x2942);
    MT9M114_write_cmos_sensor(0xD0C2, 0x714B);
    MT9M114_write_cmos_sensor(0xD0C4, 0x2A04);
    MT9M114_write_cmos_sensor(0xD0C6, 0x10BE);
    MT9M114_write_cmos_sensor(0xD0C8, 0x130C);
    MT9M114_write_cmos_sensor(0xD0CA, 0x010A);
    MT9M114_write_cmos_sensor(0xD0CC, 0x2942);
    MT9M114_write_cmos_sensor(0xD0CE, 0x7142);
    MT9M114_write_cmos_sensor(0xD0D0, 0x2250);
    MT9M114_write_cmos_sensor(0xD0D2, 0x13CA);
    MT9M114_write_cmos_sensor(0xD0D4, 0x1B0C);
    MT9M114_write_cmos_sensor(0xD0D6, 0x0284);
    MT9M114_write_cmos_sensor(0xD0D8, 0xB307);
    MT9M114_write_cmos_sensor(0xD0DA, 0xB328);
    MT9M114_write_cmos_sensor(0xD0DC, 0x1B12);
    MT9M114_write_cmos_sensor(0xD0DE, 0x02C4);
    MT9M114_write_cmos_sensor(0xD0E0, 0xB34A);
    MT9M114_write_cmos_sensor(0xD0E2, 0xED88);
    MT9M114_write_cmos_sensor(0xD0E4, 0x71CF);
    MT9M114_write_cmos_sensor(0xD0E6, 0xFF00);
    MT9M114_write_cmos_sensor(0xD0E8, 0x3174);
    MT9M114_write_cmos_sensor(0xD0EA, 0x9106);
    MT9M114_write_cmos_sensor(0xD0EC, 0xB88F);
    MT9M114_write_cmos_sensor(0xD0EE, 0xB106);
    MT9M114_write_cmos_sensor(0xD0F0, 0x210A);
    MT9M114_write_cmos_sensor(0xD0F2, 0x8340);
    MT9M114_write_cmos_sensor(0xD0F4, 0xC000);
    MT9M114_write_cmos_sensor(0xD0F6, 0x21CA);
    MT9M114_write_cmos_sensor(0xD0F8, 0x0062);
    MT9M114_write_cmos_sensor(0xD0FA, 0x20F0);
    MT9M114_write_cmos_sensor(0xD0FC, 0x0040);
    MT9M114_write_cmos_sensor(0xD0FE, 0x0B02);
    MT9M114_write_cmos_sensor(0xD100, 0x0320);
    MT9M114_write_cmos_sensor(0xD102, 0xD901);
    MT9M114_write_cmos_sensor(0xD104, 0x07F1);
    MT9M114_write_cmos_sensor(0xD106, 0x05E0);
    MT9M114_write_cmos_sensor(0xD108, 0xC0A1);
    MT9M114_write_cmos_sensor(0xD10A, 0x78E0);
    MT9M114_write_cmos_sensor(0xD10C, 0xC0F1);
    MT9M114_write_cmos_sensor(0xD10E, 0x71CF);
    MT9M114_write_cmos_sensor(0xD110, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD112, 0xC7C0);
    MT9M114_write_cmos_sensor(0xD114, 0xD840);
    MT9M114_write_cmos_sensor(0xD116, 0xA900);
    MT9M114_write_cmos_sensor(0xD118, 0x71CF);
    MT9M114_write_cmos_sensor(0xD11A, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD11C, 0xD02C);
    MT9M114_write_cmos_sensor(0xD11E, 0xD81E);
    MT9M114_write_cmos_sensor(0xD120, 0x0A5A);
    MT9M114_write_cmos_sensor(0xD122, 0x04E0);
    MT9M114_write_cmos_sensor(0xD124, 0xDA00);
    MT9M114_write_cmos_sensor(0xD126, 0xD800);
    MT9M114_write_cmos_sensor(0xD128, 0xC0D1);
    MT9M114_write_cmos_sensor(0xD12A, 0x7EE0);

    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor(0xE000, 0x010C);	// PATCHLDR_LOADER_ADDRESS
    MT9M114_write_cmos_sensor(0xE002, 0x0202); 	// PATCHLDR_PATCH_ID
    // FixMe
    MT9M114_write_cmos_sensor(0xE004, 0x4103); 	// PATCHLDR_FIRMWARE_ID
    MT9M114_write_cmos_sensor(0xE006, 0x0202);

    MT9M114_write_cmos_sensor(0x0080, 0xFFF0); 	// COMMAND_REGISTER
    //mDELAY(100);
    //  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00
    MT9M114_write_cmos_sensor(0x0080, 0xFFF1); 	// COMMAND_REGISTER

    //mDELAY(100);
    MT9M114_wait_command_0();
    //  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00
    
    MT9M114_write_cmos_sensor(0x0982, 0x0001); 	// ACCESS_CTL_STAT
    MT9M114_write_cmos_sensor(0x098A, 0x512C); 	// PHYSICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor(0xD12C, 0x70CF);
    MT9M114_write_cmos_sensor(0xD12E, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD130, 0xC5D4);
    MT9M114_write_cmos_sensor(0xD132, 0x903A);
    MT9M114_write_cmos_sensor(0xD134, 0x2144);
    MT9M114_write_cmos_sensor(0xD136, 0x0C00);
    MT9M114_write_cmos_sensor(0xD138, 0x2186);
    MT9M114_write_cmos_sensor(0xD13A, 0x0FF3);
    MT9M114_write_cmos_sensor(0xD13C, 0xB844);
    MT9M114_write_cmos_sensor(0xD13E, 0x262F);
    MT9M114_write_cmos_sensor(0xD140, 0xF008);
    MT9M114_write_cmos_sensor(0xD142, 0xB948);
    MT9M114_write_cmos_sensor(0xD144, 0x21CC);
    MT9M114_write_cmos_sensor(0xD146, 0x8021);
    MT9M114_write_cmos_sensor(0xD148, 0xD801);
    MT9M114_write_cmos_sensor(0xD14A, 0xF203);
    MT9M114_write_cmos_sensor(0xD14C, 0xD800);
    MT9M114_write_cmos_sensor(0xD14E, 0x7EE0);
    MT9M114_write_cmos_sensor(0xD150, 0xC0F1);
    MT9M114_write_cmos_sensor(0xD152, 0x71CF);
    MT9M114_write_cmos_sensor(0xD154, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD156, 0xC610);
    MT9M114_write_cmos_sensor(0xD158, 0x910E);
    MT9M114_write_cmos_sensor(0xD15A, 0x208C);
    MT9M114_write_cmos_sensor(0xD15C, 0x8014);
    MT9M114_write_cmos_sensor(0xD15E, 0xF418);
    MT9M114_write_cmos_sensor(0xD160, 0x910F);
    MT9M114_write_cmos_sensor(0xD162, 0x208C);
    MT9M114_write_cmos_sensor(0xD164, 0x800F);
    MT9M114_write_cmos_sensor(0xD166, 0xF414);
    MT9M114_write_cmos_sensor(0xD168, 0x9116);
    MT9M114_write_cmos_sensor(0xD16A, 0x208C);
    MT9M114_write_cmos_sensor(0xD16C, 0x800A);
    MT9M114_write_cmos_sensor(0xD16E, 0xF410);
    MT9M114_write_cmos_sensor(0xD170, 0x9117);
    MT9M114_write_cmos_sensor(0xD172, 0x208C);
    MT9M114_write_cmos_sensor(0xD174, 0x8807);
    MT9M114_write_cmos_sensor(0xD176, 0xF40C);
    MT9M114_write_cmos_sensor(0xD178, 0x9118);
    MT9M114_write_cmos_sensor(0xD17A, 0x2086);
    MT9M114_write_cmos_sensor(0xD17C, 0x0FF3);
    MT9M114_write_cmos_sensor(0xD17E, 0xB848);
    MT9M114_write_cmos_sensor(0xD180, 0x080D);
    MT9M114_write_cmos_sensor(0xD182, 0x0090);
    MT9M114_write_cmos_sensor(0xD184, 0xFFEA);
    MT9M114_write_cmos_sensor(0xD186, 0xE081);
    MT9M114_write_cmos_sensor(0xD188, 0xD801);
    MT9M114_write_cmos_sensor(0xD18A, 0xF203);
    MT9M114_write_cmos_sensor(0xD18C, 0xD800);
    MT9M114_write_cmos_sensor(0xD18E, 0xC0D1);
    MT9M114_write_cmos_sensor(0xD190, 0x7EE0);
    MT9M114_write_cmos_sensor(0xD192, 0x78E0);
    MT9M114_write_cmos_sensor(0xD194, 0xC0F1);
    MT9M114_write_cmos_sensor(0xD196, 0x71CF);
    MT9M114_write_cmos_sensor(0xD198, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD19A, 0xC610);
    MT9M114_write_cmos_sensor(0xD19C, 0x910E);
    MT9M114_write_cmos_sensor(0xD19E, 0x208C);
    MT9M114_write_cmos_sensor(0xD1A0, 0x800A);
    MT9M114_write_cmos_sensor(0xD1A2, 0xF418);
    MT9M114_write_cmos_sensor(0xD1A4, 0x910F);
    MT9M114_write_cmos_sensor(0xD1A6, 0x208C);
    MT9M114_write_cmos_sensor(0xD1A8, 0x8807);
    MT9M114_write_cmos_sensor(0xD1AA, 0xF414);
    MT9M114_write_cmos_sensor(0xD1AC, 0x9116);
    MT9M114_write_cmos_sensor(0xD1AE, 0x208C);
    MT9M114_write_cmos_sensor(0xD1B0, 0x800A);
    MT9M114_write_cmos_sensor(0xD1B2, 0xF410);
    MT9M114_write_cmos_sensor(0xD1B4, 0x9117);
    MT9M114_write_cmos_sensor(0xD1B6, 0x208C);
    MT9M114_write_cmos_sensor(0xD1B8, 0x8807);
    MT9M114_write_cmos_sensor(0xD1BA, 0xF40C);
    MT9M114_write_cmos_sensor(0xD1BC, 0x9118);
    MT9M114_write_cmos_sensor(0xD1BE, 0x2086);
    MT9M114_write_cmos_sensor(0xD1C0, 0x0FF3);
    MT9M114_write_cmos_sensor(0xD1C2, 0xB848);
    MT9M114_write_cmos_sensor(0xD1C4, 0x080D);
    MT9M114_write_cmos_sensor(0xD1C6, 0x0090);
    MT9M114_write_cmos_sensor(0xD1C8, 0xFFD9);
    MT9M114_write_cmos_sensor(0xD1CA, 0xE080);
    MT9M114_write_cmos_sensor(0xD1CC, 0xD801);
    MT9M114_write_cmos_sensor(0xD1CE, 0xF203);
    MT9M114_write_cmos_sensor(0xD1D0, 0xD800);
    MT9M114_write_cmos_sensor(0xD1D2, 0xF1DF);
    MT9M114_write_cmos_sensor(0xD1D4, 0x9040);
    MT9M114_write_cmos_sensor(0xD1D6, 0x71CF);
    MT9M114_write_cmos_sensor(0xD1D8, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD1DA, 0xC5D4);
    MT9M114_write_cmos_sensor(0xD1DC, 0xB15A);
    MT9M114_write_cmos_sensor(0xD1DE, 0x9041);
    MT9M114_write_cmos_sensor(0xD1E0, 0x73CF);
    MT9M114_write_cmos_sensor(0xD1E2, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD1E4, 0xC7D0);
    MT9M114_write_cmos_sensor(0xD1E6, 0xB140);
    MT9M114_write_cmos_sensor(0xD1E8, 0x9042);
    MT9M114_write_cmos_sensor(0xD1EA, 0xB141);
    MT9M114_write_cmos_sensor(0xD1EC, 0x9043);
    MT9M114_write_cmos_sensor(0xD1EE, 0xB142);
    MT9M114_write_cmos_sensor(0xD1F0, 0x9044);
    MT9M114_write_cmos_sensor(0xD1F2, 0xB143);
    MT9M114_write_cmos_sensor(0xD1F4, 0x9045);
    MT9M114_write_cmos_sensor(0xD1F6, 0xB147);
    MT9M114_write_cmos_sensor(0xD1F8, 0x9046);
    MT9M114_write_cmos_sensor(0xD1FA, 0xB148);
    MT9M114_write_cmos_sensor(0xD1FC, 0x9047);
    MT9M114_write_cmos_sensor(0xD1FE, 0xB14B);
    MT9M114_write_cmos_sensor(0xD200, 0x9048);
    MT9M114_write_cmos_sensor(0xD202, 0xB14C);
    MT9M114_write_cmos_sensor(0xD204, 0x9049);
    MT9M114_write_cmos_sensor(0xD206, 0x1958);
    MT9M114_write_cmos_sensor(0xD208, 0x0084);
    MT9M114_write_cmos_sensor(0xD20A, 0x904A);
    MT9M114_write_cmos_sensor(0xD20C, 0x195A);
    MT9M114_write_cmos_sensor(0xD20E, 0x0084);
    MT9M114_write_cmos_sensor(0xD210, 0x8856);
    MT9M114_write_cmos_sensor(0xD212, 0x1B36);
    MT9M114_write_cmos_sensor(0xD214, 0x8082);
    MT9M114_write_cmos_sensor(0xD216, 0x8857);
    MT9M114_write_cmos_sensor(0xD218, 0x1B37);
    MT9M114_write_cmos_sensor(0xD21A, 0x8082);
    MT9M114_write_cmos_sensor(0xD21C, 0x904C);
    MT9M114_write_cmos_sensor(0xD21E, 0x19A7);
    MT9M114_write_cmos_sensor(0xD220, 0x009C);
    MT9M114_write_cmos_sensor(0xD222, 0x881A);
    MT9M114_write_cmos_sensor(0xD224, 0x7FE0);
    MT9M114_write_cmos_sensor(0xD226, 0x1B54);
    MT9M114_write_cmos_sensor(0xD228, 0x8002);
    MT9M114_write_cmos_sensor(0xD22A, 0x78E0);
    MT9M114_write_cmos_sensor(0xD22C, 0x71CF);
    MT9M114_write_cmos_sensor(0xD22E, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD230, 0xC350);
    MT9M114_write_cmos_sensor(0xD232, 0xD828);
    MT9M114_write_cmos_sensor(0xD234, 0xA90B);
    MT9M114_write_cmos_sensor(0xD236, 0x8100);
    MT9M114_write_cmos_sensor(0xD238, 0x01C5);
    MT9M114_write_cmos_sensor(0xD23A, 0x0320);
    MT9M114_write_cmos_sensor(0xD23C, 0xD900);
    MT9M114_write_cmos_sensor(0xD23E, 0x78E0);
    MT9M114_write_cmos_sensor(0xD240, 0x220A);
    MT9M114_write_cmos_sensor(0xD242, 0x1F80);
    MT9M114_write_cmos_sensor(0xD244, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD246, 0xD4E0);
    MT9M114_write_cmos_sensor(0xD248, 0xC0F1);
    MT9M114_write_cmos_sensor(0xD24A, 0x0811);
    MT9M114_write_cmos_sensor(0xD24C, 0x0051);
    MT9M114_write_cmos_sensor(0xD24E, 0x2240);
    MT9M114_write_cmos_sensor(0xD250, 0x1200);
    MT9M114_write_cmos_sensor(0xD252, 0xFFE1);
    MT9M114_write_cmos_sensor(0xD254, 0xD801);
    MT9M114_write_cmos_sensor(0xD256, 0xF006);
    MT9M114_write_cmos_sensor(0xD258, 0x2240);
    MT9M114_write_cmos_sensor(0xD25A, 0x1900);
    MT9M114_write_cmos_sensor(0xD25C, 0xFFDE);
    MT9M114_write_cmos_sensor(0xD25E, 0xD802);
    MT9M114_write_cmos_sensor(0xD260, 0x1A05);
    MT9M114_write_cmos_sensor(0xD262, 0x1002);
    MT9M114_write_cmos_sensor(0xD264, 0xFFF2);
    MT9M114_write_cmos_sensor(0xD266, 0xF195);
    MT9M114_write_cmos_sensor(0xD268, 0xC0F1);
    MT9M114_write_cmos_sensor(0xD26A, 0x0E7E);
    MT9M114_write_cmos_sensor(0xD26C, 0x05C0);
    MT9M114_write_cmos_sensor(0xD26E, 0x75CF);
    MT9M114_write_cmos_sensor(0xD270, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD272, 0xC84C);
    MT9M114_write_cmos_sensor(0xD274, 0x9502);
    MT9M114_write_cmos_sensor(0xD276, 0x77CF);
    MT9M114_write_cmos_sensor(0xD278, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD27A, 0xC344);
    MT9M114_write_cmos_sensor(0xD27C, 0x2044);
    MT9M114_write_cmos_sensor(0xD27E, 0x008E);
    MT9M114_write_cmos_sensor(0xD280, 0xB8A1);
    MT9M114_write_cmos_sensor(0xD282, 0x0926);
    MT9M114_write_cmos_sensor(0xD284, 0x03E0);
    MT9M114_write_cmos_sensor(0xD286, 0xB502);
    MT9M114_write_cmos_sensor(0xD288, 0x9502);
    MT9M114_write_cmos_sensor(0xD28A, 0x952E);
    MT9M114_write_cmos_sensor(0xD28C, 0x7E05);
    MT9M114_write_cmos_sensor(0xD28E, 0xB5C2);
    MT9M114_write_cmos_sensor(0xD290, 0x70CF);
    MT9M114_write_cmos_sensor(0xD292, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD294, 0xC610);
    MT9M114_write_cmos_sensor(0xD296, 0x099A);
    MT9M114_write_cmos_sensor(0xD298, 0x04A0);
    MT9M114_write_cmos_sensor(0xD29A, 0xB026);
    MT9M114_write_cmos_sensor(0xD29C, 0x0E02);
    MT9M114_write_cmos_sensor(0xD29E, 0x0560);
    MT9M114_write_cmos_sensor(0xD2A0, 0xDE00);
    MT9M114_write_cmos_sensor(0xD2A2, 0x0A12);
    MT9M114_write_cmos_sensor(0xD2A4, 0x0320);
    MT9M114_write_cmos_sensor(0xD2A6, 0xB7C4);
    MT9M114_write_cmos_sensor(0xD2A8, 0x0B36);
    MT9M114_write_cmos_sensor(0xD2AA, 0x03A0);
    MT9M114_write_cmos_sensor(0xD2AC, 0x70C9);
    MT9M114_write_cmos_sensor(0xD2AE, 0x9502);
    MT9M114_write_cmos_sensor(0xD2B0, 0x7608);
    MT9M114_write_cmos_sensor(0xD2B2, 0xB8A8);
    MT9M114_write_cmos_sensor(0xD2B4, 0xB502);
    MT9M114_write_cmos_sensor(0xD2B6, 0x70CF);
    MT9M114_write_cmos_sensor(0xD2B8, 0x0000);
    MT9M114_write_cmos_sensor(0xD2BA, 0x5536);
    MT9M114_write_cmos_sensor(0xD2BC, 0x7860);
    MT9M114_write_cmos_sensor(0xD2BE, 0x2686);
    MT9M114_write_cmos_sensor(0xD2C0, 0x1FFB);
    MT9M114_write_cmos_sensor(0xD2C2, 0x9502);
    MT9M114_write_cmos_sensor(0xD2C4, 0x78C5);
    MT9M114_write_cmos_sensor(0xD2C6, 0x0631);
    MT9M114_write_cmos_sensor(0xD2C8, 0x05E0);
    MT9M114_write_cmos_sensor(0xD2CA, 0xB502);
    MT9M114_write_cmos_sensor(0xD2CC, 0x72CF);
    MT9M114_write_cmos_sensor(0xD2CE, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD2D0, 0xC5D4);
    MT9M114_write_cmos_sensor(0xD2D2, 0x923A);
    MT9M114_write_cmos_sensor(0xD2D4, 0x73CF);
    MT9M114_write_cmos_sensor(0xD2D6, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD2D8, 0xC7D0);
    MT9M114_write_cmos_sensor(0xD2DA, 0xB020);
    MT9M114_write_cmos_sensor(0xD2DC, 0x9220);
    MT9M114_write_cmos_sensor(0xD2DE, 0xB021);
    MT9M114_write_cmos_sensor(0xD2E0, 0x9221);
    MT9M114_write_cmos_sensor(0xD2E2, 0xB022);
    MT9M114_write_cmos_sensor(0xD2E4, 0x9222);
    MT9M114_write_cmos_sensor(0xD2E6, 0xB023);
    MT9M114_write_cmos_sensor(0xD2E8, 0x9223);
    MT9M114_write_cmos_sensor(0xD2EA, 0xB024);
    MT9M114_write_cmos_sensor(0xD2EC, 0x9227);
    MT9M114_write_cmos_sensor(0xD2EE, 0xB025);
    MT9M114_write_cmos_sensor(0xD2F0, 0x9228);
    MT9M114_write_cmos_sensor(0xD2F2, 0xB026);
    MT9M114_write_cmos_sensor(0xD2F4, 0x922B);
    MT9M114_write_cmos_sensor(0xD2F6, 0xB027);
    MT9M114_write_cmos_sensor(0xD2F8, 0x922C);
    MT9M114_write_cmos_sensor(0xD2FA, 0xB028);
    MT9M114_write_cmos_sensor(0xD2FC, 0x1258);
    MT9M114_write_cmos_sensor(0xD2FE, 0x0101);
    MT9M114_write_cmos_sensor(0xD300, 0xB029);
    MT9M114_write_cmos_sensor(0xD302, 0x125A);
    MT9M114_write_cmos_sensor(0xD304, 0x0101);
    MT9M114_write_cmos_sensor(0xD306, 0xB02A);
    MT9M114_write_cmos_sensor(0xD308, 0x1336);
    MT9M114_write_cmos_sensor(0xD30A, 0x8081);
    MT9M114_write_cmos_sensor(0xD30C, 0xA836);
    MT9M114_write_cmos_sensor(0xD30E, 0x1337);
    MT9M114_write_cmos_sensor(0xD310, 0x8081);
    MT9M114_write_cmos_sensor(0xD312, 0xA837);
    MT9M114_write_cmos_sensor(0xD314, 0x12A7);
    MT9M114_write_cmos_sensor(0xD316, 0x0701);
    MT9M114_write_cmos_sensor(0xD318, 0xB02C);
    MT9M114_write_cmos_sensor(0xD31A, 0x1354);
    MT9M114_write_cmos_sensor(0xD31C, 0x8081);
    MT9M114_write_cmos_sensor(0xD31E, 0x7FE0);
    MT9M114_write_cmos_sensor(0xD320, 0xA83A);
    MT9M114_write_cmos_sensor(0xD322, 0x78E0);
    MT9M114_write_cmos_sensor(0xD324, 0xC0F1);
    MT9M114_write_cmos_sensor(0xD326, 0x0DC2);
    MT9M114_write_cmos_sensor(0xD328, 0x05C0);
    MT9M114_write_cmos_sensor(0xD32A, 0x7608);
    MT9M114_write_cmos_sensor(0xD32C, 0x09BB);
    MT9M114_write_cmos_sensor(0xD32E, 0x0010);
    MT9M114_write_cmos_sensor(0xD330, 0x75CF);
    MT9M114_write_cmos_sensor(0xD332, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD334, 0xD4E0);
    MT9M114_write_cmos_sensor(0xD336, 0x8D21);
    MT9M114_write_cmos_sensor(0xD338, 0x8D00);
    MT9M114_write_cmos_sensor(0xD33A, 0x2153);
    MT9M114_write_cmos_sensor(0xD33C, 0x0003);
    MT9M114_write_cmos_sensor(0xD33E, 0xB8C0);
    MT9M114_write_cmos_sensor(0xD340, 0x8D45);
    MT9M114_write_cmos_sensor(0xD342, 0x0B23);
    MT9M114_write_cmos_sensor(0xD344, 0x0000);
    MT9M114_write_cmos_sensor(0xD346, 0xEA8F);
    MT9M114_write_cmos_sensor(0xD348, 0x0915);
    MT9M114_write_cmos_sensor(0xD34A, 0x001E);
    MT9M114_write_cmos_sensor(0xD34C, 0xFF81);
    MT9M114_write_cmos_sensor(0xD34E, 0xE808);
    MT9M114_write_cmos_sensor(0xD350, 0x2540);
    MT9M114_write_cmos_sensor(0xD352, 0x1900);
    MT9M114_write_cmos_sensor(0xD354, 0xFFDE);
    MT9M114_write_cmos_sensor(0xD356, 0x8D00);
    MT9M114_write_cmos_sensor(0xD358, 0xB880);
    MT9M114_write_cmos_sensor(0xD35A, 0xF004);
    MT9M114_write_cmos_sensor(0xD35C, 0x8D00);
    MT9M114_write_cmos_sensor(0xD35E, 0xB8A0);
    MT9M114_write_cmos_sensor(0xD360, 0xAD00);
    MT9M114_write_cmos_sensor(0xD362, 0x8D05);
    MT9M114_write_cmos_sensor(0xD364, 0xE081);
    MT9M114_write_cmos_sensor(0xD366, 0x20CC);
    MT9M114_write_cmos_sensor(0xD368, 0x80A2);
    MT9M114_write_cmos_sensor(0xD36A, 0xDF00);
    MT9M114_write_cmos_sensor(0xD36C, 0xF40A);
    MT9M114_write_cmos_sensor(0xD36E, 0x71CF);
    MT9M114_write_cmos_sensor(0xD370, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD372, 0xC84C);
    MT9M114_write_cmos_sensor(0xD374, 0x9102);
    MT9M114_write_cmos_sensor(0xD376, 0x7708);
    MT9M114_write_cmos_sensor(0xD378, 0xB8A6);
    MT9M114_write_cmos_sensor(0xD37A, 0x2786);
    MT9M114_write_cmos_sensor(0xD37C, 0x1FFE);
    MT9M114_write_cmos_sensor(0xD37E, 0xB102);
    MT9M114_write_cmos_sensor(0xD380, 0x0B42);
    MT9M114_write_cmos_sensor(0xD382, 0x0180);
    MT9M114_write_cmos_sensor(0xD384, 0x0E3E);
    MT9M114_write_cmos_sensor(0xD386, 0x0180);
    MT9M114_write_cmos_sensor(0xD388, 0x0F4A);
    MT9M114_write_cmos_sensor(0xD38A, 0x0160);
    MT9M114_write_cmos_sensor(0xD38C, 0x70C9);
    MT9M114_write_cmos_sensor(0xD38E, 0x8D05);
    MT9M114_write_cmos_sensor(0xD390, 0xE081);
    MT9M114_write_cmos_sensor(0xD392, 0x20CC);
    MT9M114_write_cmos_sensor(0xD394, 0x80A2);
    MT9M114_write_cmos_sensor(0xD396, 0xF429);
    MT9M114_write_cmos_sensor(0xD398, 0x76CF);
    MT9M114_write_cmos_sensor(0xD39A, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD39C, 0xC84C);
    MT9M114_write_cmos_sensor(0xD39E, 0x082D);
    MT9M114_write_cmos_sensor(0xD3A0, 0x0051);
    MT9M114_write_cmos_sensor(0xD3A2, 0x70CF);
    MT9M114_write_cmos_sensor(0xD3A4, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD3A6, 0xC90C);
    MT9M114_write_cmos_sensor(0xD3A8, 0x8805);
    MT9M114_write_cmos_sensor(0xD3AA, 0x09B6);
    MT9M114_write_cmos_sensor(0xD3AC, 0x0360);
    MT9M114_write_cmos_sensor(0xD3AE, 0xD908);
    MT9M114_write_cmos_sensor(0xD3B0, 0x2099);
    MT9M114_write_cmos_sensor(0xD3B2, 0x0802);
    MT9M114_write_cmos_sensor(0xD3B4, 0x9634);
    MT9M114_write_cmos_sensor(0xD3B6, 0xB503);
    MT9M114_write_cmos_sensor(0xD3B8, 0x7902);
    MT9M114_write_cmos_sensor(0xD3BA, 0x1523);
    MT9M114_write_cmos_sensor(0xD3BC, 0x1080);
    MT9M114_write_cmos_sensor(0xD3BE, 0xB634);
    MT9M114_write_cmos_sensor(0xD3C0, 0xE001);
    MT9M114_write_cmos_sensor(0xD3C2, 0x1D23);
    MT9M114_write_cmos_sensor(0xD3C4, 0x1002);
    MT9M114_write_cmos_sensor(0xD3C6, 0xF00B);
    MT9M114_write_cmos_sensor(0xD3C8, 0x9634);
    MT9M114_write_cmos_sensor(0xD3CA, 0x9503);
    MT9M114_write_cmos_sensor(0xD3CC, 0x6038);
    MT9M114_write_cmos_sensor(0xD3CE, 0xB614);
    MT9M114_write_cmos_sensor(0xD3D0, 0x153F);
    MT9M114_write_cmos_sensor(0xD3D2, 0x1080);
    MT9M114_write_cmos_sensor(0xD3D4, 0xE001);
    MT9M114_write_cmos_sensor(0xD3D6, 0x1D3F);
    MT9M114_write_cmos_sensor(0xD3D8, 0x1002);
    MT9M114_write_cmos_sensor(0xD3DA, 0xFFA4);
    MT9M114_write_cmos_sensor(0xD3DC, 0x9602);
    MT9M114_write_cmos_sensor(0xD3DE, 0x7F05);
    MT9M114_write_cmos_sensor(0xD3E0, 0xD800);
    MT9M114_write_cmos_sensor(0xD3E2, 0xB6E2);
    MT9M114_write_cmos_sensor(0xD3E4, 0xAD05);
    MT9M114_write_cmos_sensor(0xD3E6, 0x0511);
    MT9M114_write_cmos_sensor(0xD3E8, 0x05E0);
    MT9M114_write_cmos_sensor(0xD3EA, 0xD800);
    MT9M114_write_cmos_sensor(0xD3EC, 0xC0F1);
    MT9M114_write_cmos_sensor(0xD3EE, 0x0CFE);
    MT9M114_write_cmos_sensor(0xD3F0, 0x05C0);
    MT9M114_write_cmos_sensor(0xD3F2, 0x0A96);
    MT9M114_write_cmos_sensor(0xD3F4, 0x05A0);
    MT9M114_write_cmos_sensor(0xD3F6, 0x7608);
    MT9M114_write_cmos_sensor(0xD3F8, 0x0C22);
    MT9M114_write_cmos_sensor(0xD3FA, 0x0240);
    MT9M114_write_cmos_sensor(0xD3FC, 0xE080);
    MT9M114_write_cmos_sensor(0xD3FE, 0x20CA);
    MT9M114_write_cmos_sensor(0xD400, 0x0F82);
    MT9M114_write_cmos_sensor(0xD402, 0x0000);
    MT9M114_write_cmos_sensor(0xD404, 0x190B);
    MT9M114_write_cmos_sensor(0xD406, 0x0C60);
    MT9M114_write_cmos_sensor(0xD408, 0x05A2);
    MT9M114_write_cmos_sensor(0xD40A, 0x21CA);
    MT9M114_write_cmos_sensor(0xD40C, 0x0022);
    MT9M114_write_cmos_sensor(0xD40E, 0x0C56);
    MT9M114_write_cmos_sensor(0xD410, 0x0240);
    MT9M114_write_cmos_sensor(0xD412, 0xE806);
    MT9M114_write_cmos_sensor(0xD414, 0x0E0E);
    MT9M114_write_cmos_sensor(0xD416, 0x0220);
    MT9M114_write_cmos_sensor(0xD418, 0x70C9);
    MT9M114_write_cmos_sensor(0xD41A, 0xF048);
    MT9M114_write_cmos_sensor(0xD41C, 0x0896);
    MT9M114_write_cmos_sensor(0xD41E, 0x0440);
    MT9M114_write_cmos_sensor(0xD420, 0x0E96);
    MT9M114_write_cmos_sensor(0xD422, 0x0400);
    MT9M114_write_cmos_sensor(0xD424, 0x0966);
    MT9M114_write_cmos_sensor(0xD426, 0x0380);
    MT9M114_write_cmos_sensor(0xD428, 0x75CF);
    MT9M114_write_cmos_sensor(0xD42A, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD42C, 0xD4E0);
    MT9M114_write_cmos_sensor(0xD42E, 0x8D00);
    MT9M114_write_cmos_sensor(0xD430, 0x084D);
    MT9M114_write_cmos_sensor(0xD432, 0x001E);
    MT9M114_write_cmos_sensor(0xD434, 0xFF47);
    MT9M114_write_cmos_sensor(0xD436, 0x080D);
    MT9M114_write_cmos_sensor(0xD438, 0x0050);
    MT9M114_write_cmos_sensor(0xD43A, 0xFF57);
    MT9M114_write_cmos_sensor(0xD43C, 0x0841);
    MT9M114_write_cmos_sensor(0xD43E, 0x0051);
    MT9M114_write_cmos_sensor(0xD440, 0x8D04);
    MT9M114_write_cmos_sensor(0xD442, 0x9521);
    MT9M114_write_cmos_sensor(0xD444, 0xE064);
    MT9M114_write_cmos_sensor(0xD446, 0x790C);
    MT9M114_write_cmos_sensor(0xD448, 0x702F);
    MT9M114_write_cmos_sensor(0xD44A, 0x0CE2);
    MT9M114_write_cmos_sensor(0xD44C, 0x05E0);
    MT9M114_write_cmos_sensor(0xD44E, 0xD964);
    MT9M114_write_cmos_sensor(0xD450, 0x72CF);
    MT9M114_write_cmos_sensor(0xD452, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD454, 0xC700);
    MT9M114_write_cmos_sensor(0xD456, 0x9235);
    MT9M114_write_cmos_sensor(0xD458, 0x0811);
    MT9M114_write_cmos_sensor(0xD45A, 0x0043);
    MT9M114_write_cmos_sensor(0xD45C, 0xFF3D);
    MT9M114_write_cmos_sensor(0xD45E, 0x080D);
    MT9M114_write_cmos_sensor(0xD460, 0x0051);
    MT9M114_write_cmos_sensor(0xD462, 0xD801);
    MT9M114_write_cmos_sensor(0xD464, 0xFF77);
    MT9M114_write_cmos_sensor(0xD466, 0xF025);
    MT9M114_write_cmos_sensor(0xD468, 0x9501);
    MT9M114_write_cmos_sensor(0xD46A, 0x9235);
    MT9M114_write_cmos_sensor(0xD46C, 0x0911);
    MT9M114_write_cmos_sensor(0xD46E, 0x0003);
    MT9M114_write_cmos_sensor(0xD470, 0xFF49);
    MT9M114_write_cmos_sensor(0xD472, 0x080D);
    MT9M114_write_cmos_sensor(0xD474, 0x0051);
    MT9M114_write_cmos_sensor(0xD476, 0xD800);
    MT9M114_write_cmos_sensor(0xD478, 0xFF72);
    MT9M114_write_cmos_sensor(0xD47A, 0xF01B);
    MT9M114_write_cmos_sensor(0xD47C, 0x0886);
    MT9M114_write_cmos_sensor(0xD47E, 0x03E0);
    MT9M114_write_cmos_sensor(0xD480, 0xD801);
    MT9M114_write_cmos_sensor(0xD482, 0x0EF6);
    MT9M114_write_cmos_sensor(0xD484, 0x03C0);
    MT9M114_write_cmos_sensor(0xD486, 0x0F52);
    MT9M114_write_cmos_sensor(0xD488, 0x0340);
    MT9M114_write_cmos_sensor(0xD48A, 0x0DBA);
    MT9M114_write_cmos_sensor(0xD48C, 0x0200);
    MT9M114_write_cmos_sensor(0xD48E, 0x0AF6);
    MT9M114_write_cmos_sensor(0xD490, 0x0440);
    MT9M114_write_cmos_sensor(0xD492, 0x0C22);
    MT9M114_write_cmos_sensor(0xD494, 0x0400);
    MT9M114_write_cmos_sensor(0xD496, 0x0D72);
    MT9M114_write_cmos_sensor(0xD498, 0x0440);
    MT9M114_write_cmos_sensor(0xD49A, 0x0DC2);
    MT9M114_write_cmos_sensor(0xD49C, 0x0200);
    MT9M114_write_cmos_sensor(0xD49E, 0x0972);
    MT9M114_write_cmos_sensor(0xD4A0, 0x0440);
    MT9M114_write_cmos_sensor(0xD4A2, 0x0D3A);
    MT9M114_write_cmos_sensor(0xD4A4, 0x0220);
    MT9M114_write_cmos_sensor(0xD4A6, 0xD820);
    MT9M114_write_cmos_sensor(0xD4A8, 0x0BFA);
    MT9M114_write_cmos_sensor(0xD4AA, 0x0260);
    MT9M114_write_cmos_sensor(0xD4AC, 0x70C9);
    MT9M114_write_cmos_sensor(0xD4AE, 0x0451);
    MT9M114_write_cmos_sensor(0xD4B0, 0x05C0);
    MT9M114_write_cmos_sensor(0xD4B2, 0x78E0);
    MT9M114_write_cmos_sensor(0xD4B4, 0xD900);
    MT9M114_write_cmos_sensor(0xD4B6, 0xF00A);
    MT9M114_write_cmos_sensor(0xD4B8, 0x70CF);
    MT9M114_write_cmos_sensor(0xD4BA, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD4BC, 0xD520);
    MT9M114_write_cmos_sensor(0xD4BE, 0x7835);
    MT9M114_write_cmos_sensor(0xD4C0, 0x8041);
    MT9M114_write_cmos_sensor(0xD4C2, 0x8000);
    MT9M114_write_cmos_sensor(0xD4C4, 0xE102);
    MT9M114_write_cmos_sensor(0xD4C6, 0xA040);
    MT9M114_write_cmos_sensor(0xD4C8, 0x09F1);
    MT9M114_write_cmos_sensor(0xD4CA, 0x8114);
    MT9M114_write_cmos_sensor(0xD4CC, 0x71CF);
    MT9M114_write_cmos_sensor(0xD4CE, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD4D0, 0xD4E0);
    MT9M114_write_cmos_sensor(0xD4D2, 0x70CF);
    MT9M114_write_cmos_sensor(0xD4D4, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD4D6, 0xC594);
    MT9M114_write_cmos_sensor(0xD4D8, 0xB03A);
    MT9M114_write_cmos_sensor(0xD4DA, 0x7FE0);
    MT9M114_write_cmos_sensor(0xD4DC, 0xD800);
    MT9M114_write_cmos_sensor(0xD4DE, 0x0000);
    MT9M114_write_cmos_sensor(0xD4E0, 0x0000);
    MT9M114_write_cmos_sensor(0xD4E2, 0x0500);
    MT9M114_write_cmos_sensor(0xD4E4, 0x0500);
    MT9M114_write_cmos_sensor(0xD4E6, 0x0200);
    MT9M114_write_cmos_sensor(0xD4E8, 0x0330);
    MT9M114_write_cmos_sensor(0xD4EA, 0x0000);
    MT9M114_write_cmos_sensor(0xD4EC, 0x0000);
    MT9M114_write_cmos_sensor(0xD4EE, 0x03CD);
    MT9M114_write_cmos_sensor(0xD4F0, 0x050D);
    MT9M114_write_cmos_sensor(0xD4F2, 0x01C5);
    MT9M114_write_cmos_sensor(0xD4F4, 0x03B3);
    MT9M114_write_cmos_sensor(0xD4F6, 0x00E0);
    MT9M114_write_cmos_sensor(0xD4F8, 0x01E3);
    MT9M114_write_cmos_sensor(0xD4FA, 0x0280);
    MT9M114_write_cmos_sensor(0xD4FC, 0x01E0);
    MT9M114_write_cmos_sensor(0xD4FE, 0x0109);
    MT9M114_write_cmos_sensor(0xD500, 0x0080);
    MT9M114_write_cmos_sensor(0xD502, 0x0500);
    MT9M114_write_cmos_sensor(0xD504, 0x0000);
    MT9M114_write_cmos_sensor(0xD506, 0x0000);
    MT9M114_write_cmos_sensor(0xD508, 0x0000);
    MT9M114_write_cmos_sensor(0xD50A, 0x0000);
    MT9M114_write_cmos_sensor(0xD50C, 0x0000);
    MT9M114_write_cmos_sensor(0xD50E, 0x0000);
    MT9M114_write_cmos_sensor(0xD510, 0x0000);
    MT9M114_write_cmos_sensor(0xD512, 0x0000);
    MT9M114_write_cmos_sensor(0xD514, 0x0000);
    MT9M114_write_cmos_sensor(0xD516, 0x0000);
    MT9M114_write_cmos_sensor(0xD518, 0x0000);
    MT9M114_write_cmos_sensor(0xD51A, 0x0000);
    MT9M114_write_cmos_sensor(0xD51C, 0x0000);
    MT9M114_write_cmos_sensor(0xD51E, 0x0000);
    MT9M114_write_cmos_sensor(0xD520, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD522, 0xC9B4);
    MT9M114_write_cmos_sensor(0xD524, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD526, 0xD324);
    MT9M114_write_cmos_sensor(0xD528, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD52A, 0xCA34);
    MT9M114_write_cmos_sensor(0xD52C, 0xFFFF);
    MT9M114_write_cmos_sensor(0xD52E, 0xD3EC);

    
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor(0xE000, 0x04B4); 	// PATCHLDR_LOADER_ADDRESS
    MT9M114_write_cmos_sensor(0xE002, 0x0302); 	// PATCHLDR_PATCH_ID
    //FixMe:
    MT9M114_write_cmos_sensor(0xE004, 0x4103);  // PATCHLDR_FIRMWARE_ID
    MT9M114_write_cmos_sensor(0xE006, 0x0202); 	// PATCHLDR_FIRMWARE_ID

    MT9M114_write_cmos_sensor(0x0080, 0xFFF0); 	// COMMAND_REGISTER
    //  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00
    //mDELAY(100);

    MT9M114_write_cmos_sensor(0x0080, 0xFFF1); 	// COMMAND_REGISTER
    //  POLL  COMMAND_REGISTER::HOST_COMMAND_0 =>  0x00
    //mDELAY(100);
    MT9M114_wait_command_0();


    //Soc2 register
    MT9M114_write_cmos_sensor(0x3640, 0x00F0); 	//  P_G1_P0Q0
    MT9M114_write_cmos_sensor(0x3642, 0x962B); 	//  P_G1_P0Q1
    MT9M114_write_cmos_sensor(0x3644, 0x26D0); 	//  P_G1_P0Q2
    MT9M114_write_cmos_sensor(0x3646, 0x15AD); 	//  P_G1_P0Q3
    MT9M114_write_cmos_sensor(0x3648, 0x498F); 	//  P_G1_P0Q4
    MT9M114_write_cmos_sensor(0x364A, 0x00F0); 	//  P_R_P0Q0
    MT9M114_write_cmos_sensor(0x364C, 0x4609); 	//  P_R_P0Q1
    MT9M114_write_cmos_sensor(0x364E, 0x0211); 	//  P_R_P0Q2
    MT9M114_write_cmos_sensor(0x3650, 0x166E); 	//  P_R_P0Q3
    MT9M114_write_cmos_sensor(0x3652, 0x668F); 	//  P_R_P0Q4
    MT9M114_write_cmos_sensor(0x3654, 0x0170); 	//  P_B_P0Q0
    MT9M114_write_cmos_sensor(0x3656, 0x2CEB); 	//  P_B_P0Q1
    MT9M114_write_cmos_sensor(0x3658, 0x616F); 	//  P_B_P0Q2
    MT9M114_write_cmos_sensor(0x365A, 0x416C); 	//  P_B_P0Q3
    MT9M114_write_cmos_sensor(0x365C, 0x1BB0); 	//  P_B_P0Q4
    MT9M114_write_cmos_sensor(0x365E, 0x00F0); 	//  P_G2_P0Q0
    MT9M114_write_cmos_sensor(0x3660, 0x96AB); 	//  P_G2_P0Q1
    MT9M114_write_cmos_sensor(0x3662, 0x26B0); 	//  P_G2_P0Q2
    MT9M114_write_cmos_sensor(0x3664, 0x166D); 	//  P_G2_P0Q3
    MT9M114_write_cmos_sensor(0x3666, 0x49CF); 	//  P_G2_P0Q4
    MT9M114_write_cmos_sensor(0x3680, 0x8A0C); 	//  P_G1_P1Q0
    MT9M114_write_cmos_sensor(0x3682, 0x28CA); 	//  P_G1_P1Q1
    MT9M114_write_cmos_sensor(0x3684, 0xAD2D); 	//  P_G1_P1Q2
    MT9M114_write_cmos_sensor(0x3686, 0xD5AB); 	//  P_G1_P1Q3
    MT9M114_write_cmos_sensor(0x3688, 0x212E); 	//  P_G1_P1Q4
    MT9M114_write_cmos_sensor(0x368A, 0x910D); 	//  P_R_P1Q0
    MT9M114_write_cmos_sensor(0x368C, 0x0F6C); 	//  P_R_P1Q1
    MT9M114_write_cmos_sensor(0x368E, 0xC32D); 	//  P_R_P1Q2
    MT9M114_write_cmos_sensor(0x3690, 0xA9CC); 	//  P_R_P1Q3
    MT9M114_write_cmos_sensor(0x3692, 0x352F); 	//  P_R_P1Q4
    MT9M114_write_cmos_sensor(0x3694, 0xA60A); 	//  P_B_P1Q0
    MT9M114_write_cmos_sensor(0x3696, 0x136C); 	//  P_B_P1Q1
    MT9M114_write_cmos_sensor(0x3698, 0xCC0E); 	//  P_B_P1Q2
    MT9M114_write_cmos_sensor(0x369A, 0x880E); 	//  P_B_P1Q3
    MT9M114_write_cmos_sensor(0x369C, 0x560E); 	//  P_B_P1Q4
    MT9M114_write_cmos_sensor(0x369E, 0x8AEC); 	//  P_G2_P1Q0
    MT9M114_write_cmos_sensor(0x36A0, 0x19EA); 	//  P_G2_P1Q1
    MT9M114_write_cmos_sensor(0x36A2, 0xAE4D); 	//  P_G2_P1Q2
    MT9M114_write_cmos_sensor(0x36A4, 0xA88B); 	//  P_G2_P1Q3
    MT9M114_write_cmos_sensor(0x36A6, 0x220E); 	//  P_G2_P1Q4
    MT9M114_write_cmos_sensor(0x36C0, 0x4930); 	//  P_G1_P2Q0
    MT9M114_write_cmos_sensor(0x36C2, 0xC32C); 	//  P_G1_P2Q1
    MT9M114_write_cmos_sensor(0x36C4, 0x3731); 	//  P_G1_P2Q2
    MT9M114_write_cmos_sensor(0x36C6, 0x24D0); 	//  P_G1_P2Q3
    MT9M114_write_cmos_sensor(0x36C8, 0xBBF0); 	//  P_G1_P2Q4
    MT9M114_write_cmos_sensor(0x36CA, 0x18D1); 	//  P_R_P2Q0
    MT9M114_write_cmos_sensor(0x36CC, 0x720A); 	//  P_R_P2Q1
    MT9M114_write_cmos_sensor(0x36CE, 0x0852); 	//  P_R_P2Q2
    MT9M114_write_cmos_sensor(0x36D0, 0x15D0); 	//  P_R_P2Q3
    MT9M114_write_cmos_sensor(0x36D2, 0xA7F2); 	//  P_R_P2Q4
    MT9M114_write_cmos_sensor(0x36D4, 0x1050); 	//  P_B_P2Q0
    MT9M114_write_cmos_sensor(0x36D6, 0x560B); 	//  P_B_P2Q1
    MT9M114_write_cmos_sensor(0x36D8, 0x6BD1); 	//  P_B_P2Q2
    MT9M114_write_cmos_sensor(0x36DA, 0x452F); 	//  P_B_P2Q3
    MT9M114_write_cmos_sensor(0x36DC, 0xDFD1); 	//  P_B_P2Q4
    MT9M114_write_cmos_sensor(0x36DE, 0x4910); 	//  P_G2_P2Q0
    MT9M114_write_cmos_sensor(0x36E0, 0xA12C); 	//  P_G2_P2Q1
    MT9M114_write_cmos_sensor(0x36E2, 0x38B1); 	//  P_G2_P2Q2
    MT9M114_write_cmos_sensor(0x36E4, 0x16D0); 	//  P_G2_P2Q3
    MT9M114_write_cmos_sensor(0x36E6, 0xC030); 	//  P_G2_P2Q4
    MT9M114_write_cmos_sensor(0x3700, 0xB66C); 	//  P_G1_P3Q0
    MT9M114_write_cmos_sensor(0x3702, 0xB68D); 	//  P_G1_P3Q1
    MT9M114_write_cmos_sensor(0x3704, 0xDDAF); 	//  P_G1_P3Q2
    MT9M114_write_cmos_sensor(0x3706, 0x1E0F); 	//  P_G1_P3Q3
    MT9M114_write_cmos_sensor(0x3708, 0x6051); 	//  P_G1_P3Q4
    MT9M114_write_cmos_sensor(0x370A, 0x926E); 	//  P_R_P3Q0
    MT9M114_write_cmos_sensor(0x370C, 0x9C6E); 	//  P_R_P3Q1
    MT9M114_write_cmos_sensor(0x370E, 0x7BEE); 	//  P_R_P3Q2
    MT9M114_write_cmos_sensor(0x3710, 0x6D6F); 	//  P_R_P3Q3
    MT9M114_write_cmos_sensor(0x3712, 0x0A51); 	//  P_R_P3Q4
    MT9M114_write_cmos_sensor(0x3714, 0x214C); 	//  P_B_P3Q0
    MT9M114_write_cmos_sensor(0x3716, 0xDC6E); 	//  P_B_P3Q1
    MT9M114_write_cmos_sensor(0x3718, 0xE40F); 	//  P_B_P3Q2
    MT9M114_write_cmos_sensor(0x371A, 0x1290); 	//  P_B_P3Q3
    MT9M114_write_cmos_sensor(0x371C, 0x1072); 	//  P_B_P3Q4
    MT9M114_write_cmos_sensor(0x371E, 0xAF2C); 	//  P_G2_P3Q0
    MT9M114_write_cmos_sensor(0x3720, 0xAC4D); 	//  P_G2_P3Q1
    MT9M114_write_cmos_sensor(0x3722, 0xDC4F); 	//  P_G2_P3Q2
    MT9M114_write_cmos_sensor(0x3724, 0x0D8F); 	//  P_G2_P3Q3
    MT9M114_write_cmos_sensor(0x3726, 0x6151); 	//  P_G2_P3Q4
    MT9M114_write_cmos_sensor(0x3740, 0xFA2E); 	//  P_G1_P4Q0
    MT9M114_write_cmos_sensor(0x3742, 0x21D0); 	//  P_G1_P4Q1
    MT9M114_write_cmos_sensor(0x3744, 0x0E73); 	//  P_G1_P4Q2
    MT9M114_write_cmos_sensor(0x3746, 0xC492); 	//  P_G1_P4Q3
    MT9M114_write_cmos_sensor(0x3748, 0xFF95); 	//  P_G1_P4Q4
    MT9M114_write_cmos_sensor(0x374A, 0x57C8); 	//  P_R_P4Q0
    MT9M114_write_cmos_sensor(0x374C, 0x5970); 	//  P_R_P4Q1
    MT9M114_write_cmos_sensor(0x374E, 0x5D12); 	//  P_R_P4Q2
    MT9M114_write_cmos_sensor(0x3750, 0xAD33); 	//  P_R_P4Q3
    MT9M114_write_cmos_sensor(0x3752, 0xAF36); 	//  P_R_P4Q4
    MT9M114_write_cmos_sensor(0x3754, 0x478F); 	//  P_B_P4Q0
    MT9M114_write_cmos_sensor(0x3756, 0x0AEE); 	//  P_B_P4Q1
    MT9M114_write_cmos_sensor(0x3758, 0x2212); 	//  P_B_P4Q2
    MT9M114_write_cmos_sensor(0x375A, 0xA132); 	//  P_B_P4Q3
    MT9M114_write_cmos_sensor(0x375C, 0xD9B5); 	//  P_B_P4Q4
    MT9M114_write_cmos_sensor(0x375E, 0xF80E); 	//  P_G2_P4Q0
    MT9M114_write_cmos_sensor(0x3760, 0x1570); 	//  P_G2_P4Q1
    MT9M114_write_cmos_sensor(0x3762, 0x0B93); 	//  P_G2_P4Q2
    MT9M114_write_cmos_sensor(0x3764, 0xAEB2); 	//  P_G2_P4Q3
    MT9M114_write_cmos_sensor(0x3766, 0xFE55); 	//  P_G2_P4Q4
    MT9M114_write_cmos_sensor(0x3784, 0x0280); 	//  CENTER_COLUMN
    MT9M114_write_cmos_sensor(0x3782, 0x01D8); 	//  CENTER_ROW
    MT9M114_write_cmos_sensor(0x37C0, 0xA749); 	//  P_GR_Q5
    MT9M114_write_cmos_sensor(0x37C2, 0xE768); 	//  P_RD_Q5
    MT9M114_write_cmos_sensor(0x37C4, 0xCDEA); 	//  P_BL_Q5
    MT9M114_write_cmos_sensor(0x37C6, 0xA509); 	//  P_GB_Q5

    // Adaptive PGA
    // color interpolation and edge detection
    // color correction and aperture correction
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	//  LOGICAL addressing
    MT9M114_write_cmos_sensor(0xC960, 0x0ABE); 	//  CAM_PGA_L_CONFIG_COLOUR_TEMP
    MT9M114_write_cmos_sensor(0xC962, 0x76E4); 	//  CAM_PGA_L_CONFIG_GREEN_RED_Q14
    MT9M114_write_cmos_sensor(0xC964, 0x6380); 	//  CAM_PGA_L_CONFIG_RED_Q14
    MT9M114_write_cmos_sensor(0xC966, 0x76C8); 	//  CAM_PGA_L_CONFIG_GREEN_BLUE_Q14
    MT9M114_write_cmos_sensor(0xC968, 0x78C1); 	//  CAM_PGA_L_CONFIG_BLUE_Q14
    MT9M114_write_cmos_sensor(0xC96A, 0x0F0A); 	//  CAM_PGA_M_CONFIG_COLOUR_TEMP
    MT9M114_write_cmos_sensor(0xC96C, 0x7F13); 	//  CAM_PGA_M_CONFIG_GREEN_RED_Q14
    MT9M114_write_cmos_sensor(0xC96E, 0x89E6); 	//  CAM_PGA_M_CONFIG_RED_Q14
    MT9M114_write_cmos_sensor(0xC970, 0x7F0C); 	//  CAM_PGA_M_CONFIG_GREEN_BLUE_Q14
    MT9M114_write_cmos_sensor(0xC972, 0x83E0); 	//  CAM_PGA_M_CONFIG_BLUE_Q14
    MT9M114_write_cmos_sensor(0xC974, 0x1964); 	//  CAM_PGA_R_CONFIG_COLOUR_TEMP
    MT9M114_write_cmos_sensor(0xC976, 0x7F59); 	//  CAM_PGA_R_CONFIG_GREEN_RED_Q14
    MT9M114_write_cmos_sensor(0xC978, 0x7F8C); 	//  CAM_PGA_R_CONFIG_RED_Q14
    MT9M114_write_cmos_sensor(0xC97A, 0x7F5B); 	//  CAM_PGA_R_CONFIG_GREEN_BLUE_Q14
    MT9M114_write_cmos_sensor(0xC97C, 0x7E64); 	//  CAM_PGA_R_CONFIG_BLUE_Q14
    MT9M114_write_cmos_sensor(0xC95E, 0x0003); 	//  CAM_PGA_PGA_CONTROL

    // CCM
    MT9M114_write_cmos_sensor(0xC892, 0x0267); 	// CAM_AWB_CCM_L_0
    MT9M114_write_cmos_sensor(0xC894, 0xFF1A); 	// CAM_AWB_CCM_L_1
    MT9M114_write_cmos_sensor(0xC896, 0xFFB3); 	// CAM_AWB_CCM_L_2
    MT9M114_write_cmos_sensor(0xC898, 0xFF80); 	// CAM_AWB_CCM_L_3
    MT9M114_write_cmos_sensor(0xC89A, 0x0166); 	// CAM_AWB_CCM_L_4
    MT9M114_write_cmos_sensor(0xC89C, 0x0003); 	// CAM_AWB_CCM_L_5
    MT9M114_write_cmos_sensor(0xC89E, 0xFF9A); 	// CAM_AWB_CCM_L_6
    MT9M114_write_cmos_sensor(0xC8A0, 0xFEB4); 	// CAM_AWB_CCM_L_7
    MT9M114_write_cmos_sensor(0xC8A2, 0x024D); 	// CAM_AWB_CCM_L_8
    MT9M114_write_cmos_sensor(0xC8A4, 0x01BF); 	// CAM_AWB_CCM_M_0
    MT9M114_write_cmos_sensor(0xC8A6, 0xFF01); 	// CAM_AWB_CCM_M_1
    MT9M114_write_cmos_sensor(0xC8A8, 0xFFF3); 	// CAM_AWB_CCM_M_2
    MT9M114_write_cmos_sensor(0xC8AA, 0xFF75); 	// CAM_AWB_CCM_M_3
    MT9M114_write_cmos_sensor(0xC8AC, 0x0198); 	// CAM_AWB_CCM_M_4
    MT9M114_write_cmos_sensor(0xC8AE, 0xFFFD); 	// CAM_AWB_CCM_M_5
    MT9M114_write_cmos_sensor(0xC8B0, 0xFF9A); 	// CAM_AWB_CCM_M_6
    MT9M114_write_cmos_sensor(0xC8B2, 0xFEE7); 	// CAM_AWB_CCM_M_7
    MT9M114_write_cmos_sensor(0xC8B4, 0x02A8); 	// CAM_AWB_CCM_M_8
    MT9M114_write_cmos_sensor(0xC8B6, 0x01D9); 	// CAM_AWB_CCM_R_0
    MT9M114_write_cmos_sensor(0xC8B8, 0xFF26); 	// CAM_AWB_CCM_R_1
    MT9M114_write_cmos_sensor(0xC8BA, 0xFFF3); 	// CAM_AWB_CCM_R_2
    MT9M114_write_cmos_sensor(0xC8BC, 0xFFB3); 	// CAM_AWB_CCM_R_3
    MT9M114_write_cmos_sensor(0xC8BE, 0x0132); 	// CAM_AWB_CCM_R_4
    MT9M114_write_cmos_sensor(0xC8C0, 0xFFE8); 	// CAM_AWB_CCM_R_5
    MT9M114_write_cmos_sensor(0xC8C2, 0xFFDA); 	// CAM_AWB_CCM_R_6
    MT9M114_write_cmos_sensor(0xC8C4, 0xFECD); 	// CAM_AWB_CCM_R_7
    MT9M114_write_cmos_sensor(0xC8C6, 0x02C2); 	// CAM_AWB_CCM_R_8
    MT9M114_write_cmos_sensor(0xC8C8, 0x0075); 	// CAM_AWB_CCM_L_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8CA, 0x011C); 	// CAM_AWB_CCM_L_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8CC, 0x009A); 	// CAM_AWB_CCM_M_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8CE, 0x0105); 	// CAM_AWB_CCM_M_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8D0, 0x00A4); 	// CAM_AWB_CCM_R_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8D2, 0x00AC); 	// CAM_AWB_CCM_R_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8D4, 0x0A8C); 	// CAM_AWB_CCM_L_CTEMP
    MT9M114_write_cmos_sensor(0xC8D6, 0x0F0A); 	// CAM_AWB_CCM_M_CTEMP
    MT9M114_write_cmos_sensor(0xC8D8, 0x1964); 	// CAM_AWB_CCM_R_CTEMP
    MT9M114_write_cmos_sensor(0xC914, 0x0000); 	// CAM_STAT_AWB_CLIP_WINDOW_XSTART
    MT9M114_write_cmos_sensor(0xC916, 0x0000); 	// CAM_STAT_AWB_CLIP_WINDOW_YSTART
    MT9M114_write_cmos_sensor(0xC918, 0x04FF); 	// CAM_STAT_AWB_CLIP_WINDOW_XEND
    MT9M114_write_cmos_sensor(0xC91A, 0x02CF); 	// CAM_STAT_AWB_CLIP_WINDOW_YEND
    MT9M114_write_cmos_sensor(0xC904, 0x0033); 	// CAM_AWB_AWB_XSHIFT_PRE_ADJ
    MT9M114_write_cmos_sensor(0xC906, 0x0040); 	// CAM_AWB_AWB_YSHIFT_PRE_ADJ
    MT9M114_write_cmos_sensor_8(0xC8F2, 0x04);  // CAM_AWB_AWB_XSCALE
    MT9M114_write_cmos_sensor_8(0xC8F3, 0x02);  // CAM_AWB_AWB_YSCALE
    MT9M114_write_cmos_sensor(0xC8F4, 0x0000); 	// CAM_AWB_AWB_WEIGHTS_0
    MT9M114_write_cmos_sensor(0xC8F6, 0x0000); 	// CAM_AWB_AWB_WEIGHTS_1
    MT9M114_write_cmos_sensor(0xC8F8, 0x0000); 	// CAM_AWB_AWB_WEIGHTS_2
    MT9M114_write_cmos_sensor(0xC8FA, 0x47A4); 	// CAM_AWB_AWB_WEIGHTS_3
    MT9M114_write_cmos_sensor(0xC8FC, 0x1EB4); 	// CAM_AWB_AWB_WEIGHTS_4
    MT9M114_write_cmos_sensor(0xC8FE, 0x2045); 	// CAM_AWB_AWB_WEIGHTS_5
    MT9M114_write_cmos_sensor(0xC900, 0x01AC); 	// CAM_AWB_AWB_WEIGHTS_6
    MT9M114_write_cmos_sensor(0xC902, 0x007C); 	// CAM_AWB_AWB_WEIGHTS_7
    MT9M114_write_cmos_sensor(0xC926, 0x0020); 	// CAM_LL_START_BRIGHTNESS
    MT9M114_write_cmos_sensor(0xC928, 0x009A); 	// CAM_LL_STOP_BRIGHTNESS
    MT9M114_write_cmos_sensor(0xC946, 0x0096); 	// CAM_LL_START_GAIN_METRIC
    MT9M114_write_cmos_sensor(0xC948, 0x00F5); 	// CAM_LL_STOP_GAIN_METRIC
    MT9M114_write_cmos_sensor(0xC952, 0x0020); 	// CAM_LL_START_TARGET_LUMA_BM
    MT9M114_write_cmos_sensor(0xC954, 0x009A); 	// CAM_LL_STOP_TARGET_LUMA_BM
    MT9M114_write_cmos_sensor(0xC926, 0x0020); 	// CAM_LL_START_BRIGHTNESS
    MT9M114_write_cmos_sensor(0xC928, 0x009A); 	// CAM_LL_STOP_BRIGHTNESS
    MT9M114_write_cmos_sensor_8(0xC92A, 0x80);  // CAM_LL_START_SATURATION // defaul setting 0x80
    MT9M114_write_cmos_sensor_8(0xC92B, 0x3C);  // CAM_LL_END_SATURATION
    MT9M114_write_cmos_sensor_8(0xC92C, 0x00);  // CAM_LL_START_DESATURATION
    MT9M114_write_cmos_sensor_8(0xC92D, 0xFF);  // CAM_LL_END_DESATURATION
    MT9M114_write_cmos_sensor_8(0xC92E, 0x1E);  // CAM_LL_START_DEMOSAIC
    MT9M114_write_cmos_sensor_8(0xC92F, 0x03);  // CAM_LL_START_AP_GAIN
    MT9M114_write_cmos_sensor_8(0xC930, 0x06);  // CAM_LL_START_AP_THRESH
    MT9M114_write_cmos_sensor_8(0xC931, 0x78);  // CAM_LL_STOP_DEMOSAIC
    MT9M114_write_cmos_sensor_8(0xC932, 0x01);  // CAM_LL_STOP_AP_GAIN
    MT9M114_write_cmos_sensor_8(0xC933, 0x0C);  // CAM_LL_STOP_AP_THRESH
    MT9M114_write_cmos_sensor_8(0xC934, 0x64);  // CAM_LL_START_NR_RED
    MT9M114_write_cmos_sensor_8(0xC935, 0x64);  // CAM_LL_START_NR_GREEN
    MT9M114_write_cmos_sensor_8(0xC936, 0x64);  // CAM_LL_START_NR_BLUE
    MT9M114_write_cmos_sensor_8(0xC937, 0x0F);  // CAM_LL_START_NR_THRESH
    MT9M114_write_cmos_sensor_8(0xC938, 0x64);  // CAM_LL_STOP_NR_RED
    MT9M114_write_cmos_sensor_8(0xC939, 0x64);  // CAM_LL_STOP_NR_GREEN
    MT9M114_write_cmos_sensor_8(0xC93A, 0x64);  // CAM_LL_STOP_NR_BLUE
    MT9M114_write_cmos_sensor_8(0xC93B, 0x32);  // CAM_LL_STOP_NR_THRESH
    MT9M114_write_cmos_sensor(0xC93C, 0x0020); 	// CAM_LL_START_CONTRAST_BM
    MT9M114_write_cmos_sensor(0xC93E, 0x009A); 	// CAM_LL_STOP_CONTRAST_BM
    MT9M114_write_cmos_sensor(0xC940, 0x00DC); 	// CAM_LL_GAMMA
    MT9M114_write_cmos_sensor_8(0xC942, 0x30); 	// CAM_LL_START_CONTRAST_GRADIENT
    MT9M114_write_cmos_sensor_8(0xC943, 0x30); 	// CAM_LL_STOP_CONTRAST_GRADIENT
    MT9M114_write_cmos_sensor_8(0xC944, 0x50); 	// CAM_LL_START_CONTRAST_LUMA_PERCENTAGE
    MT9M114_write_cmos_sensor_8(0xC945, 0x19); 	// CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE
    MT9M114_write_cmos_sensor(0xC946, 0x0096); 	// CAM_LL_START_GAIN_METRIC
    MT9M114_write_cmos_sensor(0xC948, 0x00F3); 	// CAM_LL_STOP_GAIN_METRIC
    MT9M114_write_cmos_sensor(0xC94A, 0x0370); 	// CAM_LL_START_FADE_TO_BLACK_LUMA
    MT9M114_write_cmos_sensor(0xC94C, 0x00F0); 	// CAM_LL_STOP_FADE_TO_BLACK_LUMA
    MT9M114_write_cmos_sensor(0xC94E, 0x00A6); 	// CAM_LL_CLUSTER_DC_TH_BM
    MT9M114_write_cmos_sensor_8(0xC950, 0x05); 	// CAM_LL_CLUSTER_DC_GATE_PERCENTAGE
    MT9M114_write_cmos_sensor_8(0xC951, 0x40); 	// CAM_LL_SUMMING_SENSITIVITY_FACTOR
    MT9M114_write_cmos_sensor(0xC952, 0x0020); 	// CAM_LL_START_TARGET_LUMA_BM
    MT9M114_write_cmos_sensor(0xC954, 0x009A); 	// CAM_LL_STOP_TARGET_LUMA_BM
    MT9M114_write_cmos_sensor_8(0xC90D, 0x90); 	// CAM_AWB_K_G_L
    MT9M114_write_cmos_sensor_8(0xC90E, 0x90); 	// CAM_AWB_K_B_L
    MT9M114_write_cmos_sensor_8(0xC90F, 0x88); 	// CAM_AWB_K_R_R
    MT9M114_write_cmos_sensor_8(0xC910, 0x88); 	// CAM_AWB_K_G_R
    MT9M114_write_cmos_sensor_8(0xC911, 0x88); 	// CAM_AWB_K_B_R
    MT9M114_write_cmos_sensor_8(0xC942, 0x38); 	// CAM_LL_START_CONTRAST_GRADIENT
    MT9M114_write_cmos_sensor(0xC93C, 0x0020); 	// CAM_LL_START_CONTRAST_BM
    MT9M114_write_cmos_sensor(0xC93E, 0x009A); 	// CAM_LL_STOP_CONTRAST_BM
    MT9M114_write_cmos_sensor_8(0xC878, 0x00); 	// CAM_AET_AEMODE
    MT9M114_write_cmos_sensor_8(0xC87B, 0x15); 	// CAM_AET_TARGET_AVERAGE_LUMA_DARK
    MT9M114_write_cmos_sensor(0xC81C, 0x01F8); 	// CAM_SENSOR_CFG_MAX_ANALOG_GAIN

    // gamma 
    // noise reduction curve mode
    MT9M114_write_cmos_sensor(0x098E, 0xBC07); 	// LOGICAL_ADDRESS_ACCESS [LL_GAMMA_SELECT]
    MT9M114_write_cmos_sensor_8(0xBC07, 0x02); 	// LL_GAMMA_SELECT
    MT9M114_write_cmos_sensor_8(0xBC1D, 0x00); 	// LL_GAMMA_NRCURVE_0
    MT9M114_write_cmos_sensor_8(0xBC1E, 0x05); 	// LL_GAMMA_NRCURVE_1
    MT9M114_write_cmos_sensor_8(0xBC1F, 0x11); 	// LL_GAMMA_NRCURVE_2
    MT9M114_write_cmos_sensor_8(0xBC20, 0x31); 	// LL_GAMMA_NRCURVE_3
    MT9M114_write_cmos_sensor_8(0xBC21, 0x5C); 	// LL_GAMMA_NRCURVE_4
    MT9M114_write_cmos_sensor_8(0xBC22, 0x77); 	// LL_GAMMA_NRCURVE_5
    MT9M114_write_cmos_sensor_8(0xBC23, 0x8E); 	// LL_GAMMA_NRCURVE_6
    MT9M114_write_cmos_sensor_8(0xBC24, 0x9F); 	// LL_GAMMA_NRCURVE_7
    MT9M114_write_cmos_sensor_8(0xBC25, 0xAE); 	// LL_GAMMA_NRCURVE_8
    MT9M114_write_cmos_sensor_8(0xBC26, 0xBA); 	// LL_GAMMA_NRCURVE_9
    MT9M114_write_cmos_sensor_8(0xBC27, 0xC5); 	// LL_GAMMA_NRCURVE_10
    MT9M114_write_cmos_sensor_8(0xBC28, 0xCF); 	// LL_GAMMA_NRCURVE_11
    MT9M114_write_cmos_sensor_8(0xBC29, 0xD8); 	// LL_GAMMA_NRCURVE_12
    MT9M114_write_cmos_sensor_8(0xBC2A, 0xE0); 	// LL_GAMMA_NRCURVE_13
    MT9M114_write_cmos_sensor_8(0xBC2B, 0xE7); 	// LL_GAMMA_NRCURVE_14
    MT9M114_write_cmos_sensor_8(0xBC2C, 0xED); 	// LL_GAMMA_NRCURVE_15
    MT9M114_write_cmos_sensor_8(0xBC2D, 0xF4); 	// LL_GAMMA_NRCURVE_16
    MT9M114_write_cmos_sensor_8(0xBC2E, 0xFA); 	// LL_GAMMA_NRCURVE_17
    MT9M114_write_cmos_sensor_8(0xBC2F, 0xFF); 	// LL_GAMMA_NRCURVE_18

    MT9M114_write_cmos_sensor(0xC924, 0x0000); 	// CAM_LL_LLMODE

    MT9M114_write_cmos_sensor(0xC87C, 0x005A); 	// CAM_AET_BLACK_CLIPPING_TARGET
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor(0xC984, 0x8040); 	// CAM_PORT_OUTPUT_CONTROL
    MT9M114_write_cmos_sensor(0x001E, 0x0777); 	// PAD_SLEW
    MT9M114_write_cmos_sensor(0x098E, 0x2802); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor(0xA802, 0x0008); 	// AE_TRACK_MODE
    MT9M114_write_cmos_sensor(0xA808, 0x0006); 	// AE_TRACK_MODE
    MT9M114_write_cmos_sensor_8(0xC908, 0x01); 	// CAM_AWB_SKIP_FRAMES
    MT9M114_write_cmos_sensor_8(0xC879, 0x01); 	// CAM_AET_SKIP_FRAMES
    MT9M114_write_cmos_sensor_8(0xC909, 0x02); 	// CAM_AWB_AWBMODE
    MT9M114_write_cmos_sensor_8(0xA80A, 0x18); 	// AE_TRACK_AE_TRACKING_DAMPENING_SPEED
    MT9M114_write_cmos_sensor_8(0xA80B, 0x18); 	// AE_TRACK_AE_DAMPENING_SPEED
    MT9M114_write_cmos_sensor_8(0xAC16, 0x18); 	// AWB_PRE_AWB_RATIOS_TRACKING_SPEED
    MT9M114_write_cmos_sensor_8(0xC878, 0x00); 	// CAM_AET_AEMODE

    // ompiztion 
    MT9M114_write_cmos_sensor_8(0xC87A, 0x38); 	// CAM_AET_TARGET_AVERAGE_LUMA
    MT9M114_write_cmos_sensor_8(0x8404, 0x5C); 	// CAM_AET_TARGET_AVERAGE_LUMA
    MT9M114_write_cmos_sensor_8(0xC88B, 0x32); 	// FLICKER
    MT9M114_write_cmos_sensor(0xC882, 0x0080); 	// max_dgain
    MT9M114_write_cmos_sensor(0xC890, 0x0060); 	// FLICKER
    MT9M114_write_cmos_sensor(0xC886, 0x0100); 	// FLICKER
    MT9M114_write_cmos_sensor(0xC81e, 0x0020); 	// max_dgain
    MT9M114_write_cmos_sensor(0xC820, 0x0040); 	// FLICKER
    MT9M114_write_cmos_sensor(0xC822, 0x00c0); 	// FLICKER
    MT9M114_write_cmos_sensor_8(0xC92F, 0x02); 	// CAM_LL_START_AP_GAIN
    MT9M114_write_cmos_sensor_8(0xC944, 0x50); 	// CAM_LL_START_CONTRAST_LUMA_PERCENTAGE
    MT9M114_write_cmos_sensor_8(0xC942, 0x43); 	// CAM_LL_START_CONTRAST_GRADIENT
    MT9M114_write_cmos_sensor_8(0xb402, 0x02); 	// CAM_LL_START_CONTRAST_GRADIENT
    // [Adaptive AE for Lowlights]1: REG=0x98E, 0	
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    // [Adaptive AE for Lowlights]2: REG=0xA404, 0x0003 	
    MT9M114_write_cmos_sensor(0xA404, 0x0003); 	// AE_RULE_ALGO

    MT9M114_write_cmos_sensor_8(0xB00D, 0x20); 	// CAM_LL_START_CONTRAST_GRADIENT
    //REG= 0xB00e, 0x34 	// CAM_LL_START_CONTRAST_GRADIENT
    MT9M114_write_cmos_sensor(0xC87C, 0x005A); 	// CAM_AET_BLACK_CLIPPING_TARGET
    MT9M114_write_cmos_sensor_8(0xC87A, 0x3b); 	// CAM_AET_BLACK_CLIPPING_TARGET  ����57 
    MT9M114_write_cmos_sensor_8(0xC92A, 0xA0); 	// CAM_LL_START_SATURATION

    MT9M114_write_cmos_sensor_8(0xB42A, 0x05); 	// CCM_DELTA_GAIN
    MT9M114_write_cmos_sensor_8(0xA80A, 0x20); 	// AE_TRACK_AE_TRACKING_DAMPENING_SPEED

    // change config 
    //MT9M114_write_cmos_sensor(0x098E, 0xDC00);		// LOGICAL_ADDRESS_ACCESS
    //MT9M114_write_cmos_sensor_8(0xDC00, 0x28);		// sysmgr_next_state
    //MT9M114_write_cmos_sensor(0x0080, 0x8002);		// COMMAND_REGISTER
    //mDELAY(100);
    
#endif
    MT9M114_change_config_command();

    SENSORDB("[Exit]: %s \n", __FUNCTION__);
    
}

void MT9M114_Set_Mirror_Flip(kal_uint8 image_mirror)
{
    kal_uint16 reg_value, new_value, mirror;

    spin_lock(&mt9m114yuv_drv_lock);
    mirror = MT9M114_para.iMirror;
    spin_unlock(&mt9m114yuv_drv_lock);

    if (mirror == image_mirror)
    {
        SENSORDB("[%s]: , same Mirror Flip setting[%d], Don't set \n", __FUNCTION__, image_mirror);
        return KAL_TRUE;
    }

    SENSORDB("[Enter]: %s, image_mirror = %d \n", __FUNCTION__, image_mirror);

    reg_value = MT9M114_read_cmos_sensor(0xC834);
    
    switch (image_mirror)
    {
        case IMAGE_NORMAL:
            new_value = (reg_value && 0xFFFC) | 0x0000;
            break;
        case IMAGE_H_MIRROR:
            new_value = (reg_value && 0xFFFC) | 0x0001;
            break;
        case IMAGE_V_MIRROR:
            new_value = (reg_value && 0xFFFC) | 0x0002;
            break;
        case IMAGE_HV_MIRROR:
            new_value = (reg_value && 0xFFFC) | 0x0003;
            break;
        default:
            // normal
            new_value = (reg_value && 0xFFFC) | 0x0000;
            break;
    }

    MT9M114_write_cmos_sensor(0xC834, new_value);

    //change config
    //MT9M114_write_cmos_sensor(0x098E, 0xDC00);		// LOGICAL_ADDRESS_ACCESS
    //MT9M114_write_cmos_sensor_8(0xDC00, 0x28);		// sysmgr_next_state
    //MT9M114_write_cmos_sensor(0x0080, 0x8002);		// COMMAND_REGISTER
    MT9M114_change_config_command();

    spin_lock(&mt9m114yuv_drv_lock);
    MT9M114_para.iMirror = image_mirror;
    spin_unlock(&mt9m114yuv_drv_lock);

    SENSORDB("[Exit]: %s \n", __FUNCTION__);
}


/*************************************************************************
* FUNCTION
*   MT9M114Preview
*
* DESCRIPTION
*   This function start the sensor preview.
*
* PARAMETERS
*  
* 
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 MT9M114Preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                    MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    SENSORDB("[Enter]: %s \n", __FUNCTION__);

    spin_lock(&mt9m114yuv_drv_lock);
    MT9M114_para.sensorMode = SENSOR_MODE_PREVIEW;
    spin_unlock(&mt9m114yuv_drv_lock);

#ifdef NONE_MTK_CAMERA_SETTING
    if(MT9M114_READ_ID == MT9M114_READ_ID_1)
        MT9M114_LiteOn_Preview();
    else
    	MT9M114_LiteOn_Preview();
        
#else
    // set Mirror & Flip
#endif
    MT9M114_Set_Mirror_Flip(sensor_config_data->SensorImageMirror);

    SENSORDB("[Exit]: %s \n", __FUNCTION__);
}

/*************************************************************************
* FUNCTION
*   MT9M114Capture
*
* DESCRIPTION
*   This function start the sensor capture mode.
*
* PARAMETERS
*  
* 
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 MT9M114Capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                             MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    SENSORDB("[Enter]: %s \n", __FUNCTION__);

    spin_lock(&mt9m114yuv_drv_lock);
    if(MT9M114_para.sensorMode == SENSOR_MODE_CAPTURE)
    {
        spin_unlock(&mt9m114yuv_drv_lock);
        SENSORDB("Entering in burstshot mode!");
        return KAL_TRUE;
    }
    MT9M114_para.sensorMode = SENSOR_MODE_CAPTURE;
    spin_unlock(&mt9m114yuv_drv_lock);

#ifdef NONE_MTK_CAMERA_SETTING
    if(MT9M114_READ_ID == MT9M114_READ_ID_1)
        MT9M114_LiteOn_Capture();
    else
        MT9M114_LiteOn_Capture();
#else
    // set Mirror & Flip
#endif    
    MT9M114_Set_Mirror_Flip(sensor_config_data->SensorImageMirror);

    SENSORDB("[Exit]: %s \n", __FUNCTION__);
}

/*************************************************************************
* FUNCTION
*   MT9M114_night_mode
*
* DESCRIPTION
*   This function change the sensor mode.
*
* PARAMETERS
*  
* 
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void MT9M114NightMode(kal_bool enable)
{
    SENSORDB("[Enter]: %s \n", __FUNCTION__);
#ifdef NONE_MTK_CAMERA_SETTING
    if (enable) 
    {
        SENSORDB("[%s]: enable night mode \n", __FUNCTION__);

        MT9M114_write_cmos_sensor(0x098E, 0x0000);      // LOGICAL_ADDRESS_ACCESS
        MT9M114_write_cmos_sensor(0xC80E, 0x00DB);      //cam_sensor_cfg_fine_integ_time_min
        MT9M114_write_cmos_sensor(0xC810, 0x05B3);		//cam_sensor_cfg_fine_integ_time_max = 1459
        MT9M114_write_cmos_sensor(0xC812, 0x03EE);		//cam_sensor_cfg_frame_length_lines = 1006
        MT9M114_write_cmos_sensor(0xC814, 0x0636);		//cam_sensor_cfg_line_length_pck = 1590
        MT9M114_write_cmos_sensor(0xC88C, 0x1E02);		//cam_aet_max_frame_rate = 7682
        MT9M114_write_cmos_sensor(0xC88E, 0x0780);      //cam_aet_min_frame_rate =7.5fps
        MT9M114_write_cmos_sensor(0xC882, 0x00C0);      // max_dgain        

        //change config
        MT9M114_change_config_command();
    } else 
    {
        SENSORDB("[%s]: disable night mode \n", __FUNCTION__);

        MT9M114_write_cmos_sensor(0x098E, 0x0000);      // LOGICAL_ADDRESS_ACCESS
        MT9M114_write_cmos_sensor(0xC80E, 0x00DB);      //cam_sensor_cfg_fine_integ_time_min
        MT9M114_write_cmos_sensor(0xC810, 0x05B3);		//cam_sensor_cfg_fine_integ_time_max = 1459
        MT9M114_write_cmos_sensor(0xC812, 0x03EE);		//cam_sensor_cfg_frame_length_lines = 1006
        MT9M114_write_cmos_sensor(0xC814, 0x0636);		//cam_sensor_cfg_line_length_pck = 1590
        MT9M114_write_cmos_sensor(0xC88C, 0x1E02);		//cam_aet_max_frame_rate = 7682
        MT9M114_write_cmos_sensor(0xC88E, 0x0F00);      //cam_aet_min_frame_rate = 15fps
        MT9M114_write_cmos_sensor(0xC882, 0x00C0);      // max_dgain

        //change config
        MT9M114_change_config_command();
    }
#else
    if (enable) 
    {
        SENSORDB("[%s]: enable night mode \n", __FUNCTION__);

        MT9M114_write_cmos_sensor(0x098E, 0x0000);      // LOGICAL_ADDRESS_ACCESS
        MT9M114_write_cmos_sensor(0xC80E, 0x00DB);      //cam_sensor_cfg_fine_integ_time_min
        MT9M114_write_cmos_sensor(0xC810, 0x05C1);      //cam_sensor_cfg_fine_integ_time_max
        MT9M114_write_cmos_sensor(0xC812, 0x03F3);      //cam_sensor_cfg_frame_length_lines
        MT9M114_write_cmos_sensor(0xC814, 0x0644);      //cam_sensor_cfg_line_length_pck
        MT9M114_write_cmos_sensor(0xC88C, 0x1D99);      //cam_aet_max_frame_rate
        MT9M114_write_cmos_sensor(0xC88E, 0x0500);      //cam_aet_min_frame_rate
        MT9M114_write_cmos_sensor(0xC882, 0x00C0);      // max_dgain        

        //change config
        MT9M114_change_config_command();
    } else 
    {
        SENSORDB("[%s]: disable night mode \n", __FUNCTION__);

        MT9M114_write_cmos_sensor(0x098E, 0x0000);      // LOGICAL_ADDRESS_ACCESS
        MT9M114_write_cmos_sensor(0xC80E, 0x00DB);      //cam_sensor_cfg_fine_integ_time_min
        MT9M114_write_cmos_sensor(0xC810, 0x05C1);      //cam_sensor_cfg_fine_integ_time_max
        MT9M114_write_cmos_sensor(0xC812, 0x03F3);      //cam_sensor_cfg_frame_length_lines
        MT9M114_write_cmos_sensor(0xC814, 0x0644);      //cam_sensor_cfg_line_length_pck
        MT9M114_write_cmos_sensor(0xC88C, 0x1D99);      //cam_aet_max_frame_rate
        MT9M114_write_cmos_sensor(0xC88E, 0x0A00);      //cam_aet_min_frame_rate
        MT9M114_write_cmos_sensor(0xC882, 0x00C0);      // max_dgain

        //change config
        MT9M114_change_config_command();
    }
#endif
    SENSORDB("[Exit]: %s \n", __FUNCTION__);
}



/*************************************************************************
* FUNCTION
*   MT9M114YUVSetVideoMode
*
* DESCRIPTION
*   This function set the sensor video mode.
*
* PARAMETERS
*  
* 
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 MT9M114YUVSetVideoMode(UINT16 u2FrameRate)
{
    SENSORDB("[Enter]: %s \n", __FUNCTION__);

	if (u2FrameRate == 30)
    {
    	SENSORDB("[%s]: FrameRate: %d fps \n", __FUNCTION__, u2FrameRate);

        MT9M114_write_cmos_sensor(0x098E, 0x4812);
        MT9M114_write_cmos_sensor(0xC812, 0x03F3);
        MT9M114_write_cmos_sensor(0xC88C, 0x1D99);
        MT9M114_write_cmos_sensor(0xC88E, 0x1D99);
    }
    else if (u2FrameRate == 15)       
    {
		SENSORDB("[%s]: FrameRate: %d fps \n", __FUNCTION__, u2FrameRate);

        MT9M114_write_cmos_sensor(0x098E, 0x4812);
        MT9M114_write_cmos_sensor(0xC812, 0x07CB);
        MT9M114_write_cmos_sensor(0xC88C, 0x0F00);
        MT9M114_write_cmos_sensor(0xC88E, 0x0F00);
    }
    else 
    {
        printk("[%s]: Wrong frame rate setting \n", __FUNCTION__);
        return KAL_FALSE;
    }   
    
    //change config
    MT9M114_change_config_command();

    spin_lock(&mt9m114yuv_drv_lock);
    MT9M114_para.iFrameRate = u2FrameRate;
    spin_unlock(&mt9m114yuv_drv_lock);

    SENSORDB("[Exit]: %s \n", __FUNCTION__);

    return KAL_TRUE;
}

/*************************************************************************
* FUNCTION
*   MT9M114_set_param_scene_mode
*
* DESCRIPTION
*   scene mode setting.
*
* PARAMETERS
*  
* 
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL MT9M114_set_param_scene_mode(UINT16 para)
{
    SENSORDB("[Enter]: %s, scene mode = %d \n", __FUNCTION__, para);

    switch (para)
    {
        case SCENE_MODE_OFF:
            MT9M114NightMode(FALSE);
            break;
        case SCENE_MODE_NIGHTSCENE:
            MT9M114NightMode(TRUE);
            break;
        default:
            SENSORDB("[Error]: %s, not support scene mode = %d \n", __FUNCTION__, para);
            return KAL_FALSE;
    }

    spin_lock(&mt9m114yuv_drv_lock);
    MT9M114_para.iNightMode = para;
    spin_unlock(&mt9m114yuv_drv_lock);

    SENSORDB("[Exit]: %s \n", __FUNCTION__);

    return KAL_TRUE;
}


/*************************************************************************
* FUNCTION
*	MT9M114_set_param_wb
*
* DESCRIPTION
*	wb setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL MT9M114_set_param_wb(UINT16 para)
{
    kal_uint16 wb;

    spin_lock(&mt9m114yuv_drv_lock);
    wb = MT9M114_para.iWB;
    spin_unlock(&mt9m114yuv_drv_lock);
    
    if (wb == para)
    {
        SENSORDB("[%s]: , same wb setting[%d], Don't set \n", __FUNCTION__, para);
        return KAL_TRUE;
    }

    SENSORDB("[Enter]: %s, wb value = %d \n", __FUNCTION__, para);

#ifdef NONE_MTK_CAMERA_SETTING
    if(MT9M114_READ_ID == MT9M114_READ_ID_1)
    {  
        switch (para)
        { 
            case AWB_MODE_AUTO:
                MT9M114_LiteOn_WB_AWB(); 
                break;
            case AWB_MODE_CLOUDY_DAYLIGHT:
                MT9M114_LiteOn_WB_D65();  
                break;
            case AWB_MODE_DAYLIGHT:
                MT9M114_LiteOn_WB_D50(); 
                break;
            case AWB_MODE_INCANDESCENT:
                MT9M114_LiteOn_WB_A();
                break;
            case AWB_MODE_FLUORESCENT:
                MT9M114_LiteOn_WB_TL84(); 
                break;
            case AWB_MODE_TUNGSTEN:
                MT9M114_LiteOn_WB_H();
                break;
            default:
                SENSORDB("[Error]: %s, not support wb value = %d \n", __FUNCTION__, para);
                return KAL_FALSE;
        } 
    }   
    else
    {  
        switch (para)
        { 
            case AWB_MODE_AUTO:
                MT9M114_LiteOn_WB_AWB(); 
                break;
            case AWB_MODE_CLOUDY_DAYLIGHT:
                MT9M114_LiteOn_WB_D65();  
                break;
            case AWB_MODE_DAYLIGHT:
                MT9M114_LiteOn_WB_D50(); 
                break;
            case AWB_MODE_INCANDESCENT:
                MT9M114_LiteOn_WB_A();
                break;
            case AWB_MODE_FLUORESCENT:
                MT9M114_LiteOn_WB_TL84(); 
                break;
            case AWB_MODE_TUNGSTEN:
                MT9M114_LiteOn_WB_H();
                break;
            default:
                SENSORDB("[Error]: %s, not support wb value = %d \n", __FUNCTION__, para);
                return KAL_FALSE;
        } 
    }
#else
    switch (para)
    {
        case AWB_MODE_AUTO:
            MT9M114_write_cmos_sensor(0x098E, 0xCC01);
            MT9M114_write_cmos_sensor_8(0xCC01, 0x01);      // UVC_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL
            break;
        case AWB_MODE_CLOUDY_DAYLIGHT:
            // coudy 3300
            MT9M114_write_cmos_sensor(0x098E, 0xCC01);
            MT9M114_write_cmos_sensor_8(0xCC01, 0x00);      // UVC_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL        
            mDELAY(50);
            MT9M114_write_cmos_sensor(0xCC18, 0x0CE4);      // UVC_WHITE_BALANCE_TEMPERATURE_CONTROL        
            break;
        case AWB_MODE_DAYLIGHT:
            // D65  6500
            MT9M114_write_cmos_sensor(0x098E, 0xCC01);
            MT9M114_write_cmos_sensor_8(0xCC01, 0x00);      // UVC_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL        
            mDELAY(50);
            MT9M114_write_cmos_sensor(0xCC18, 0x1964);      // UVC_WHITE_BALANCE_TEMPERATURE_CONTROL
            break;
        case AWB_MODE_INCANDESCENT:
            // Alight 2856
            MT9M114_write_cmos_sensor(0x098E, 0xCC01);
            MT9M114_write_cmos_sensor_8(0xCC01, 0x00);      // UVC_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL        
            mDELAY(50);
            MT9M114_write_cmos_sensor(0xCC18, 0x0B28);      // UVC_WHITE_BALANCE_TEMPERATURE_CONTROL
            break;
        case AWB_MODE_FLUORESCENT:
            // CWF 3850
            MT9M114_write_cmos_sensor(0x098E, 0xCC01);
            MT9M114_write_cmos_sensor_8(0xCC01, 0x00);      // UVC_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL        
            mDELAY(50);
            MT9M114_write_cmos_sensor(0xCC18, 0x0F0A);      // UVC_WHITE_BALANCE_TEMPERATURE_CONTROL
            break;
        case AWB_MODE_TUNGSTEN:
            // Alight 2856
            MT9M114_write_cmos_sensor(0x098E, 0xCC01);
            MT9M114_write_cmos_sensor_8(0xCC01, 0x00);      // UVC_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL        
            mDELAY(50);
            MT9M114_write_cmos_sensor(0xCC18, 0x0BB8);      // UVC_WHITE_BALANCE_TEMPERATURE_CONTROL
            break;
        default:
            SENSORDB("[Error]: %s, not support wb value = %d \n", __FUNCTION__, para);
            return KAL_FALSE;
    }
#endif

    spin_lock(&mt9m114yuv_drv_lock);
    MT9M114_para.iWB = para;
    spin_unlock(&mt9m114yuv_drv_lock);

    SENSORDB("[Exit]: %s \n", __FUNCTION__);

    return KAL_TRUE;
}


/*************************************************************************
* FUNCTION
*	MT9M114_set_param_effect
*
* DESCRIPTION
*	effect setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL MT9M114_set_param_effect(UINT16 para)
{
    kal_uint16 effect;

    spin_lock(&mt9m114yuv_drv_lock);
    effect = MT9M114_para.iEffect;
    spin_unlock(&mt9m114yuv_drv_lock);
    
    if (effect == para)
    {
        SENSORDB("[%s]: , same effect[%d], Don't set \n", __FUNCTION__, para);
        return KAL_TRUE;
    }

    SENSORDB("[Enter]: %s, effect = %d \n", __FUNCTION__, para);

    switch (para)
    {
        case MEFFECT_OFF:
            MT9M114_write_cmos_sensor(0x098E, 0xC874);      // LOGICAL_ADDRESS_ACCESS [CAM_SFX_CONTROL]
            MT9M114_write_cmos_sensor_8(0xC874, 0x00);      // CAM_SFX_CONTROL
            break;
        case MEFFECT_NEGATIVE:
            MT9M114_write_cmos_sensor(0x098E, 0xC874);      // LOGICAL_ADDRESS_ACCESS [CAM_SFX_CONTROL]
            MT9M114_write_cmos_sensor_8(0xC874, 0x03);      // CAM_SFX_CONTROL
            break;
        case MEFFECT_SEPIA:
            MT9M114_write_cmos_sensor(0x098E, 0xC874);      // LOGICAL_ADDRESS_ACCESS [CAM_SFX_CONTROL]
            MT9M114_write_cmos_sensor_8(0xC874, 0x02);      // CAM_SFX_CONTROL
            MT9M114_write_cmos_sensor_8(0xC876, 0x23);      // CAM_SFX_SEPIA_CR
            MT9M114_write_cmos_sensor_8(0xC877, 0xB2);      // CAM_SFX_SEPIA_CB
            break;
        case MEFFECT_SEPIAGREEN:
            MT9M114_write_cmos_sensor(0x098E, 0xC874);      // LOGICAL_ADDRESS_ACCESS [CAM_SFX_CONTROL]
            MT9M114_write_cmos_sensor_8(0xC874, 0x02);      // CAM_SFX_CONTROL
            MT9M114_write_cmos_sensor_8(0xC876, 0xEC);      // CAM_SFX_SEPIA_CR
            MT9M114_write_cmos_sensor_8(0xC877, 0xEC);      // CAM_SFX_SEPIA_CB
            break;
        case MEFFECT_SEPIABLUE:
            MT9M114_write_cmos_sensor(0x098E, 0xC874);      // LOGICAL_ADDRESS_ACCESS [CAM_SFX_CONTROL]
            MT9M114_write_cmos_sensor_8(0xC874, 0x02);      // CAM_SFX_CONTROL
            MT9M114_write_cmos_sensor_8(0xC876, 0x00);      // CAM_SFX_SEPIA_CR
            MT9M114_write_cmos_sensor_8(0xC877, 0x20);      // CAM_SFX_SEPIA_CB
            break;
        case MEFFECT_MONO:
            MT9M114_write_cmos_sensor(0x098E, 0xC874);      // LOGICAL_ADDRESS_ACCESS [CAM_SFX_CONTROL]
            MT9M114_write_cmos_sensor_8(0xC874, 0x01);      // CAM_SFX_CONTROL
            break;

        default:
            SENSORDB("[Error] %s, not support effect = %d \n", __FUNCTION__, para);
            return KAL_FALSE;
    }

    // refresh command
    MT9M114_write_cmos_sensor(0x0080, 0x8004);		// COMMAND_REGISTER

    spin_lock(&mt9m114yuv_drv_lock);
    MT9M114_para.iEffect = para;
    spin_unlock(&mt9m114yuv_drv_lock);

    SENSORDB("[Exit]: %s \n", __FUNCTION__);

	return KAL_TRUE;
}

/*************************************************************************
* FUNCTION
*	MT9M114_set_param_exposure
*
* DESCRIPTION
*	exposure setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL MT9M114_set_param_exposure(UINT16 para)
{
    kal_uint16 base_target = 0, exposureValue;

    spin_lock(&mt9m114yuv_drv_lock);
    exposureValue = MT9M114_para.iEV;
    spin_unlock(&mt9m114yuv_drv_lock);

    if (exposureValue == para)
    {
        SENSORDB("[%s]: , same exposure[%d], Don't set \n", __FUNCTION__, para);
        return KAL_TRUE;
    }

    SENSORDB("[Enter]: %s, exposure level = %d \n", __FUNCTION__, para);

#ifdef NONE_MTK_CAMERA_SETTING
    switch (para)
    {
        case AE_EV_COMP_20:
            MT9M114_write_cmos_sensor(0x098E, 0xC87A);      // LOGICAL_ADDRESS_ACCESS [CAM_AET_TARGET_AVERAGE_LUMA]
            MT9M114_write_cmos_sensor_8(0xC87A, 0x5F);      // CAM_AET_TARGET_AVERAGE_LUMA
            break;
        case AE_EV_COMP_10:
            MT9M114_write_cmos_sensor(0x098E, 0xC87A);      // LOGICAL_ADDRESS_ACCESS [CAM_AET_TARGET_AVERAGE_LUMA]
            MT9M114_write_cmos_sensor_8(0xC87A, 0x4F);      // CAM_AET_TARGET_AVERAGE_LUMA
            break;
        case AE_EV_COMP_00:
            MT9M114_write_cmos_sensor(0x098E, 0xC87A);      // LOGICAL_ADDRESS_ACCESS [CAM_AET_TARGET_AVERAGE_LUMA]
            MT9M114_write_cmos_sensor_8(0xC87A, 0x3F);      // CAM_AET_TARGET_AVERAGE_LUMA
            break;
        case AE_EV_COMP_n10:
            MT9M114_write_cmos_sensor(0x098E, 0xC87A);      // LOGICAL_ADDRESS_ACCESS [CAM_AET_TARGET_AVERAGE_LUMA]
            MT9M114_write_cmos_sensor_8(0xC87A, 0x2F);      // CAM_AET_TARGET_AVERAGE_LUMA
            break;
        case AE_EV_COMP_n20:
            MT9M114_write_cmos_sensor(0x098E, 0xC87A);      // LOGICAL_ADDRESS_ACCESS [CAM_AET_TARGET_AVERAGE_LUMA]
            MT9M114_write_cmos_sensor_8(0xC87A, 0x1F);      // CAM_AET_TARGET_AVERAGE_LUMA
            break;

        default:
            SENSORDB("[Error] %s, not support exposure = %d \n", __FUNCTION__, para);
            return KAL_FALSE;      
    }
#else    
    switch (para)
    {
        case AE_EV_COMP_20:
            MT9M114_write_cmos_sensor(0x098E, 0xC87A);      // LOGICAL_ADDRESS_ACCESS [CAM_AET_TARGET_AVERAGE_LUMA]
            MT9M114_write_cmos_sensor_8(0xC87A, 0x57);      // CAM_AET_TARGET_AVERAGE_LUMA
            break;
        case AE_EV_COMP_10:
            MT9M114_write_cmos_sensor(0x098E, 0xC87A);      // LOGICAL_ADDRESS_ACCESS [CAM_AET_TARGET_AVERAGE_LUMA]
            MT9M114_write_cmos_sensor_8(0xC87A, 0x47);      // CAM_AET_TARGET_AVERAGE_LUMA
            break;
        case AE_EV_COMP_00:
            MT9M114_write_cmos_sensor(0x098E, 0xC87A);      // LOGICAL_ADDRESS_ACCESS [CAM_AET_TARGET_AVERAGE_LUMA]
            MT9M114_write_cmos_sensor_8(0xC87A, 0x37);      // CAM_AET_TARGET_AVERAGE_LUMA
            break;
        case AE_EV_COMP_n10:
            MT9M114_write_cmos_sensor(0x098E, 0xC87A);      // LOGICAL_ADDRESS_ACCESS [CAM_AET_TARGET_AVERAGE_LUMA]
            MT9M114_write_cmos_sensor_8(0xC87A, 0x2D);      // CAM_AET_TARGET_AVERAGE_LUMA
            break;
        case AE_EV_COMP_n20:
            MT9M114_write_cmos_sensor(0x098E, 0xC87A);      // LOGICAL_ADDRESS_ACCESS [CAM_AET_TARGET_AVERAGE_LUMA]
            MT9M114_write_cmos_sensor_8(0xC87A, 0x1E);      // CAM_AET_TARGET_AVERAGE_LUMA
            break;

        default:
            SENSORDB("[Error] %s, not support exposure = %d \n", __FUNCTION__, para);
            return KAL_FALSE;      
    }
#endif
    spin_lock(&mt9m114yuv_drv_lock);
    MT9M114_para.iEV = para;
    spin_unlock(&mt9m114yuv_drv_lock);

	SENSORDB("[Exit]: %s \n", __FUNCTION__);

    return KAL_TRUE;
}

/*************************************************************************
* FUNCTION
*	MT9M114_set_param_banding
*
* DESCRIPTION
*	banding setting.
*
* PARAMETERS
*	none
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
BOOL MT9M114_set_param_banding(UINT16 para)
{
    kal_uint16 banding;

    spin_lock(&mt9m114yuv_drv_lock);
    banding = MT9M114_para.iBanding;
    spin_unlock(&mt9m114yuv_drv_lock);

    if (banding == para)
    {
        SENSORDB("[%s]: , same banding setting[%d], Don't set \n", __FUNCTION__, para);
        return KAL_TRUE;
    }

    SENSORDB("[Enter]: %s, banding para = %d \n", __FUNCTION__, para);

#ifdef NONE_MTK_CAMERA_SETTING
    if(MT9M114_READ_ID == MT9M114_READ_ID_1)
    {
        switch (para)
        {
            case AE_FLICKER_MODE_50HZ:
                MT9M114_LiteOn_Anti_Banding_50HZ();
                break;
            case AE_FLICKER_MODE_60HZ:
                MT9M114_LiteOn_Anti_Banding_60HZ();
                break;
            default:
                SENSORDB("[Error] %s, not support banding setting = %d \n", __FUNCTION__, para);
                return KAL_FALSE;
        }
    }
    else
    {
        switch (para)
        {
            case AE_FLICKER_MODE_50HZ:
                MT9M114_LiteOn_Anti_Banding_50HZ();
                break;
            case AE_FLICKER_MODE_60HZ:
                MT9M114_LiteOn_Anti_Banding_60HZ();
                break;
            default:
                SENSORDB("[Error] %s, not support banding setting = %d \n", __FUNCTION__, para);
                return KAL_FALSE;
        }
    }
#else
    switch (para)
    {
        case AE_FLICKER_MODE_50HZ:
            MT9M114_write_cmos_sensor(0x098E, 0xC88B);      // LOGICAL_ADDRESS_ACCESS [CAM_AET_FLICKER_FREQ_HZ]
            MT9M114_write_cmos_sensor_8(0xC88A, 0x05);
            MT9M114_write_cmos_sensor_8(0xC88B, 0x32);      // CAM_AET_FLICKER_FREQ_HZ
            break;
        case AE_FLICKER_MODE_60HZ:
            MT9M114_write_cmos_sensor(0x098E, 0xC88B);      // LOGICAL_ADDRESS_ACCESS [CAM_AET_FLICKER_FREQ_HZ]
            MT9M114_write_cmos_sensor_8(0xC88A, 0x05);
            MT9M114_write_cmos_sensor_8(0xC88B, 0x3C);      // CAM_AET_FLICKER_FREQ_HZ
            break;
        default:
            SENSORDB("[Error] %s, not support banding setting = %d \n", __FUNCTION__, para);
            return KAL_FALSE;
    }
#endif
    //change config
    MT9M114_change_config_command();

    spin_lock(&mt9m114yuv_drv_lock);
    MT9M114_para.iBanding = para;
    spin_unlock(&mt9m114yuv_drv_lock);
    
    SENSORDB("[Exit]: %s \n", __FUNCTION__);
    
    return KAL_TRUE;
    
}

UINT32 MT9M114YUVSensorSetting(FEATURE_ID iCmd, UINT32 iPara)
{
    SENSORDB("[Enter]: %s \n", __FUNCTION__);

    switch (iCmd)
    {
        case FID_SCENE_MODE:
            MT9M114_set_param_scene_mode(iPara);
            break;
        case FID_AWB_MODE:
            MT9M114_set_param_wb(iPara);
            break;
        case FID_COLOR_EFFECT:
            MT9M114_set_param_effect(iPara);
            break;
        case FID_AE_EV:
            MT9M114_set_param_exposure(iPara);
            break;
        case FID_AE_FLICKER:
            MT9M114_set_param_banding(iPara);
            break;
        case FID_ZOOM_FACTOR:
            break;
        default:
            break;
    }
    
    SENSORDB("[Exit]: %s \n", __FUNCTION__);

    return TRUE;
}

/*************************************************************************
* FUNCTION
*    MT9M114YUVGetEvAwbRef
*
* DESCRIPTION
*    This function get sensor Ev/Awb (EV05/EV13) for auto scene detect
*
* PARAMETERS
*    Ref
*
* RETURNS
*    None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void MT9M114YUVGetEvAwbRef(UINT32 pSensorAEAWBRefStruct/*PSENSOR_AE_AWB_REF_STRUCT Ref*/)
{
    PSENSOR_AE_AWB_REF_STRUCT Ref = (PSENSOR_AE_AWB_REF_STRUCT)pSensorAEAWBRefStruct;
    
    SENSORDB("[Enter]: %s \n", __FUNCTION__);
    	
    Ref->SensorAERef.AeRefLV05Shutter = 2989;
    Ref->SensorAERef.AeRefLV05Gain = 94 * 4; /* 7.75x, 128 base */
    Ref->SensorAERef.AeRefLV13Shutter = 33;
    Ref->SensorAERef.AeRefLV13Gain = 32 * 4; /* 1x, 128 base */
    Ref->SensorAwbGainRef.AwbRefD65Rgain = 191; /* 1.46875x, 128 base */
    Ref->SensorAwbGainRef.AwbRefD65Bgain = 147; /* 1x, 128 base */
    Ref->SensorAwbGainRef.AwbRefCWFRgain = 164; /* 1.25x, 128 base */
    Ref->SensorAwbGainRef.AwbRefCWFBgain = 241; /* 1.28125x, 128 base */

    SENSORDB("[Exit]: %s \n", __FUNCTION__);
}

/*************************************************************************
* FUNCTION
*    MT9M114YUVGetCurAeAwbInfo
*
* DESCRIPTION
*    This function get sensor cur Ae/Awb for auto scene detect
*
* PARAMETERS
*    Info
*
* RETURNS
*    None
*
* LOCAL AFFECTED
*
*************************************************************************/
static void MT9M114YUVGetCurAeAwbInfo(UINT32 pSensorAEAWBCurStruct/*PSENSOR_AE_AWB_CUR_STRUCT Info*/)
{
    PSENSOR_AE_AWB_CUR_STRUCT Info = (PSENSOR_AE_AWB_CUR_STRUCT)pSensorAEAWBCurStruct;

    kal_uint16 Shutter, Gain, newGain, RGain, BGain;

    SENSORDB("[Enter]: %s \n", __FUNCTION__);

    Shutter = MT9M114_read_cmos_sensor(0x3012);
    Info->SensorAECur.AeCurShutter = Shutter;

    Gain = MT9M114_read_cmos_sensor(0x305E);
    newGain = (((Gain & 0x0080) >> 7) * 3 + 1) * (((Gain & 0x0040) >> 6) * 1 + 1)* (Gain & 0x003F)*4;
    //newGain = ((Gain & 0x8000) >> 15) * 8 + ((Gain & 0x4000) >> 14) * 4 + ((Gain & 0x2000) >> 13) * 8 + ((Gain & 0x1000) >> 12) * 1;

    Info->SensorAECur.AeCurGain = newGain; /* 128 base */

    MT9M114_write_cmos_sensor(0x098E, 0xAC12);
    RGain = MT9M114_read_cmos_sensor(0xAC12);
    Info->SensorAwbGainCur.AwbCurRgain = RGain; /* 128 base */
    MT9M114_write_cmos_sensor(0x098E, 0xAC14);
    BGain = MT9M114_read_cmos_sensor(0xAC14);
    Info->SensorAwbGainCur.AwbCurBgain = BGain; /* 128 base */

    SENSORDB("[Exit]: %s \n", __FUNCTION__);
}

//<2012/12/5-leolee, fix [8317][CQ][BU2SC00138356]
//Cts failed case :android.hardware.cts.CameraTest#testFocusAreas
//Due to Hawk40 front camera has no focus feature, driver return value should be modify from 1 to 0. 
void MT9M114GetAFMaxNumFocusAreas(UINT32 *pFeatureReturnPara32)
{	
    SENSORDB("[Enter]: %s \n", __FUNCTION__);
    
    *pFeatureReturnPara32 = 0;
    SENSORDB("[MT9M114GetAFMaxNumFocusAreas], *pFeatureReturnPara32 = %d\n",  *pFeatureReturnPara32);

    SENSORDB("[Exit]: %s \n", __FUNCTION__);
}

void MT9M114GetAFMaxNumMeteringAreas(UINT32 *pFeatureReturnPara32)
{	
    SENSORDB("[Enter]: %s \n", __FUNCTION__);

    *pFeatureReturnPara32 = 0;    
    SENSORDB("[MT9M114GetAFMaxNumMeteringAreas], *pFeatureReturnPara32 = %d\n",  *pFeatureReturnPara32);

    SENSORDB("[Exit]: %s \n", __FUNCTION__);
}
//>2012/12/5-leolee end
void MT9M114GetExifInfo(UINT32 exifAddr)
{
    SENSOR_EXIF_INFO_STRUCT* pExifInfo = (SENSOR_EXIF_INFO_STRUCT*)exifAddr;

    SENSORDB("[Enter]: %s \n", __FUNCTION__);

    spin_lock(&mt9m114yuv_drv_lock);
    
    pExifInfo->FNumber = 24;
    pExifInfo->AEISOSpeed = AE_ISO_100;
    pExifInfo->AWBMode = MT9M114_para.iWB;
    pExifInfo->CapExposureTime = MT9M114_para.iEV;
    pExifInfo->FlashLightTimeus = 0;
    pExifInfo->RealISOValue = AE_ISO_100;
    
    spin_unlock(&mt9m114yuv_drv_lock);

    SENSORDB("[Exit]: %s \n", __FUNCTION__);
}


/*************************************************************************
* FUNCTION
*	MT9M114Open
*
* DESCRIPTION
*	This function initialize the registers of CMOS sensor
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 MT9M114Open(void)
{
    kal_uint16 sensor_id;

    SENSORDB("[Enter]: %s \n", __FUNCTION__);

    sensor_id = MT9M114GetSensorID();
    
    SENSORDB("MT9M114Open sensor_id is %x \n", sensor_id);
    
    if (sensor_id != MT9M114_SENSOR_ID)
    {
        SENSORDB("Error: read sensor ID fail\n");
        return ERROR_SENSOR_CONNECT_FAIL;
    }

    /* Set initail para, please sync with initial setting */
    MT9M114InitialPara();
    
    /* Apply sensor initail setting */
    MT9M114InitialSetting();

    SENSORDB("[Exit]: %s \n", __FUNCTION__);
    
	return ERROR_NONE;
} /* MT9M114Open() */

/*************************************************************************
* FUNCTION
*	MT9M114Close
*
* DESCRIPTION
*	This function is to turn off sensor module power.
*
* PARAMETERS
*	None
*
* RETURNS
*	None
*
* GLOBALS AFFECTED
*
*************************************************************************/
UINT32 MT9M114Close(void)
{
    
    return ERROR_NONE;
} /* MT9M114Close() */


UINT32 MT9M114GetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution)
{
    SENSORDB("[Enter]: %s \n", __FUNCTION__);

    pSensorResolution->SensorPreviewWidth = MT9M114_IMAGE_SENSOR_PV_WIDTH - 8;
    pSensorResolution->SensorPreviewHeight = MT9M114_IMAGE_SENSOR_PV_HEIGHT - 6;
    pSensorResolution->SensorFullWidth = MT9M114_IMAGE_SENSOR_FULL_WIDTH - 8;
    pSensorResolution->SensorFullHeight = MT9M114_IMAGE_SENSOR_FULL_HEIGHT - 6;

    SENSORDB("[Exit]: %s \n", __FUNCTION__);
    
	return ERROR_NONE;
} /* MT9M114GetResolution() */

UINT32 MT9M114GetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId,
					  MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
    SENSORDB("[Enter]: %s, ScenarioId = %d \n", __FUNCTION__, ScenarioId);

    switch(ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
            pSensorInfo->SensorPreviewResolutionX = MT9M114_IMAGE_SENSOR_FULL_WIDTH - 8; /* not use */
            pSensorInfo->SensorPreviewResolutionY = MT9M114_IMAGE_SENSOR_FULL_HEIGHT - 6; /* not use */
            pSensorInfo->SensorCameraPreviewFrameRate = 30; /* not use */

            break;

        default:
            pSensorInfo->SensorPreviewResolutionX = MT9M114_IMAGE_SENSOR_FULL_WIDTH - 8; /* not use */
            pSensorInfo->SensorPreviewResolutionY = MT9M114_IMAGE_SENSOR_FULL_HEIGHT - 6; /* not use */
            pSensorInfo->SensorCameraPreviewFrameRate = 30; /* not use */

            break;
    }

    pSensorInfo->SensorVideoFrameRate = 30; /* not use */
	pSensorInfo->SensorStillCaptureFrameRate=30; /* not use */
	pSensorInfo->SensorWebCamCaptureFrameRate=30; /* not use */

    pSensorInfo->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
    pSensorInfo->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW; /* not use */
    pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW; // inverse with datasheet
    pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_HIGH;
    pSensorInfo->SensorInterruptDelayLines = 1; /* not use */
    pSensorInfo->SensorResetActiveHigh = FALSE; /* not use */
    pSensorInfo->SensorResetDelayCount = 5; /* not use */

    pSensorInfo->SensroInterfaceType = SENSOR_INTERFACE_TYPE_PARALLEL;
    pSensorInfo->SensorOutputDataFormat = SENSOR_OUTPUT_FORMAT_YVYU;

    /* ISO BinningInfo  not use yet */
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxWidth=CAM_SIZE_VGA_WIDTH;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].MaxHeight=CAM_SIZE_VGA_HEIGHT;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].ISOSupported=TRUE;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_100_MODE].BinningEnable=FALSE;

    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxWidth=CAM_SIZE_VGA_WIDTH;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].MaxHeight=CAM_SIZE_VGA_HEIGHT;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].ISOSupported=TRUE;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_200_MODE].BinningEnable=FALSE;

    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxWidth=CAM_SIZE_VGA_WIDTH;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].MaxHeight=CAM_SIZE_VGA_HEIGHT;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].ISOSupported=TRUE;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_400_MODE].BinningEnable=FALSE;

    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].MaxWidth=CAM_SIZE_VGA_WIDTH;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].MaxHeight=CAM_SIZE_VGA_HEIGHT;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].ISOSupported=TRUE;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_800_MODE].BinningEnable=TRUE;

    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].MaxWidth=CAM_SIZE_VGA_WIDTH;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].MaxHeight=CAM_SIZE_VGA_HEIGHT;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].ISOSupported=TRUE;
    pSensorInfo->SensorISOBinningInfo.ISOBinningInfo[ISO_1600_MODE].BinningEnable=TRUE;

    pSensorInfo->CaptureDelayFrame = 2; 
    pSensorInfo->PreviewDelayFrame = 4; 
    pSensorInfo->VideoDelayFrame = 2; 

    pSensorInfo->SensorMasterClockSwitch = 0; /* not use */
    pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_8MA;
    pSensorInfo->SensorDriver3D = 0; /* not use */
    pSensorInfo->AEShutDelayFrame = 0;          /* The frame of setting shutter default 0 for TG int */
    pSensorInfo->AESensorGainDelayFrame = 1;    /* The frame of setting sensor gain */
    pSensorInfo->AEISPGainDelayFrame = 2;   


    switch (ScenarioId)
    {
	    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
	    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
	    case MSDK_SCENARIO_ID_CAMERA_ZSD:
			pSensorInfo->SensorClockFreq = 24;
			pSensorInfo->SensorClockDividCount = 3; /* not use */
			pSensorInfo->SensorClockRisingCount = 0;
			pSensorInfo->SensorClockFallingCount = 2; /* not use */
			pSensorInfo->SensorPixelClockCount = 3; /* not use */
			pSensorInfo->SensorDataLatchCount = 2; /* not use */
	        pSensorInfo->SensorGrabStartX = MT9M114_FULL_START_X; 
	        pSensorInfo->SensorGrabStartY = MT9M114_FULL_START_Y;

	        break;
	    default:
	        pSensorInfo->SensorClockFreq = 24;
			pSensorInfo->SensorClockDividCount = 3; /* not use */
			pSensorInfo->SensorClockRisingCount = 0;
			pSensorInfo->SensorClockFallingCount = 2; /* not use */
			pSensorInfo->SensorPixelClockCount = 3; /* not use */
			pSensorInfo->SensorDataLatchCount = 2; /* not use */
	        pSensorInfo->SensorGrabStartX = MT9M114_PV_START_X; 
	        pSensorInfo->SensorGrabStartY = MT9M114_PV_START_Y;

	        break;
    }

	SENSORDB("[Exit]: %s \n", __FUNCTION__);
	
	return ERROR_NONE;
}	/* MT9M114GetInfo() */


UINT32 MT9M114Control(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
					  MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
    SENSORDB("[Enter]: %s, ScenarioId = %d \n", __FUNCTION__, ScenarioId);
    
    spin_lock(&mt9m114yuv_drv_lock);
    MT9M114_CurrentScenarioId = ScenarioId;
    spin_unlock(&mt9m114yuv_drv_lock);

    switch (ScenarioId)
    {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
        case MSDK_SCENARIO_ID_VIDEO_CAPTURE_MPEG4:
            MT9M114Preview(pImageWindow, pSensorConfigData);
            break;

        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
        case MSDK_SCENARIO_ID_CAMERA_ZSD:
            MT9M114Capture(pImageWindow, pSensorConfigData);
            break;
        default:
            SENSORDB("Error ScenarioId!!\n");
            break;
    }

    SENSORDB("[Exit]: %s \n", __FUNCTION__);

    return ERROR_NONE;
}	/* MT9M114Control() */


UINT32 MT9M114FeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
							 UINT8 *pFeaturePara,UINT32 *pFeatureParaLen)
{
    UINT16 u2Temp = 0; 
	UINT16 *pFeatureReturnPara16 = (UINT16 *) pFeaturePara;
	UINT16 *pFeatureData16 = (UINT16 *) pFeaturePara;
	UINT32 *pFeatureReturnPara32 = (UINT32 *) pFeaturePara;
	UINT32 *pFeatureData32 = (UINT32 *) pFeaturePara;
	MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData = (MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
	MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData = (MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;

    SENSORDB("[Enter]: %s, FeatureId = %d \n", __FUNCTION__, FeatureId);

    switch (FeatureId)
    {
        case SENSOR_FEATURE_GET_RESOLUTION:
            *pFeatureReturnPara16++ = MT9M114_IMAGE_SENSOR_FULL_WIDTH - 8;
            *pFeatureReturnPara16 = MT9M114_IMAGE_SENSOR_FULL_HEIGHT - 6;
            *pFeatureParaLen = 4;
            break;
        case SENSOR_FEATURE_GET_PERIOD:
			switch(MT9M114_CurrentScenarioId)
			{
			    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			    case MSDK_SCENARIO_ID_CAMERA_CAPTURE_MEM:
				case MSDK_SCENARIO_ID_CAMERA_ZSD:
	            	*pFeatureReturnPara16++ = MT9M114_IMAGE_SENSOR_FULL_WIDTH - 8;
	            	*pFeatureReturnPara16 = MT9M114_IMAGE_SENSOR_FULL_HEIGHT - 6;
	           		*pFeatureParaLen = 4;
				     break;
				default:
					*pFeatureReturnPara16++ = MT9M114_IMAGE_SENSOR_PV_WIDTH - 8;
	            	*pFeatureReturnPara16 = MT9M114_IMAGE_SENSOR_PV_HEIGHT - 6;
	           		*pFeatureParaLen = 4;
				     break;
			}
			break;
        case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
            *pFeatureReturnPara32 = 96000000;
            *pFeatureParaLen = 4;
            break;
        case SENSOR_FEATURE_SET_ESHUTTER:
            // for raw sensor
            break;
        case SENSOR_FEATURE_SET_NIGHTMODE:
            MT9M114NightMode((BOOL)*pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_GAIN:
            // for raw sensor
            break;
        case SENSOR_FEATURE_SET_FLASHLIGHT:
            // not use
            break;
        case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
            // not use
            break;
        case SENSOR_FEATURE_SET_REGISTER:
            // for debug
            MT9M114_write_cmos_sensor(pSensorRegData->RegAddr, pSensorRegData->RegData);
            break;
        case SENSOR_FEATURE_GET_REGISTER:
            // for debug
            pSensorRegData->RegData = MT9M114_read_cmos_sensor(pSensorRegData->RegAddr);
            break;
        case SENSOR_FEATURE_GET_CONFIG_PARA:
            memcpy(pSensorConfigData, &MT9M114_SensorConfigData, sizeof(MSDK_SENSOR_CONFIG_STRUCT));
            *pFeatureParaLen = sizeof(MSDK_SENSOR_CONFIG_STRUCT);
            break;
        case SENSOR_FEATURE_SET_CCT_REGISTER:
        case SENSOR_FEATURE_GET_CCT_REGISTER:
        case SENSOR_FEATURE_SET_ENG_REGISTER:
        case SENSOR_FEATURE_GET_ENG_REGISTER:
        case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
        case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
        case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
        case SENSOR_FEATURE_GET_GROUP_COUNT:
        case SENSOR_FEATURE_GET_GROUP_INFO:
        case SENSOR_FEATURE_GET_ITEM_INFO:
        case SENSOR_FEATURE_SET_ITEM_INFO:
        case SENSOR_FEATURE_GET_ENG_INFO:
            // for cct use in raw sensor
            break;
        case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
            *pFeatureReturnPara32 = LENS_DRIVER_ID_DO_NOT_CARE;
            *pFeatureParaLen = 4;
            break;
        case SENSOR_FEATURE_SET_YUV_CMD:
            MT9M114YUVSensorSetting((FEATURE_ID)*pFeatureData32, *(pFeatureData32 + 1));
            break;
        case SENSOR_FEATURE_SET_VIDEO_MODE:
            MT9M114YUVSetVideoMode(*pFeatureData16);
            break;
        case SENSOR_FEATURE_SET_CALIBRATION_DATA:
        case SENSOR_FEATURE_SET_SENSOR_SYNC:  
        case SENSOR_FEATURE_INITIALIZE_AF:
        case SENSOR_FEATURE_CONSTANT_AF:
        case SENSOR_FEATURE_MOVE_FOCUS_LENS:
        case SENSOR_FEATURE_GET_AF_STATUS:
        case SENSOR_FEATURE_GET_AF_INF:
        case SENSOR_FEATURE_GET_AF_MACRO:
            // not use
            break;
        case SENSOR_FEATURE_CHECK_SENSOR_ID:
            MT9M114CheckSensorID(pFeatureReturnPara32); 
            break;
        case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
        case SENSOR_FEATURE_SET_TEST_PATTERN:
        case SENSOR_FEATURE_SET_SOFTWARE_PWDN:  
        case SENSOR_FEATURE_SINGLE_FOCUS_MODE:
        case SENSOR_FEATURE_CANCEL_AF:
        case SENSOR_FEATURE_SET_AF_WINDOW:
            // not use
            break;        
        case SENSOR_FEATURE_GET_EV_AWB_REF:
            MT9M114YUVGetEvAwbRef(*pFeatureData32);
            break;
        case SENSOR_FEATURE_GET_SHUTTER_GAIN_AWB_GAIN:
            MT9M114YUVGetCurAeAwbInfo(*pFeatureData32);	
            break;
        case SENSOR_FEATURE_GET_AF_MAX_NUM_FOCUS_AREAS:
            MT9M114GetAFMaxNumFocusAreas(pFeatureReturnPara32);                    
            *pFeatureParaLen=4;        
            break;
        case SENSOR_FEATURE_GET_AE_MAX_NUM_METERING_AREAS:
            MT9M114GetAFMaxNumMeteringAreas(pFeatureReturnPara32);                    
            *pFeatureParaLen=4;        
            break;    
        case SENSOR_FEATURE_SET_AE_WINDOW:
            // not use
            break;
        case SENSOR_FEATURE_GET_EXIF_INFO:                 
            MT9M114GetExifInfo(*pFeatureData32);        
            break; 
        default:
            break;
    }

    //SENSORDB("[Exit]: %s \n", __FUNCTION__);
    
	return ERROR_NONE;
}	/* MT9M114FeatureControl() */


SENSOR_FUNCTION_STRUCT	SensorFuncMT9M114=
{
	MT9M114Open,
	MT9M114GetInfo,
	MT9M114GetResolution,
	MT9M114FeatureControl,
	MT9M114Control,
	MT9M114Close
};

UINT32 MT9M114_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc)
{
	/* To Do : Check Sensor status here */
	if (pfFunc!=NULL)
		*pfFunc=&SensorFuncMT9M114;

	return ERROR_NONE;
}	/* SensorInit() */
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
void MT9M114_Abico_InitialSetting(void)
{
//<20130313-ClarkLin-add delay for camera sensor reset stable to avoid preview black screen
    mDELAY(50);
//>20130313-ClarkLin
    MT9M114_write_cmos_sensor(0x301A, 0x0230); 	// RESET_REGISTER

    mDELAY(50);

//*Step2-PLL_Timing_Full resolution			//PLL and Timing
//[MT9M114_1280x960_MIPI_768_fixed_30fps_EXTCLK_24]
    MT9M114_write_cmos_sensor(0x098E, 0x1000);
    MT9M114_write_cmos_sensor_8(0xC97E, 0x01);		//cam_sysctl_pll_enable = 1
    MT9M114_write_cmos_sensor(0xC980, 0x0120);		//cam_sysctl_pll_divider_m_n = 288
    MT9M114_write_cmos_sensor(0xC982, 0x0700);		//cam_sysctl_pll_divider_p = 1792
    MT9M114_write_cmos_sensor(0xC984, 0x8040);		//cam_port_output_control = 32769
    MT9M114_write_cmos_sensor(0xC988, 0x0F00);		//cam_port_mipi_timing_t_hs_zero = 3840
    MT9M114_write_cmos_sensor(0xC98A, 0x0B07);		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
    MT9M114_write_cmos_sensor(0xC98C, 0x0D01);		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
    MT9M114_write_cmos_sensor(0xC98E, 0x071D);		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
    MT9M114_write_cmos_sensor(0xC990, 0x0006);		//cam_port_mipi_timing_t_lpx = 6
    MT9M114_write_cmos_sensor(0xC992, 0x0A0C);		//cam_port_mipi_timing_init_timing = 2572
    MT9M114_write_cmos_sensor(0xC800, 0x0004);		//cam_sensor_cfg_y_addr_start = 4
    MT9M114_write_cmos_sensor(0xC802, 0x0004);		//cam_sensor_cfg_x_addr_start = 4
    MT9M114_write_cmos_sensor(0xC804, 0x03CB);		//cam_sensor_cfg_y_addr_end = 971
    MT9M114_write_cmos_sensor(0xC806, 0x050B);		//cam_sensor_cfg_x_addr_end = 1291
    MT9M114_write_cmos_sensor(0xC808, 0x02DC);		//cam_sensor_cfg_pixclk = 48000000
    MT9M114_write_cmos_sensor(0xC80A, 0x6C00);
    MT9M114_write_cmos_sensor(0xC80C, 0x0001);		//cam_sensor_cfg_row_speed = 1
    MT9M114_write_cmos_sensor(0xC80E, 0x00DB);		//cam_sensor_cfg_fine_integ_time_min = 219
    MT9M114_write_cmos_sensor(0xC810, 0x05B3);		//cam_sensor_cfg_fine_integ_time_max = 1459
    MT9M114_write_cmos_sensor(0xC812, 0x03EE);		//cam_sensor_cfg_frame_length_lines = 1006
    MT9M114_write_cmos_sensor(0xC814, 0x0636);		//cam_sensor_cfg_line_length_pck = 1590
    MT9M114_write_cmos_sensor(0xC816, 0x0060);		//cam_sensor_cfg_fine_correction = 96
    MT9M114_write_cmos_sensor(0xC818, 0x03C3);		//cam_sensor_cfg_cpipe_last_row = 963
    MT9M114_write_cmos_sensor(0xC826, 0x0020);		//cam_sensor_cfg_reg_0_data = 32
    MT9M114_write_cmos_sensor(0xC834, 0x0000);		//cam_sensor_control_read_mode = 0
    MT9M114_write_cmos_sensor(0xC854, 0x0000);		//cam_crop_window_xoffset = 0
    MT9M114_write_cmos_sensor(0xC856, 0x0000);		//cam_crop_window_yoffset = 0
    MT9M114_write_cmos_sensor(0xC858, 0x0500);		//cam_crop_window_width = 1280
    MT9M114_write_cmos_sensor(0xC85A, 0x03C0);		//cam_crop_window_height = 960
    MT9M114_write_cmos_sensor_8(0xC85C, 0x03);		//cam_crop_cropmode = 3
    MT9M114_write_cmos_sensor(0xC868, 0x0500);		//cam_output_width = 1280
    MT9M114_write_cmos_sensor(0xC86A, 0x03C0);		//cam_output_height = 960
    MT9M114_write_cmos_sensor_8(0xC878, 0x00);		//cam_aet_aemode = 0
    MT9M114_write_cmos_sensor(0xC88C, 0x1E02);		//cam_aet_max_frame_rate = 7682
//REG = 0xC88E, 0x1E02		//cam_aet_min_frame_rate = 7682

    MT9M114_write_cmos_sensor(0xC88E, 0x0780);		//cam_aet_min_frame_rate = 7680

    MT9M114_write_cmos_sensor(0xC914, 0x0000);		//cam_stat_awb_clip_window_xstart = 0
    MT9M114_write_cmos_sensor(0xC916, 0x0000);		//cam_stat_awb_clip_window_ystart = 0
    MT9M114_write_cmos_sensor(0xC918, 0x04FF);		//cam_stat_awb_clip_window_xend = 1279
    MT9M114_write_cmos_sensor(0xC91A, 0x03BF);		//cam_stat_awb_clip_window_yend = 959
    MT9M114_write_cmos_sensor(0xC91C, 0x0000);		//cam_stat_ae_initial_window_xstart = 0
    MT9M114_write_cmos_sensor(0xC91E, 0x0000);		//cam_stat_ae_initial_window_ystart = 0
    MT9M114_write_cmos_sensor(0xC920, 0x00FF);		//cam_stat_ae_initial_window_xend = 255
    MT9M114_write_cmos_sensor(0xC922, 0x00BF);		//cam_stat_ae_initial_window_yend = 1

//*Step3-Recommended		//Patch,Errata and Sensor optimization Setting
// Sensor optimization
    MT9M114_write_cmos_sensor(0x316A, 0x8270); 	// DAC_TXLO_ROW
    MT9M114_write_cmos_sensor(0x316C, 0x8270); 	// DAC_TXLO
    MT9M114_write_cmos_sensor(0x3ED0, 0x2305); 	// DAC_LD_4_5
    MT9M114_write_cmos_sensor(0x3ED2, 0x77CF); 	// DAC_LD_6_7
    MT9M114_write_cmos_sensor(0x316E, 0x8202); 	// DAC_ECL
    MT9M114_write_cmos_sensor(0x3180, 0x87FF); 	// DELTA_DK_CONTROL
    MT9M114_write_cmos_sensor(0x30D4, 0x6080); 	// COLUMN_CORRECTION
    MT9M114_write_cmos_sensor(0x098E, 0x2802); 	// LOGICAL_ADDRESS_ACCESS [AE_TRACK_MODE]
    MT9M114_write_cmos_sensor(0xA802, 0x0008); 	// AE_TRACK_MODE

    MT9M114_write_cmos_sensor(0x31e0, 0x0001);     // color dot fix, 0x0001 for SOC mode....

 // Errata item 1
    MT9M114_write_cmos_sensor(0x3E14, 0xFF39); 	// SAMP_COL_PUP2

//Step4-F139_APGA_LSC_M_90_Abico
//Lens register settings for A-1040SOC (MT9M114) REV2
// SELECTING 1012 95

    MT9M114_write_cmos_sensor(0x3640, 0x0310); 	// P_G1_P0Q0
    MT9M114_write_cmos_sensor(0x3642, 0xA009); 	// P_G1_P0Q1
    MT9M114_write_cmos_sensor(0x3644, 0x55B0); 	// P_G1_P0Q2
    MT9M114_write_cmos_sensor(0x3646, 0x85AB); 	// P_G1_P0Q3
    MT9M114_write_cmos_sensor(0x3648, 0x0372); 	// P_G1_P0Q4
    MT9M114_write_cmos_sensor(0x364A, 0x0130); 	// P_R_P0Q0
    MT9M114_write_cmos_sensor(0x364C, 0x89CA); 	// P_R_P0Q1
    MT9M114_write_cmos_sensor(0x364E, 0x1111); 	// P_R_P0Q2
    MT9M114_write_cmos_sensor(0x3650, 0x2F67); 	// P_R_P0Q3
    MT9M114_write_cmos_sensor(0x3652, 0x1A32); 	// P_R_P0Q4
    MT9M114_write_cmos_sensor(0x3654, 0x00B0); 	// P_B_P0Q0
    MT9M114_write_cmos_sensor(0x3656, 0x4B29); 	// P_B_P0Q1
    MT9M114_write_cmos_sensor(0x3658, 0x4B10); 	// P_B_P0Q2
    MT9M114_write_cmos_sensor(0x365A, 0xC38A); 	// P_B_P0Q3
    MT9M114_write_cmos_sensor(0x365C, 0x4431); 	// P_B_P0Q4
    MT9M114_write_cmos_sensor(0x365E, 0x0110); 	// P_G2_P0Q0
    MT9M114_write_cmos_sensor(0x3660, 0xF3AA); 	// P_G2_P0Q1
    MT9M114_write_cmos_sensor(0x3662, 0x5B70); 	// P_G2_P0Q2
    MT9M114_write_cmos_sensor(0x3664, 0xBF49); 	// P_G2_P0Q3
    MT9M114_write_cmos_sensor(0x3666, 0x73B1); 	// P_G2_P0Q4
    MT9M114_write_cmos_sensor(0x3680, 0x8B89); 	// P_G1_P1Q0
    MT9M114_write_cmos_sensor(0x3682, 0x212B); 	// P_G1_P1Q1
    MT9M114_write_cmos_sensor(0x3684, 0x0B4E); 	// P_G1_P1Q2
    MT9M114_write_cmos_sensor(0x3686, 0x5A4E); 	// P_G1_P1Q3
    MT9M114_write_cmos_sensor(0x3688, 0x3F8E); 	// P_G1_P1Q4
    MT9M114_write_cmos_sensor(0x368A, 0xCB07); 	// P_R_P1Q0
    MT9M114_write_cmos_sensor(0x368C, 0x2B2C); 	// P_R_P1Q1
    MT9M114_write_cmos_sensor(0x368E, 0x1EAE); 	// P_R_P1Q2
    MT9M114_write_cmos_sensor(0x3690, 0x178E); 	// P_R_P1Q3
    MT9M114_write_cmos_sensor(0x3692, 0x966F); 	// P_R_P1Q4
    MT9M114_write_cmos_sensor(0x3694, 0x58AA); 	// P_B_P1Q0
    MT9M114_write_cmos_sensor(0x3696, 0xB70C); 	// P_B_P1Q1
    MT9M114_write_cmos_sensor(0x3698, 0x1E8D); 	// P_B_P1Q2
    MT9M114_write_cmos_sensor(0x369A, 0x428D); 	// P_B_P1Q3
    MT9M114_write_cmos_sensor(0x369C, 0x9071); 	// P_B_P1Q4
    MT9M114_write_cmos_sensor(0x369E, 0x5EEB); 	// P_G2_P1Q0
    MT9M114_write_cmos_sensor(0x36A0, 0x9E2B); 	// P_G2_P1Q1
    MT9M114_write_cmos_sensor(0x36A2, 0x9CAE); 	// P_G2_P1Q2
    MT9M114_write_cmos_sensor(0x36A4, 0x49CE); 	// P_G2_P1Q3
    MT9M114_write_cmos_sensor(0x36A6, 0xA3CF); 	// P_G2_P1Q4
    MT9M114_write_cmos_sensor(0x36C0, 0x0B51); 	// P_G1_P2Q0
    MT9M114_write_cmos_sensor(0x36C2, 0x83CB); 	// P_G1_P2Q1
    MT9M114_write_cmos_sensor(0x36C4, 0x37D3); 	// P_G1_P2Q2
    MT9M114_write_cmos_sensor(0x36C6, 0x632E); 	// P_G1_P2Q3
    MT9M114_write_cmos_sensor(0x36C8, 0xEF92); 	// P_G1_P2Q4
    MT9M114_write_cmos_sensor(0x36CA, 0x2971); 	// P_R_P2Q0
    MT9M114_write_cmos_sensor(0x36CC, 0x63AC); 	// P_R_P2Q1
    MT9M114_write_cmos_sensor(0x36CE, 0x7E93); 	// P_R_P2Q2
    MT9M114_write_cmos_sensor(0x36D0, 0x498D); 	// P_R_P2Q3
    MT9M114_write_cmos_sensor(0x36D2, 0x86F3); 	// P_R_P2Q4
    MT9M114_write_cmos_sensor(0x36D4, 0x65D0); 	// P_B_P2Q0
    MT9M114_write_cmos_sensor(0x36D6, 0x044B); 	// P_B_P2Q1
    MT9M114_write_cmos_sensor(0x36D8, 0x20F3); 	// P_B_P2Q2
    MT9M114_write_cmos_sensor(0x36DA, 0x2E6F); 	// P_B_P2Q3
    MT9M114_write_cmos_sensor(0x36DC, 0x9351); 	// P_B_P2Q4
    MT9M114_write_cmos_sensor(0x36DE, 0x0AF1); 	// P_G2_P2Q0
    MT9M114_write_cmos_sensor(0x36E0, 0x366B); 	// P_G2_P2Q1
    MT9M114_write_cmos_sensor(0x36E2, 0x3533); 	// P_G2_P2Q2
    MT9M114_write_cmos_sensor(0x36E4, 0x636D); 	// P_G2_P2Q3
    MT9M114_write_cmos_sensor(0x36E6, 0x81F3); 	// P_G2_P2Q4
    MT9M114_write_cmos_sensor(0x3700, 0xC04E); 	// P_G1_P3Q0
    MT9M114_write_cmos_sensor(0x3702, 0x512B); 	// P_G1_P3Q1
    MT9M114_write_cmos_sensor(0x3704, 0xF7B0); 	// P_G1_P3Q2
    MT9M114_write_cmos_sensor(0x3706, 0xA910); 	// P_G1_P3Q3
    MT9M114_write_cmos_sensor(0x3708, 0x4C4B); 	// P_G1_P3Q4
    MT9M114_write_cmos_sensor(0x370A, 0xEBAE); 	// P_R_P3Q0
    MT9M114_write_cmos_sensor(0x370C, 0x8F2D); 	// P_R_P3Q1
    MT9M114_write_cmos_sensor(0x370E, 0x8971); 	// P_R_P3Q2
    MT9M114_write_cmos_sensor(0x3710, 0xD9AF); 	// P_R_P3Q3
    MT9M114_write_cmos_sensor(0x3712, 0x2970); 	// P_R_P3Q4
    MT9M114_write_cmos_sensor(0x3714, 0x29ED); 	// P_B_P3Q0
    MT9M114_write_cmos_sensor(0x3716, 0x078F); 	// P_B_P3Q1
    MT9M114_write_cmos_sensor(0x3718, 0xAA71); 	// P_B_P3Q2
    MT9M114_write_cmos_sensor(0x371A, 0x8530); 	// P_B_P3Q3
    MT9M114_write_cmos_sensor(0x371C, 0x4DD2); 	// P_B_P3Q4
    MT9M114_write_cmos_sensor(0x371E, 0x9B2D); 	// P_G2_P3Q0
    MT9M114_write_cmos_sensor(0x3720, 0x5C8E); 	// P_G2_P3Q1
    MT9M114_write_cmos_sensor(0x3722, 0xA372); 	// P_G2_P3Q2
    MT9M114_write_cmos_sensor(0x3724, 0xCB70); 	// P_G2_P3Q3
    MT9M114_write_cmos_sensor(0x3726, 0x4793); 	// P_G2_P3Q4
    MT9M114_write_cmos_sensor(0x3740, 0x0CB1); 	// P_G1_P4Q0
    MT9M114_write_cmos_sensor(0x3742, 0x5BCE); 	// P_G1_P4Q1
    MT9M114_write_cmos_sensor(0x3744, 0x6773); 	// P_G1_P4Q2
    MT9M114_write_cmos_sensor(0x3746, 0xAC51); 	// P_G1_P4Q3
    MT9M114_write_cmos_sensor(0x3748, 0x9457); 	// P_G1_P4Q4
    MT9M114_write_cmos_sensor(0x374A, 0x0CB2); 	// P_R_P4Q0
    MT9M114_write_cmos_sensor(0x374C, 0x4CAC); 	// P_R_P4Q1
    MT9M114_write_cmos_sensor(0x374E, 0x5614); 	// P_R_P4Q2
    MT9M114_write_cmos_sensor(0x3750, 0xB1B1); 	// P_R_P4Q3
    MT9M114_write_cmos_sensor(0x3752, 0xED57); 	// P_R_P4Q4
    MT9M114_write_cmos_sensor(0x3754, 0x5931); 	// P_B_P4Q0
    MT9M114_write_cmos_sensor(0x3756, 0x66CD); 	// P_B_P4Q1
    MT9M114_write_cmos_sensor(0x3758, 0x7773); 	// P_B_P4Q2
    MT9M114_write_cmos_sensor(0x375A, 0xFEB1); 	// P_B_P4Q3
    MT9M114_write_cmos_sensor(0x375C, 0x9BD7); 	// P_B_P4Q4
    MT9M114_write_cmos_sensor(0x375E, 0x0E31); 	// P_G2_P4Q0
    MT9M114_write_cmos_sensor(0x3760, 0x27CB); 	// P_G2_P4Q1
    MT9M114_write_cmos_sensor(0x3762, 0x5B73); 	// P_G2_P4Q2
    MT9M114_write_cmos_sensor(0x3764, 0xF450); 	// P_G2_P4Q3
    MT9M114_write_cmos_sensor(0x3766, 0x8BD7); 	// P_G2_P4Q4
    MT9M114_write_cmos_sensor(0x3782, 0x01E0); 	// CENTER_ROW
    MT9M114_write_cmos_sensor(0x3784, 0x0280); 	// CENTER_COLUMN

    MT9M114_write_cmos_sensor(0x37C0, 0xEF67); 	//  P_GR_Q5
    MT9M114_write_cmos_sensor(0x37C2, 0xCC2A); 	//  P_RD_Q5
    MT9M114_write_cmos_sensor(0x37C4, 0xFECA); 	//  P_BL_Q5
    MT9M114_write_cmos_sensor(0x37C6, 0xDCE8); 	//  P_GB_Q5

    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	//  LOGICAL addressing
    MT9M114_write_cmos_sensor(0xC960, 0x0AF0); 	//  CAM_PGA_L_CONFIG_COLOUR_TEMP
    MT9M114_write_cmos_sensor(0xC962, 0x7963); 	//  CAM_PGA_L_CONFIG_GREEN_RED_Q14

    MT9M114_write_cmos_sensor(0xC966, 0x786D); 	//  CAM_PGA_L_CONFIG_GREEN_BLUE_Q14

    MT9M114_write_cmos_sensor(0xC964, 0x6481); 	// CAM_PGA_L_CONFIG_RED_Q14     // 1002
    MT9M114_write_cmos_sensor(0xC968, 0x8888); 	// CAM_PGA_L_CONFIG_BLUE_Q14    // 1002
                                        
    MT9M114_write_cmos_sensor(0xC96A, 0x0DAC); 	// CAM_PGA_M_CONFIG_COLOUR_TEMP  // 1002
    MT9M114_write_cmos_sensor(0xC96C, 0x86A3); 	// CAM_PGA_M_CONFIG_GREEN_RED_Q14                             

    MT9M114_write_cmos_sensor(0xC970, 0x80F2); 	// CAM_PGA_M_CONFIG_GREEN_BLUE_Q14                          

    MT9M114_write_cmos_sensor(0xC972, 0x8888); 	// CAM_PGA_M_CONFIG_BLUE_Q14
                                        
    MT9M114_write_cmos_sensor(0xC96E, 0x86A3); 	// CAM_PGA_M_CONFIG_RED_Q14                

    MT9M114_write_cmos_sensor(0xC974, 0x1964); 	//  CAM_PGA_R_CONFIG_COLOUR_TEMP
    MT9M114_write_cmos_sensor(0xC976, 0x7EA4); 	//  CAM_PGA_R_CONFIG_GREEN_RED_Q14
                                        
    MT9M114_write_cmos_sensor(0xC978, 0x7DDA); 	// CAM_PGA_R_CONFIG_RED_Q14      // 08/30
    MT9M114_write_cmos_sensor(0xC97A, 0x8021); 	//  CAM_PGA_R_CONFIG_GREEN_BLUE_Q14
    MT9M114_write_cmos_sensor(0xC97C, 0x78C8); 	//  CAM_PGA_R_CONFIG_BLUE_Q14
    MT9M114_write_cmos_sensor(0xC95E, 0x0003); 	//  CAM_PGA_PGA_CONTROL

//*Step5-Abico_CCM_AWB		//AWB & CCM
//Default AWB_CCM
    MT9M114_write_cmos_sensor(0x098E, 0x4892); 	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_CCM_L_0]
    MT9M114_write_cmos_sensor(0xC892, 0x0267); 	// CAM_AWB_CCM_L_0
    MT9M114_write_cmos_sensor(0xC894, 0xFF1A); 	// CAM_AWB_CCM_L_1
    MT9M114_write_cmos_sensor(0xC896, 0xFFB3); 	// CAM_AWB_CCM_L_2
    MT9M114_write_cmos_sensor(0xC898, 0xFF80); 	// CAM_AWB_CCM_L_3
    MT9M114_write_cmos_sensor(0xC89A, 0x0166); 	// CAM_AWB_CCM_L_4
    MT9M114_write_cmos_sensor(0xC89C, 0x0003); 	// CAM_AWB_CCM_L_5
    MT9M114_write_cmos_sensor(0xC89E, 0xFF9A); 	// CAM_AWB_CCM_L_6
    MT9M114_write_cmos_sensor(0xC8A0, 0xFEB4); 	// CAM_AWB_CCM_L_7
    MT9M114_write_cmos_sensor(0xC8A2, 0x024D); 	// CAM_AWB_CCM_L_8
    MT9M114_write_cmos_sensor(0xC8A4, 0x01BF); 	// CAM_AWB_CCM_M_0
    MT9M114_write_cmos_sensor(0xC8A6, 0xFF01); 	// CAM_AWB_CCM_M_1
    MT9M114_write_cmos_sensor(0xC8A8, 0xFFF3); 	// CAM_AWB_CCM_M_2
    MT9M114_write_cmos_sensor(0xC8AA, 0xFF75); 	// CAM_AWB_CCM_M_3
    MT9M114_write_cmos_sensor(0xC8AC, 0x0198); 	// CAM_AWB_CCM_M_4
    MT9M114_write_cmos_sensor(0xC8AE, 0xFFFD); 	// CAM_AWB_CCM_M_5
    MT9M114_write_cmos_sensor(0xC8B0, 0xFF9A); 	// CAM_AWB_CCM_M_6
    MT9M114_write_cmos_sensor(0xC8B2, 0xFEE7); 	// CAM_AWB_CCM_M_7
    MT9M114_write_cmos_sensor(0xC8B4, 0x02A8); 	// CAM_AWB_CCM_M_8
    MT9M114_write_cmos_sensor(0xC8B6, 0x01D9); 	// CAM_AWB_CCM_R_0
    MT9M114_write_cmos_sensor(0xC8B8, 0xFF26); 	// CAM_AWB_CCM_R_1
    MT9M114_write_cmos_sensor(0xC8BA, 0xFFF3); 	// CAM_AWB_CCM_R_2
    MT9M114_write_cmos_sensor(0xC8BC, 0xFFB3); 	// CAM_AWB_CCM_R_3
    MT9M114_write_cmos_sensor(0xC8BE, 0x0132); 	// CAM_AWB_CCM_R_4
    MT9M114_write_cmos_sensor(0xC8C0, 0xFFE8); 	// CAM_AWB_CCM_R_5
    MT9M114_write_cmos_sensor(0xC8C2, 0xFFDA); 	// CAM_AWB_CCM_R_6
    MT9M114_write_cmos_sensor(0xC8C4, 0xFECD); 	// CAM_AWB_CCM_R_7
    MT9M114_write_cmos_sensor(0xC8C6, 0x02C2); 	// CAM_AWB_CCM_R_8
    MT9M114_write_cmos_sensor(0xC8C8, 0x0075); 	// CAM_AWB_CCM_L_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8CA, 0x011C); 	// CAM_AWB_CCM_L_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8CC, 0x009A); 	// CAM_AWB_CCM_M_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8CE, 0x0105); 	// CAM_AWB_CCM_M_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8D0, 0x00A4); 	// CAM_AWB_CCM_R_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8D2, 0x00AC); 	// CAM_AWB_CCM_R_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8D4, 0x0A8C); 	// CAM_AWB_CCM_L_CTEMP
    MT9M114_write_cmos_sensor(0xC8D6, 0x0F0A); 	// CAM_AWB_CCM_M_CTEMP
    MT9M114_write_cmos_sensor(0xC8D8, 0x1964); 	// CAM_AWB_CCM_R_CTEMP
    MT9M114_write_cmos_sensor(0xC914, 0x0000); 	// CAM_STAT_AWB_CLIP_WINDOW_XSTART
    MT9M114_write_cmos_sensor(0xC916, 0x0000); 	// CAM_STAT_AWB_CLIP_WINDOW_YSTART
    MT9M114_write_cmos_sensor(0xC918, 0x04FF); 	// CAM_STAT_AWB_CLIP_WINDOW_XEND
    MT9M114_write_cmos_sensor(0xC91A, 0x02CF); 	// CAM_STAT_AWB_CLIP_WINDOW_YEND
    MT9M114_write_cmos_sensor(0xC904, 0x0033); 	// CAM_AWB_AWB_XSHIFT_PRE_ADJ
    MT9M114_write_cmos_sensor(0xC906, 0x0040); 	// CAM_AWB_AWB_YSHIFT_PRE_ADJ
    MT9M114_write_cmos_sensor_8(0xC8F2, 0x03); 	// CAM_AWB_AWB_XSCALE
    MT9M114_write_cmos_sensor_8(0xC8F3, 0x02); 	// CAM_AWB_AWB_YSCALE
    MT9M114_write_cmos_sensor(0xC906, 0x003C); 	// CAM_AWB_AWB_YSHIFT_PRE_ADJ
    MT9M114_write_cmos_sensor(0xC8F4, 0x0000); 	// CAM_AWB_AWB_WEIGHTS_0
    MT9M114_write_cmos_sensor(0xC8F6, 0x0000); 	// CAM_AWB_AWB_WEIGHTS_1
    MT9M114_write_cmos_sensor(0xC8F8, 0x0000); 	// CAM_AWB_AWB_WEIGHTS_2
    MT9M114_write_cmos_sensor(0xC8FA, 0xE724); 	// CAM_AWB_AWB_WEIGHTS_3
    MT9M114_write_cmos_sensor(0xC8FC, 0x1583); 	// CAM_AWB_AWB_WEIGHTS_4
    MT9M114_write_cmos_sensor(0xC8FE, 0x2045); 	// CAM_AWB_AWB_WEIGHTS_5
    MT9M114_write_cmos_sensor(0xC900, 0x03FF); 	// CAM_AWB_AWB_WEIGHTS_6
    MT9M114_write_cmos_sensor(0xC902, 0x007C); 	// CAM_AWB_AWB_WEIGHTS_7
    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
    MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
// REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L

    MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002

    MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
    MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R

//REG= 0xC90E, 0x60 	// CAM_AWB_K_B_L                     // 1002

    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012

    MT9M114_write_cmos_sensor(0xC90A, 0x1194); 	// CAM_AWB_TINTS_CTEMP_THRESHOLD   // 1002

    mDELAY(10);
//[CCM]
//[Color Correction Matrices 07/02/12 20:05:06 - A-1040SOC - REV2 ]
    MT9M114_write_cmos_sensor(0x098E, 0x0000); // LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor(0xC892, 0x01FF); // CAM_AWB_CCM_L_0
    MT9M114_write_cmos_sensor(0xC894, 0xFEC4); // CAM_AWB_CCM_L_1
    MT9M114_write_cmos_sensor(0xC896, 0x003D); // CAM_AWB_CCM_L_2
    MT9M114_write_cmos_sensor(0xC898, 0xFFB3); // CAM_AWB_CCM_L_3
    MT9M114_write_cmos_sensor(0xC89A, 0x0143); // CAM_AWB_CCM_L_4
    MT9M114_write_cmos_sensor(0xC89C, 0x000A); // CAM_AWB_CCM_L_5
    MT9M114_write_cmos_sensor(0xC89E, 0xFF7B); // CAM_AWB_CCM_L_6
    MT9M114_write_cmos_sensor(0xC8A0, 0xFEE6); // CAM_AWB_CCM_L_7
    MT9M114_write_cmos_sensor(0xC8A2, 0x029F); // CAM_AWB_CCM_L_8
    MT9M114_write_cmos_sensor(0xC8C8, 0x006D); // CAM_AWB_CCM_L_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8CA, 0x011C); // CAM_AWB_CCM_L_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8A4, 0x020A); // CAM_AWB_CCM_M_0
    MT9M114_write_cmos_sensor(0xC8A6, 0xFEDD); // CAM_AWB_CCM_M_1
    MT9M114_write_cmos_sensor(0xC8A8, 0x0019); // CAM_AWB_CCM_M_2
    MT9M114_write_cmos_sensor(0xC8AA, 0xFFB3); // CAM_AWB_CCM_M_3
    MT9M114_write_cmos_sensor(0xC8AC, 0x0177); // CAM_AWB_CCM_M_4
    MT9M114_write_cmos_sensor(0xC8AE, 0xFFD6); // CAM_AWB_CCM_M_5
    MT9M114_write_cmos_sensor(0xC8B0, 0xFFBF); // CAM_AWB_CCM_M_6
    MT9M114_write_cmos_sensor(0xC8B2, 0xFF15); // CAM_AWB_CCM_M_7
    MT9M114_write_cmos_sensor(0xC8B4, 0x022C); // CAM_AWB_CCM_M_8
    MT9M114_write_cmos_sensor(0xC8CC, 0x0094); // CAM_AWB_CCM_M_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8CE, 0x0101); // CAM_AWB_CCM_M_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8B6, 0x01DB); // CAM_AWB_CCM_R_0
    MT9M114_write_cmos_sensor(0xC8B8, 0xFF5D); // CAM_AWB_CCM_R_1
    MT9M114_write_cmos_sensor(0xC8BA, 0xFFC8); // CAM_AWB_CCM_R_2
    MT9M114_write_cmos_sensor(0xC8BC, 0xFFA6); // CAM_AWB_CCM_R_3
    MT9M114_write_cmos_sensor(0xC8BE, 0x0167); // CAM_AWB_CCM_R_4
    MT9M114_write_cmos_sensor(0xC8C0, 0xFFF3); // CAM_AWB_CCM_R_5
    MT9M114_write_cmos_sensor(0xC8C2, 0xFFEF); // CAM_AWB_CCM_R_6
    MT9M114_write_cmos_sensor(0xC8C4, 0xFF7C); // CAM_AWB_CCM_R_7
    MT9M114_write_cmos_sensor(0xC8C6, 0x0195); // CAM_AWB_CCM_R_8
    MT9M114_write_cmos_sensor(0xC8D0, 0x00AD); // CAM_AWB_CCM_R_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8D2, 0x008E); // CAM_AWB_CCM_R_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8D4, 0x09C4); // CAM_AWB_CCM_L_CTEMP
    MT9M114_write_cmos_sensor(0xC8D6, 0x0D67); // CAM_AWB_CCM_M_CTEMP
    MT9M114_write_cmos_sensor(0xC8D8, 0x1964); // CAM_AWB_CCM_R_CTEMP
    MT9M114_write_cmos_sensor(0xC8EC, 0x09C4); // CAM_AWB_COLOR_TEMPERATURE_MIN
    MT9M114_write_cmos_sensor(0xC8EE, 0x1964); // CAM_AWB_COLOR_TEMPERATURE_MAX
    MT9M114_write_cmos_sensor_8(0xC8F2, 0x03);// CAM_AWB_AWB_XSCALE
    MT9M114_write_cmos_sensor_8(0xC8F3, 0x02);// CAM_AWB_AWB_YSCALE
    MT9M114_write_cmos_sensor(0xC8F4, 0xD748); // CAM_AWB_AWB_WEIGHTS_0
    MT9M114_write_cmos_sensor(0xC8F6, 0x2E1D); // CAM_AWB_AWB_WEIGHTS_1
    MT9M114_write_cmos_sensor(0xC8F8, 0x43AA); // CAM_AWB_AWB_WEIGHTS_2
    MT9M114_write_cmos_sensor(0xC8FA, 0x6A43); // CAM_AWB_AWB_WEIGHTS_3
    MT9M114_write_cmos_sensor(0xC8FC, 0xA9F2); // CAM_AWB_AWB_WEIGHTS_4
    MT9M114_write_cmos_sensor(0xC8FE, 0xB566); // CAM_AWB_AWB_WEIGHTS_5
    MT9M114_write_cmos_sensor(0xC900, 0x9392); // CAM_AWB_AWB_WEIGHTS_6
    MT9M114_write_cmos_sensor(0xC902, 0x6A65); // CAM_AWB_AWB_WEIGHTS_7
    MT9M114_write_cmos_sensor(0xC904, 0x003E); // CAM_AWB_AWB_XSHIFT_PRE_ADJ
    MT9M114_write_cmos_sensor(0xC906, 0x0034); // CAM_AWB_AWB_YSHIFT_PRE_ADJ

//SW4-L1-HL-Camera-AEAWBFineTune-00+{_20120221
//[AE target]

    MT9M114_write_cmos_sensor(0x098E,0xC87A);  // LOGICAL_ADDRESS_ACCESS [CAM_AET_TARGET_AVERAGE_LUMA]
// REG = 0xC87A,0x3F  // CAM_AET_TARGET_AVERAGE_LUMA   // 0830 FOR DR 
//REG = 0xC87A,0x2F  // 0830 FOR DR 
    MT9M114_write_cmos_sensor_8(0xC87A,0x3F);  // 1012 FOR LV 

//skip Step6

//*Step7-CPIPE_Preference	//Color Pipe preference settings, if any
    MT9M114_write_cmos_sensor(0x098E, 0x4926); 	// LOGICAL_ADDRESS_ACCESS [CAM_LL_START_BRIGHTNESS]
    MT9M114_write_cmos_sensor(0xC926, 0x0020); 	// CAM_LL_START_BRIGHTNESS
    MT9M114_write_cmos_sensor(0xC928, 0x009A); 	// CAM_LL_STOP_BRIGHTNESS
    MT9M114_write_cmos_sensor(0xC946, 0x0070); 	// CAM_LL_START_GAIN_METRIC
    MT9M114_write_cmos_sensor(0xC948, 0x00F3); 	// CAM_LL_STOP_GAIN_METRIC
    MT9M114_write_cmos_sensor(0xC952, 0x0020); 	// CAM_LL_START_TARGET_LUMA_BM
    MT9M114_write_cmos_sensor(0xC954, 0x009A); 	// CAM_LL_STOP_TARGET_LUMA_BM
    MT9M114_write_cmos_sensor_8(0xC92A, 0xc3); 	// CAM_LL_START_SATURATION    // 1012
    MT9M114_write_cmos_sensor_8(0xC92B, 0x4B); 	// CAM_LL_END_SATURATION
    MT9M114_write_cmos_sensor_8(0xC92C, 0x00); 	// CAM_LL_START_DESATURATION
    MT9M114_write_cmos_sensor_8(0xC92D, 0xFF); 	// CAM_LL_END_DESATURATION
    MT9M114_write_cmos_sensor_8(0xC92E, 0x3C); 	// CAM_LL_START_DEMOSAIC
    MT9M114_write_cmos_sensor_8(0xC92F, 0x02); 	// CAM_LL_START_AP_GAIN
    MT9M114_write_cmos_sensor_8(0xC930, 0x06); 	// CAM_LL_START_AP_THRESH
    MT9M114_write_cmos_sensor_8(0xC931, 0x64); 	// CAM_LL_STOP_DEMOSAIC
    MT9M114_write_cmos_sensor_8(0xC932, 0x01); 	// CAM_LL_STOP_AP_GAIN
    MT9M114_write_cmos_sensor_8(0xC933, 0x0C); 	// CAM_LL_STOP_AP_THRESH
    MT9M114_write_cmos_sensor_8(0xC934, 0x3C); 	// CAM_LL_START_NR_RED
    MT9M114_write_cmos_sensor_8(0xC935, 0x3C); 	// CAM_LL_START_NR_GREEN
    MT9M114_write_cmos_sensor_8(0xC936, 0x3C); 	// CAM_LL_START_NR_BLUE
    MT9M114_write_cmos_sensor_8(0xC937, 0x0F); 	// CAM_LL_START_NR_THRESH
    MT9M114_write_cmos_sensor_8(0xC938, 0x64); 	// CAM_LL_STOP_NR_RED
    MT9M114_write_cmos_sensor_8(0xC939, 0x64); 	// CAM_LL_STOP_NR_GREEN
    MT9M114_write_cmos_sensor_8(0xC93A, 0x64); 	// CAM_LL_STOP_NR_BLUE
    MT9M114_write_cmos_sensor_8(0xC93B, 0x32); 	// CAM_LL_STOP_NR_THRESH
    MT9M114_write_cmos_sensor(0xC93C, 0x0020); 	// CAM_LL_START_CONTRAST_BM
    MT9M114_write_cmos_sensor(0xC93E, 0x009A); 	// CAM_LL_STOP_CONTRAST_BM
    MT9M114_write_cmos_sensor(0xC940, 0x00DC); 	// CAM_LL_GAMMA
    MT9M114_write_cmos_sensor_8(0xC942, 0x38); 	// CAM_LL_START_CONTRAST_GRADIENT
    MT9M114_write_cmos_sensor_8(0xC943, 0x30); 	// CAM_LL_STOP_CONTRAST_GRADIENT
    MT9M114_write_cmos_sensor_8(0xC944, 0x50); 	// CAM_LL_START_CONTRAST_LUMA_PERCENTAGE
    MT9M114_write_cmos_sensor_8(0xC945, 0x19); 	// CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE
    MT9M114_write_cmos_sensor(0xC94A, 0x0230); 	// CAM_LL_START_FADE_TO_BLACK_LUMA
    MT9M114_write_cmos_sensor(0xC94C, 0x0010); 	// CAM_LL_STOP_FADE_TO_BLACK_LUMA
    MT9M114_write_cmos_sensor(0xC94E, 0x01CD); 	// CAM_LL_CLUSTER_DC_TH_BM
    MT9M114_write_cmos_sensor_8(0xC950, 0x05); 	// CAM_LL_CLUSTER_DC_GATE_PERCENTAGE
    MT9M114_write_cmos_sensor_8(0xC951, 0x40); 	// CAM_LL_SUMMING_SENSITIVITY_FACTOR
    MT9M114_write_cmos_sensor_8(0xC87B, 0x1B); 	// CAM_AET_TARGET_AVERAGE_LUMA_DARK
    MT9M114_write_cmos_sensor_8(0xC878, 0x0E); 	// CAM_AET_AEMODE
    MT9M114_write_cmos_sensor(0xC890, 0x0080); 	// CAM_AET_TARGET_GAIN
    MT9M114_write_cmos_sensor(0xC886, 0x0100); 	// CAM_AET_AE_MAX_VIRT_AGAIN

// REG= 0xC87C, 0x005A 	// CAM_AET_BLACK_CLIPPING_TARGET    // 0830 for DR test
// REG= 0xC87C, 0x002A 	// CAM_AET_BLACK_CLIPPING_TARGET      // 0830 for DR test

    MT9M114_write_cmos_sensor(0xC87C, 0x0010); 	// CAM_AET_BLACK_CLIPPING_TARGET      // 0830 for DR test

    MT9M114_write_cmos_sensor_8(0xB42A, 0x05); 	// CCM_DELTA_GAIN
    MT9M114_write_cmos_sensor_8(0xA80A, 0x20); 	// AE_TRACK_AE_TRACKING_DAMPENING_SPEED

//*Step8-Features		//Ports, special features, etc.

    MT9M114_write_cmos_sensor(0x001E, 0x0777); 	// PAD_SLEW // ABICO
//  MT9M114_write_cmos_sensor(0x001E, 0x0577); 	// PAD_SLEW // LITEON

//Final-Change-Config
    MT9M114_write_cmos_sensor(0x098E, 0xDC00); 	// LOGICAL_ADDRESS_ACCESS [SYSMGR_NEXT_STATE]
    MT9M114_write_cmos_sensor_8(0xDC00, 0x28); 	// SYSMGR_NEXT_STATE

    MT9M114_write_cmos_sensor(0x0080, 0x8002); 	// COMMAND_REGISTER

}


void MT9M114_Abico_Preview(void)
{
return;
//[MT9M114_1280x720_720p_MIPI_768_fixed_30fps_EXTCLK_24]                               
    MT9M114_write_cmos_sensor(0x098E, 0x1000);                                                                  
    MT9M114_write_cmos_sensor_8(0xC97E, 0x01);		//cam_sysctl_pll_enable = 1                                    
    MT9M114_write_cmos_sensor(0xC980, 0x0120);		//cam_sysctl_pll_divider_m_n = 288                           
    MT9M114_write_cmos_sensor(0xC982, 0x0700);		//cam_sysctl_pll_divider_p = 1792                            
//REG = 0xC984, 0x8041		//cam_port_output_control = 32833                          
    MT9M114_write_cmos_sensor(0xC984, 0x8001);		//cam_port_output_control = 32833                            
    MT9M114_write_cmos_sensor(0xC988, 0x0F00);		//cam_port_mipi_timing_t_hs_zero = 3840                      
    MT9M114_write_cmos_sensor(0xC98A, 0x0B07);		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823             
    MT9M114_write_cmos_sensor(0xC98C, 0x0D01);		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329             
    MT9M114_write_cmos_sensor(0xC98E, 0x071D);		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821           
    MT9M114_write_cmos_sensor(0xC990, 0x0006);		//cam_port_mipi_timing_t_lpx = 6                             
    MT9M114_write_cmos_sensor(0xC992, 0x0A0C);		//cam_port_mipi_timing_init_timing = 2572                    
    MT9M114_write_cmos_sensor(0xC800, 0x007C);		//cam_sensor_cfg_y_addr_start = 124                          
    MT9M114_write_cmos_sensor(0xC802, 0x0004);		//cam_sensor_cfg_x_addr_start = 4                            
    MT9M114_write_cmos_sensor(0xC804, 0x0353);		//cam_sensor_cfg_y_addr_end = 851                            
    MT9M114_write_cmos_sensor(0xC806, 0x050B);		//cam_sensor_cfg_x_addr_end = 1291                           
    MT9M114_write_cmos_sensor(0xC808, 0x02DC);		//cam_sensor_cfg_pixclk = 48000000
    MT9M114_write_cmos_sensor(0xC80A, 0x6C00);    
    MT9M114_write_cmos_sensor(0xC80C, 0x0001);		//cam_sensor_cfg_row_speed = 1                               
    MT9M114_write_cmos_sensor(0xC80E, 0x00DB);		//cam_sensor_cfg_fine_integ_time_min = 219                   
    MT9M114_write_cmos_sensor(0xC810, 0x05BD);		//cam_sensor_cfg_fine_integ_time_max = 1469                  
    MT9M114_write_cmos_sensor(0xC812, 0x03E8);		//cam_sensor_cfg_frame_length_lines = 1000                   
    MT9M114_write_cmos_sensor(0xC814, 0x0640);		//cam_sensor_cfg_line_length_pck = 1600                      
    MT9M114_write_cmos_sensor(0xC816, 0x0060);		//cam_sensor_cfg_fine_correction = 96                        
    MT9M114_write_cmos_sensor(0xC818, 0x02D3);		//cam_sensor_cfg_cpipe_last_row = 723                        
    MT9M114_write_cmos_sensor(0xC826, 0x0020);		//cam_sensor_cfg_reg_0_data = 32                             
    MT9M114_write_cmos_sensor(0xC834, 0x0000);		//cam_sensor_control_read_mode = 0                           
    MT9M114_write_cmos_sensor(0xC854, 0x0000);		//cam_crop_window_xoffset = 0                                
    MT9M114_write_cmos_sensor(0xC856, 0x0000);		//cam_crop_window_yoffset = 0                                
    MT9M114_write_cmos_sensor(0xC858, 0x0500);		//cam_crop_window_width = 1280                               
    MT9M114_write_cmos_sensor(0xC85A, 0x02D0);		//cam_crop_window_height = 720                               
    MT9M114_write_cmos_sensor_8(0xC85C, 0x03);		//cam_crop_cropmode = 3                                        
    MT9M114_write_cmos_sensor(0xC868, 0x0500);		//cam_output_width = 1280                                    
    MT9M114_write_cmos_sensor(0xC86A, 0x02D0);		//cam_output_height = 720                                    
    MT9M114_write_cmos_sensor_8(0xC878, 0x00);		//cam_aet_aemode = 0                                           
    MT9M114_write_cmos_sensor(0xC88C, 0x1E00);		//cam_aet_max_frame_rate = 7680                              
//REG = 0xC88E, 0x1E00		//cam_aet_min_frame_rate = 7680                            
                                                                                     
    MT9M114_write_cmos_sensor(0xC88E, 0x0780);		//cam_aet_min_frame_rate = 7680   // 7.5fps min              
    MT9M114_write_cmos_sensor(0xC914, 0x0000);		//cam_stat_awb_clip_window_xstart = 0                        
    MT9M114_write_cmos_sensor(0xC916, 0x0000);		//cam_stat_awb_clip_window_ystart = 0                        
    MT9M114_write_cmos_sensor(0xC918, 0x04FF);		//cam_stat_awb_clip_window_xend = 1279                       
    MT9M114_write_cmos_sensor(0xC91A, 0x02CF);		//cam_stat_awb_clip_window_yend = 719                        
    MT9M114_write_cmos_sensor(0xC91C, 0x0000);		//cam_stat_ae_initial_window_xstart = 0                      
    MT9M114_write_cmos_sensor(0xC91E, 0x0000);		//cam_stat_ae_initial_window_ystart = 0                      
    MT9M114_write_cmos_sensor(0xC920, 0x00FF);		//cam_stat_ae_initial_window_xend = 255                      
    MT9M114_write_cmos_sensor(0xC922, 0x008F);		//cam_stat_ae_initial_window_yend = 143                                                                    
}

void MT9M114_Abico_Capture(void)
{
return;
//*Step2-PLL_Timing_Full resolution			//PLL and Timing
//[MT9M114_1280x960_MIPI_768_fixed_30fps_EXTCLK_24]
    MT9M114_write_cmos_sensor(0x98E, 0x1000);
    MT9M114_write_cmos_sensor_8(0xC97E, 0x01);		//cam_sysctl_pll_enable = 1
    MT9M114_write_cmos_sensor(0xC980, 0x0120);		//cam_sysctl_pll_divider_m_n = 288
    MT9M114_write_cmos_sensor(0xC982, 0x0700);		//cam_sysctl_pll_divider_p = 1792
    MT9M114_write_cmos_sensor(0xC984, 0x8040);		//cam_port_output_control = 32769
    MT9M114_write_cmos_sensor(0xC988, 0x0F00);		//cam_port_mipi_timing_t_hs_zero = 3840
    MT9M114_write_cmos_sensor(0xC98A, 0x0B07);		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
    MT9M114_write_cmos_sensor(0xC98C, 0x0D01);		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
    MT9M114_write_cmos_sensor(0xC98E, 0x071D);		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
    MT9M114_write_cmos_sensor(0xC990, 0x0006);		//cam_port_mipi_timing_t_lpx = 6
    MT9M114_write_cmos_sensor(0xC992, 0x0A0C);		//cam_port_mipi_timing_init_timing = 2572
    MT9M114_write_cmos_sensor(0xC800, 0x0004);		//cam_sensor_cfg_y_addr_start = 4
    MT9M114_write_cmos_sensor(0xC802, 0x0004);		//cam_sensor_cfg_x_addr_start = 4
    MT9M114_write_cmos_sensor(0xC804, 0x03CB);		//cam_sensor_cfg_y_addr_end = 971
    MT9M114_write_cmos_sensor(0xC806, 0x050B);		//cam_sensor_cfg_x_addr_end = 1291
    MT9M114_write_cmos_sensor(0xC808, 0x02DC);		//cam_sensor_cfg_pixclk = 48000000
    MT9M114_write_cmos_sensor(0xC80A, 0x6C00);    
    MT9M114_write_cmos_sensor(0xC80C, 0x0001);		//cam_sensor_cfg_row_speed = 1
    MT9M114_write_cmos_sensor(0xC80E, 0x00DB);		//cam_sensor_cfg_fine_integ_time_min = 219
    MT9M114_write_cmos_sensor(0xC810, 0x05B3);		//cam_sensor_cfg_fine_integ_time_max = 1459
    MT9M114_write_cmos_sensor(0xC812, 0x03EE);		//cam_sensor_cfg_frame_length_lines = 1006
    MT9M114_write_cmos_sensor(0xC814, 0x0636);		//cam_sensor_cfg_line_length_pck = 1590
    MT9M114_write_cmos_sensor(0xC816, 0x0060);		//cam_sensor_cfg_fine_correction = 96
    MT9M114_write_cmos_sensor(0xC818, 0x03C3);		//cam_sensor_cfg_cpipe_last_row = 963
    MT9M114_write_cmos_sensor(0xC826, 0x0020);		//cam_sensor_cfg_reg_0_data = 32
    MT9M114_write_cmos_sensor(0xC834, 0x0000);		//cam_sensor_control_read_mode = 0
    MT9M114_write_cmos_sensor(0xC854, 0x0000);		//cam_crop_window_xoffset = 0
    MT9M114_write_cmos_sensor(0xC856, 0x0000);		//cam_crop_window_yoffset = 0
    MT9M114_write_cmos_sensor(0xC858, 0x0500);		//cam_crop_window_width = 1280
    MT9M114_write_cmos_sensor(0xC85A, 0x03C0);		//cam_crop_window_height = 960
    MT9M114_write_cmos_sensor_8(0xC85C, 0x03);		//cam_crop_cropmode = 3
    MT9M114_write_cmos_sensor(0xC868, 0x0500);		//cam_output_width = 1280
    MT9M114_write_cmos_sensor(0xC86A, 0x03C0);		//cam_output_height = 960
    MT9M114_write_cmos_sensor_8(0xC878, 0x00);		//cam_aet_aemode = 0
    MT9M114_write_cmos_sensor(0xC88C, 0x1E02);		//cam_aet_max_frame_rate = 7682
//REG = 0xC88E, 0x1E02		//cam_aet_min_frame_rate = 7682

    MT9M114_write_cmos_sensor(0xC88E, 0x0780);		//cam_aet_min_frame_rate = 7680

    MT9M114_write_cmos_sensor(0xC914, 0x0000);		//cam_stat_awb_clip_window_xstart = 0
    MT9M114_write_cmos_sensor(0xC916, 0x0000);		//cam_stat_awb_clip_window_ystart = 0
    MT9M114_write_cmos_sensor(0xC918, 0x04FF);		//cam_stat_awb_clip_window_xend = 1279
    MT9M114_write_cmos_sensor(0xC91A, 0x03BF);		//cam_stat_awb_clip_window_yend = 959
    MT9M114_write_cmos_sensor(0xC91C, 0x0000);		//cam_stat_ae_initial_window_xstart = 0
    MT9M114_write_cmos_sensor(0xC91E, 0x0000);		//cam_stat_ae_initial_window_ystart = 0
    MT9M114_write_cmos_sensor(0xC920, 0x00FF);		//cam_stat_ae_initial_window_xend = 255
    MT9M114_write_cmos_sensor(0xC922, 0x00BF);		//cam_stat_ae_initial_window_yend = 1
}

void MT9M114_Abico_WB_AWB(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xC909, 0x02); 	// CAM_AWB_AWBMODE

    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
    MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
// REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L

    MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002

    MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
    MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R

    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012

    MT9M114_write_cmos_sensor(0xC90A, 0x1194); 	// CAM_AWB_TINTS_CTEMP_THRESHOLD   // 1002
}    

void MT9M114_Abico_WB_D65(void)
{
    MT9M114_write_cmos_sensor_8(0xC909, 0x00); 	// CAM_AWB_AWBMODE               
    MT9M114_write_cmos_sensor(0xC8F0, 0x1964); 	// CAM_AWB_COLOR_TEMPERATURE   
                                                
    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L                 
                                                
    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L                 
                                                
                                                
    MT9M114_write_cmos_sensor_8(0xC90F, 0x7C); 	// CAM_AWB_K_R_R                 
    MT9M114_write_cmos_sensor_8(0xC911, 0x78); 	// CAM_AWB_K_B_R                 
                                                
    MT9M114_write_cmos_sensor_8(0xC90F, 0x9C); 	// CAM_AWB_K_R_R                 
    MT9M114_write_cmos_sensor_8(0xC911, 0xB8); 	// CAM_AWB_K_B_R                 
}

void MT9M114_Abico_WB_CWF(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xC909, 0x00); 	// CAM_AWB_AWBMODE
    MT9M114_write_cmos_sensor(0xC8F0, 0x10D6); 	// CAM_AWB_COLOR_TEMPERATURE
}

void MT9M114_Abico_WB_TL84(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xC909, 0x00); 	// CAM_AWB_AWBMODE
//REG= 0xC8F0, 0x0E74 	// CAM_AWB_COLOR_TEMPERATURE

    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
    MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
// REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L

    MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002

    MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
    MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R

    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012

    MT9M114_write_cmos_sensor(0xC8F0, 0x0f18); 	// CAM_AWB_COLOR_TEMPERATURE

    MT9M114_write_cmos_sensor_8(0xC90E, 0x68); 	// CAM_AWB_K_B_L
    MT9M114_write_cmos_sensor_8(0xC90E, 0x78); 	// CAM_AWB_K_B_L
}

void MT9M114_Abico_WB_A(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xC909, 0x00); 	// CAM_AWB_AWBMODE
// REG= 0xC8F0, 0x0A8C 	// CAM_AWB_COLOR_TEMPERATURE

    MT9M114_write_cmos_sensor(0xC8F0, 0x0D50); 	// CAM_AWB_COLOR_TEMPERATURE

    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
    MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
// REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L

    MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002

    MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
    MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R

    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012

    MT9M114_write_cmos_sensor_8(0xC90C, 0x60); 	// CAM_AWB_K_R_L
    MT9M114_write_cmos_sensor_8(0xC90E, 0xBC); 	// CAM_AWB_K_B_L
}

void MT9M114_Abico_WB_D50(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xC909, 0x00); 	// CAM_AWB_AWBMODE
    MT9M114_write_cmos_sensor(0xC8F0, 0x1921); 	// CAM_AWB_COLOR_TEMPERATURE

    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
    MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
// REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L

    MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002

    MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
    MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R

    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012

    MT9M114_write_cmos_sensor_8(0xC911, 0x78); 	// CAM_AWB_K_B_R
    MT9M114_write_cmos_sensor_8(0xC90F, 0x70); 	// CAM_AWB_K_R_R

    MT9M114_write_cmos_sensor_8(0xC90C, 0x70); 	// CAM_AWB_K_R_L

    MT9M114_write_cmos_sensor_8(0xC911, 0x98); 	// CAM_AWB_K_B_R
    MT9M114_write_cmos_sensor_8(0xC90F, 0xA0); 	// CAM_AWB_K_R_R
    MT9M114_write_cmos_sensor(0xC90A, 0x1964); 	// CAM_AWB_TINTS_CTEMP_THRESHOLD

    MT9M114_write_cmos_sensor(0xC8F0, 0x1770); 	// CAM_AWB_COLOR_TEMPERATURE
}

void MT9M114_Abico_WB_H(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xC909, 0x00); 	// CAM_AWB_AWBMODE
// REG= 0xC8F0, 0x0A8C 	// CAM_AWB_COLOR_TEMPERATURE

    MT9M114_write_cmos_sensor(0xC8F0, 0x0D50); 	// CAM_AWB_COLOR_TEMPERATURE

    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
    MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
// REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L

    MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002

    MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
    MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R

    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012

    MT9M114_write_cmos_sensor_8(0xC90C, 0x60); 	// CAM_AWB_K_R_L
    MT9M114_write_cmos_sensor_8(0xC90E, 0xBC); 	// CAM_AWB_K_B_L
}

void MT9M114_Abico_Anti_Banding_50HZ(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0xCC03); 	// LOGICAL_ADDRESS_ACCESS [UVC_POWER_LINE_FREQUENCY_CONTROL]
    MT9M114_write_cmos_sensor_8(0xCC03, 0x01); 	// UVC_POWER_LINE_FREQUENCY_CONTROL
}

void MT9M114_Abico_Anti_Banding_60HZ(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0xCC03); 	// LOGICAL_ADDRESS_ACCESS [UVC_POWER_LINE_FREQUENCY_CONTROL]
    MT9M114_write_cmos_sensor_8(0xCC03, 0x02); 	// UVC_POWER_LINE_FREQUENCY_CONTROL
}
//<2012/12/3 alberthsiao-18115,modify front camera preview size for Hawk40
void MT9M114_LiteOn_InitialSetting(void)
{
//<20130313-ClarkLin-add delay for camera sensor reset stable to avoid preview black screen
    mDELAY(50);
//>20130313-ClarkLin
    MT9M114_write_cmos_sensor(0x301A, 0x0230); 	// RESET_REGISTER

    mDELAY(50);

//*Step2-PLL_Timing_Full resolution			//PLL and Timing
//[MT9M114_1280x960_MIPI_768_fixed_30fps_EXTCLK_24]
    MT9M114_write_cmos_sensor(0x098E, 0x1000);
    MT9M114_write_cmos_sensor_8(0xC97E, 0x01);		//cam_sysctl_pll_enable = 1
    MT9M114_write_cmos_sensor(0xC980, 0x0120);		//cam_sysctl_pll_divider_m_n = 288
    MT9M114_write_cmos_sensor(0xC982, 0x0700);		//cam_sysctl_pll_divider_p = 1792
    MT9M114_write_cmos_sensor(0xC984, 0x8040);		//cam_port_output_control = 32769
    MT9M114_write_cmos_sensor(0xC988, 0x0F00);		//cam_port_mipi_timing_t_hs_zero = 3840
    MT9M114_write_cmos_sensor(0xC98A, 0x0B07);		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
    MT9M114_write_cmos_sensor(0xC98C, 0x0D01);		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
    MT9M114_write_cmos_sensor(0xC98E, 0x071D);		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
    MT9M114_write_cmos_sensor(0xC990, 0x0006);		//cam_port_mipi_timing_t_lpx = 6
    MT9M114_write_cmos_sensor(0xC992, 0x0A0C);		//cam_port_mipi_timing_init_timing = 2572
    MT9M114_write_cmos_sensor(0xC800, 0x0004);		//cam_sensor_cfg_y_addr_start = 4
    MT9M114_write_cmos_sensor(0xC802, 0x0004);		//cam_sensor_cfg_x_addr_start = 4
    MT9M114_write_cmos_sensor(0xC804, 0x03CB);		//cam_sensor_cfg_y_addr_end = 971
    MT9M114_write_cmos_sensor(0xC806, 0x050B);		//cam_sensor_cfg_x_addr_end = 1291
    MT9M114_write_cmos_sensor(0xC808, 0x02DC);		//cam_sensor_cfg_pixclk = 48000000
    MT9M114_write_cmos_sensor(0xC80A, 0x6C00);
    MT9M114_write_cmos_sensor(0xC80C, 0x0001);		//cam_sensor_cfg_row_speed = 1
    MT9M114_write_cmos_sensor(0xC80E, 0x00DB);		//cam_sensor_cfg_fine_integ_time_min = 219
    MT9M114_write_cmos_sensor(0xC810, 0x05B3);		//cam_sensor_cfg_fine_integ_time_max = 1459
    MT9M114_write_cmos_sensor(0xC812, 0x03EE);		//cam_sensor_cfg_frame_length_lines = 1006
    MT9M114_write_cmos_sensor(0xC814, 0x0636);		//cam_sensor_cfg_line_length_pck = 1590
    MT9M114_write_cmos_sensor(0xC816, 0x0060);		//cam_sensor_cfg_fine_correction = 96
    MT9M114_write_cmos_sensor(0xC818, 0x03C3);		//cam_sensor_cfg_cpipe_last_row = 963
    MT9M114_write_cmos_sensor(0xC826, 0x0020);		//cam_sensor_cfg_reg_0_data = 32
    MT9M114_write_cmos_sensor(0xC834, 0x0000);		//cam_sensor_control_read_mode = 0
    MT9M114_write_cmos_sensor(0xC854, 0x0000);		//cam_crop_window_xoffset = 0
    MT9M114_write_cmos_sensor(0xC856, 0x0000);		//cam_crop_window_yoffset = 0
    MT9M114_write_cmos_sensor(0xC858, 0x0500);		//cam_crop_window_width = 1280
    MT9M114_write_cmos_sensor(0xC85A, 0x03C0);		//cam_crop_window_height = 960
   // MT9M114_write_cmos_sensor(0xC85A, 0x02D0);		//cam_crop_window_height = 720
    MT9M114_write_cmos_sensor_8(0xC85C, 0x03);		//cam_crop_cropmode = 3
    MT9M114_write_cmos_sensor(0xC868, 0x0500);		//cam_output_width = 1280
    MT9M114_write_cmos_sensor(0xC86A, 0x03C0);		//cam_output_height = 960
   // MT9M114_write_cmos_sensor(0xC86A, 0x02D0);		//cam_output_height = 720
    MT9M114_write_cmos_sensor_8(0xC878, 0x00);		//cam_aet_aemode = 0
    MT9M114_write_cmos_sensor(0xC88C, 0x1E02);		//cam_aet_max_frame_rate = 7682
//REG = 0xC88E, 0x1E02		//cam_aet_min_frame_rate = 7682

    MT9M114_write_cmos_sensor(0xC88E, 0x0780);		//cam_aet_min_frame_rate = 7680

    MT9M114_write_cmos_sensor(0xC914, 0x0000);		//cam_stat_awb_clip_window_xstart = 0
    MT9M114_write_cmos_sensor(0xC916, 0x0000);		//cam_stat_awb_clip_window_ystart = 0
    MT9M114_write_cmos_sensor(0xC918, 0x04FF);		//cam_stat_awb_clip_window_xend = 1279
    MT9M114_write_cmos_sensor(0xC91A, 0x03BF);		//cam_stat_awb_clip_window_yend = 959
    MT9M114_write_cmos_sensor(0xC91C, 0x0000);		//cam_stat_ae_initial_window_xstart = 0
    MT9M114_write_cmos_sensor(0xC91E, 0x0000);		//cam_stat_ae_initial_window_ystart = 0
    MT9M114_write_cmos_sensor(0xC920, 0x00FF);		//cam_stat_ae_initial_window_xend = 255
    MT9M114_write_cmos_sensor(0xC922, 0x00BF);		//cam_stat_ae_initial_window_yend = 1

//*Step3-Recommended		//Patch,Errata and Sensor optimization Setting
// Sensor optimization
    MT9M114_write_cmos_sensor(0x316A, 0x8270); 	// DAC_TXLO_ROW
    MT9M114_write_cmos_sensor(0x316C, 0x8270); 	// DAC_TXLO
    MT9M114_write_cmos_sensor(0x3ED0, 0x2305); 	// DAC_LD_4_5
    MT9M114_write_cmos_sensor(0x3ED2, 0x77CF); 	// DAC_LD_6_7
    MT9M114_write_cmos_sensor(0x316E, 0x8202); 	// DAC_ECL
    MT9M114_write_cmos_sensor(0x3180, 0x87FF); 	// DELTA_DK_CONTROL
    MT9M114_write_cmos_sensor(0x30D4, 0x6080); 	// COLUMN_CORRECTION
    MT9M114_write_cmos_sensor(0x098E, 0x2802); 	// LOGICAL_ADDRESS_ACCESS [AE_TRACK_MODE]
    MT9M114_write_cmos_sensor(0xA802, 0x0008); 	// AE_TRACK_MODE

    MT9M114_write_cmos_sensor(0x31e0, 0x0001);     // color dot fix, 0x0001 for SOC mode....

 // Errata item 1
    MT9M114_write_cmos_sensor(0x3E14, 0xFF39); 	// SAMP_COL_PUP2

//Step4-F139_APGA_LSC_M_90_Abico
//Lens register settings for A-1040SOC (MT9M114) REV2
// SELECTING 1023 95
#if 1
    //lens shading 85
    MT9M114_write_cmos_sensor(0x3640, 0x00D0);
    MT9M114_write_cmos_sensor(0x3642, 0x302A);
    MT9M114_write_cmos_sensor(0x3644, 0x4250);
    MT9M114_write_cmos_sensor(0x3646, 0x1C4D);
    MT9M114_write_cmos_sensor(0x3648, 0x5030);
    MT9M114_write_cmos_sensor(0x364A, 0x00F0);
    MT9M114_write_cmos_sensor(0x364C, 0x180B);
    MT9M114_write_cmos_sensor(0x364E, 0x7510);
    MT9M114_write_cmos_sensor(0x3650, 0x418D);
    MT9M114_write_cmos_sensor(0x3652, 0x6350);
    MT9M114_write_cmos_sensor(0x3654, 0x0130);
    MT9M114_write_cmos_sensor(0x3656, 0x31EB);
    MT9M114_write_cmos_sensor(0x3658, 0x17D0);
    MT9M114_write_cmos_sensor(0x365A, 0x076D);
    MT9M114_write_cmos_sensor(0x365C, 0x55B0);
    MT9M114_write_cmos_sensor(0x365E, 0x00B0);
    MT9M114_write_cmos_sensor(0x3660, 0x5449);
    MT9M114_write_cmos_sensor(0x3662, 0x4F70);
    MT9M114_write_cmos_sensor(0x3664, 0x05CD);
    MT9M114_write_cmos_sensor(0x3666, 0x3130);
    MT9M114_write_cmos_sensor(0x3680, 0x10A7);
    MT9M114_write_cmos_sensor(0x3682, 0xA6CA);
    MT9M114_write_cmos_sensor(0x3684, 0x222C);
    MT9M114_write_cmos_sensor(0x3686, 0x048C);
    MT9M114_write_cmos_sensor(0x3688, 0x1AAD);
    MT9M114_write_cmos_sensor(0x368A, 0xF5EA);
    MT9M114_write_cmos_sensor(0x368C, 0xB028);
    MT9M114_write_cmos_sensor(0x368E, 0x368A);
    MT9M114_write_cmos_sensor(0x3690, 0x368C);
    MT9M114_write_cmos_sensor(0x3692, 0x7A6D);
    MT9M114_write_cmos_sensor(0x3694, 0x5CEB);
    MT9M114_write_cmos_sensor(0x3696, 0x9FCC);
    MT9M114_write_cmos_sensor(0x3698, 0xAE4E);
    MT9M114_write_cmos_sensor(0x369A, 0x998A);
    MT9M114_write_cmos_sensor(0x369C, 0x600D);
    MT9M114_write_cmos_sensor(0x369E, 0x306B);
    MT9M114_write_cmos_sensor(0x36A0, 0x8D4A);
    MT9M114_write_cmos_sensor(0x36A2, 0xD18E);
    MT9M114_write_cmos_sensor(0x36A4, 0xDBCB);
    MT9M114_write_cmos_sensor(0x36A6, 0xAD46);
    MT9M114_write_cmos_sensor(0x36C0, 0x5370);
    MT9M114_write_cmos_sensor(0x36C2, 0x572E);
    MT9M114_write_cmos_sensor(0x36C4, 0x5ED2);
    MT9M114_write_cmos_sensor(0x36C6, 0x8B8F);
    MT9M114_write_cmos_sensor(0x36C8, 0x8913);
    MT9M114_write_cmos_sensor(0x36CA, 0x0931);
    MT9M114_write_cmos_sensor(0x36CC, 0x20EF);
    MT9M114_write_cmos_sensor(0x36CE, 0x0F93);
    MT9M114_write_cmos_sensor(0x36D0, 0xC1D0);
    MT9M114_write_cmos_sensor(0x36D2, 0xD133);
    MT9M114_write_cmos_sensor(0x36D4, 0x36D0);
    MT9M114_write_cmos_sensor(0x36D6, 0x646E);
    MT9M114_write_cmos_sensor(0x36D8, 0x5D12);
    MT9M114_write_cmos_sensor(0x36DA, 0xF44E);
    MT9M114_write_cmos_sensor(0x36DC, 0x8433);
    MT9M114_write_cmos_sensor(0x36DE, 0x5B10);
    MT9M114_write_cmos_sensor(0x36E0, 0x5BEE);
    MT9M114_write_cmos_sensor(0x36E2, 0x5672);
    MT9M114_write_cmos_sensor(0x36E4, 0x9D8F);
    MT9M114_write_cmos_sensor(0x36E6, 0x8733);
    MT9M114_write_cmos_sensor(0x3700, 0xDBEC);
    MT9M114_write_cmos_sensor(0x3702, 0x112C);
    MT9M114_write_cmos_sensor(0x3704, 0x0EAA);
    MT9M114_write_cmos_sensor(0x3706, 0x6D4B);
    MT9M114_write_cmos_sensor(0x3708, 0x10D1);
    MT9M114_write_cmos_sensor(0x370A, 0xC3CC);
    MT9M114_write_cmos_sensor(0x370C, 0xBE0B);
    MT9M114_write_cmos_sensor(0x370E, 0x2AA8);
    MT9M114_write_cmos_sensor(0x3710, 0x00AE);
    MT9M114_write_cmos_sensor(0x3712, 0x57D1);
    MT9M114_write_cmos_sensor(0x3714, 0x2A8C);
    MT9M114_write_cmos_sensor(0x3716, 0x5B8D);
    MT9M114_write_cmos_sensor(0x3718, 0xED2D);
    MT9M114_write_cmos_sensor(0x371A, 0x2DCE);
    MT9M114_write_cmos_sensor(0x371C, 0x7471);
    MT9M114_write_cmos_sensor(0x371E, 0x004B);
    MT9M114_write_cmos_sensor(0x3720, 0x224C);
    MT9M114_write_cmos_sensor(0x3722, 0xB6F0);
    MT9M114_write_cmos_sensor(0x3724, 0x35CF);
    MT9M114_write_cmos_sensor(0x3726, 0x0193);
    MT9M114_write_cmos_sensor(0x3740, 0x36B0);
    MT9M114_write_cmos_sensor(0x3742, 0x40EE);
    MT9M114_write_cmos_sensor(0x3744, 0x09D2);
    MT9M114_write_cmos_sensor(0x3746, 0xAC13);
    MT9M114_write_cmos_sensor(0x3748, 0xA196);
    MT9M114_write_cmos_sensor(0x374A, 0x5690);
    MT9M114_write_cmos_sensor(0x374C, 0x04CD);
    MT9M114_write_cmos_sensor(0x374E, 0x2631);
    MT9M114_write_cmos_sensor(0x3750, 0xAAD3);
    MT9M114_write_cmos_sensor(0x3752, 0xBA16);
    MT9M114_write_cmos_sensor(0x3754, 0x1F30);
    MT9M114_write_cmos_sensor(0x3756, 0x1EAE);
    MT9M114_write_cmos_sensor(0x3758, 0x5411);
    MT9M114_write_cmos_sensor(0x375A, 0x9AB3);
    MT9M114_write_cmos_sensor(0x375C, 0x9076);
    MT9M114_write_cmos_sensor(0x375E, 0x27D0);
    MT9M114_write_cmos_sensor(0x3760, 0x57EE);
    MT9M114_write_cmos_sensor(0x3762, 0x1732);
    MT9M114_write_cmos_sensor(0x3764, 0xA9F3);
    MT9M114_write_cmos_sensor(0x3766, 0xA076);
    MT9M114_write_cmos_sensor(0x3784, 0x02A5);
    MT9M114_write_cmos_sensor(0x3782, 0x01D1);
    MT9M114_write_cmos_sensor(0x37C0, 0x0000);
    MT9M114_write_cmos_sensor(0x37C2, 0x0000);
    MT9M114_write_cmos_sensor(0x37C4, 0x0000);
    MT9M114_write_cmos_sensor(0x37C6, 0x0000);
/*    
    //lens shading 90
    MT9M114_write_cmos_sensor(0xC95E, 0x0000); 	//  CAM_PGA_PGA_CONTROL
    MT9M114_write_cmos_sensor(0x3640, 0x00D0); 	// P_G1_P0Q0
    MT9M114_write_cmos_sensor(0x3642, 0xD06B); 	// P_G1_P0Q1
    MT9M114_write_cmos_sensor(0x3644, 0x4DF0); 	// P_G1_P0Q2
    MT9M114_write_cmos_sensor(0x3646, 0xDE8A); 	// P_G1_P0Q3
    MT9M114_write_cmos_sensor(0x3648, 0x04D1); 	// P_G1_P0Q4
    MT9M114_write_cmos_sensor(0x364A, 0x00F0); 	// P_R_P0Q0
    MT9M114_write_cmos_sensor(0x364C, 0xF88B); 	// P_R_P0Q1
    MT9M114_write_cmos_sensor(0x364E, 0x0071); 	// P_R_P0Q2
    MT9M114_write_cmos_sensor(0x3650, 0xD609); 	// P_R_P0Q3
    MT9M114_write_cmos_sensor(0x3652, 0x1091); 	// P_R_P0Q4
    MT9M114_write_cmos_sensor(0x3654, 0x0110); 	// P_B_P0Q0
    MT9M114_write_cmos_sensor(0x3656, 0xEC09); 	// P_B_P0Q1
    MT9M114_write_cmos_sensor(0x3658, 0x2510); 	// P_B_P0Q2
    MT9M114_write_cmos_sensor(0x365A, 0x8CEC); 	// P_B_P0Q3
    MT9M114_write_cmos_sensor(0x365C, 0x0371); 	// P_B_P0Q4
    MT9M114_write_cmos_sensor(0x365E, 0x00D0); 	// P_G2_P0Q0
    MT9M114_write_cmos_sensor(0x3660, 0x86AC); 	// P_G2_P0Q1
    MT9M114_write_cmos_sensor(0x3662, 0x5B70); 	// P_G2_P0Q2
    MT9M114_write_cmos_sensor(0x3664, 0xB6EA); 	// P_G2_P0Q3
    MT9M114_write_cmos_sensor(0x3666, 0x6AB0); 	// P_G2_P0Q4
    MT9M114_write_cmos_sensor(0x3680, 0xB388); 	// P_G1_P1Q0
    MT9M114_write_cmos_sensor(0x3682, 0x8E4B); 	// P_G1_P1Q1
    MT9M114_write_cmos_sensor(0x3684, 0x66EB); 	// P_G1_P1Q2
    MT9M114_write_cmos_sensor(0x3686, 0x236C); 	// P_G1_P1Q3
    MT9M114_write_cmos_sensor(0x3688, 0x6E4C); 	// P_G1_P1Q4
    MT9M114_write_cmos_sensor(0x368A, 0xAE8B); 	// P_R_P1Q0
    MT9M114_write_cmos_sensor(0x368C, 0xE249); 	// P_R_P1Q1
    MT9M114_write_cmos_sensor(0x368E, 0x6728); 	// P_R_P1Q2
    MT9M114_write_cmos_sensor(0x3690, 0x372C); 	// P_R_P1Q3
    MT9M114_write_cmos_sensor(0x3692, 0x14AD); 	// P_R_P1Q4
    MT9M114_write_cmos_sensor(0x3694, 0x4BCB); 	// P_B_P1Q0
    MT9M114_write_cmos_sensor(0x3696, 0x846C); 	// P_B_P1Q1
    MT9M114_write_cmos_sensor(0x3698, 0xB42E); 	// P_B_P1Q2
    MT9M114_write_cmos_sensor(0x369A, 0xC4C8); 	// P_B_P1Q3
    MT9M114_write_cmos_sensor(0x369C, 0x2ECC); 	// P_B_P1Q4
    MT9M114_write_cmos_sensor(0x369E, 0x0F0B); 	// P_G2_P1Q0
    MT9M114_write_cmos_sensor(0x36A0, 0x90E9); 	// P_G2_P1Q1
    MT9M114_write_cmos_sensor(0x36A2, 0xD2AE); 	// P_G2_P1Q2
    MT9M114_write_cmos_sensor(0x36A4, 0x8967); 	// P_G2_P1Q3
    MT9M114_write_cmos_sensor(0x36A6, 0xCD0C); 	// P_G2_P1Q4
    MT9M114_write_cmos_sensor(0x36C0, 0x61F0); 	// P_G1_P2Q0
    MT9M114_write_cmos_sensor(0x36C2, 0x252D); 	// P_G1_P2Q1
    MT9M114_write_cmos_sensor(0x36C4, 0x76D2); 	// P_G1_P2Q2
    MT9M114_write_cmos_sensor(0x36C6, 0xDB8D); 	// P_G1_P2Q3
    MT9M114_write_cmos_sensor(0x36C8, 0x8053); 	// P_G1_P2Q4
    MT9M114_write_cmos_sensor(0x36CA, 0x0E31); 	// P_R_P2Q0
    MT9M114_write_cmos_sensor(0x36CC, 0x136E); 	// P_R_P2Q1
    MT9M114_write_cmos_sensor(0x36CE, 0x1DB3); 	// P_R_P2Q2
    MT9M114_write_cmos_sensor(0x36D0, 0xD6AF); 	// P_R_P2Q3
    MT9M114_write_cmos_sensor(0x36D2, 0xBFD3); 	// P_R_P2Q4
    MT9M114_write_cmos_sensor(0x36D4, 0x4390); 	// P_B_P2Q0
    MT9M114_write_cmos_sensor(0x36D6, 0x330D); 	// P_B_P2Q1
    MT9M114_write_cmos_sensor(0x36D8, 0x70B2); 	// P_B_P2Q2
    MT9M114_write_cmos_sensor(0x36DA, 0x9AAD); 	// P_B_P2Q3
    MT9M114_write_cmos_sensor(0x36DC, 0xEED2); 	// P_B_P2Q4
    MT9M114_write_cmos_sensor(0x36DE, 0x6990); 	// P_G2_P2Q0
    MT9M114_write_cmos_sensor(0x36E0, 0x25CD); 	// P_G2_P2Q1
    MT9M114_write_cmos_sensor(0x36E2, 0x7072); 	// P_G2_P2Q2
    MT9M114_write_cmos_sensor(0x36E4, 0xF78D); 	// P_G2_P2Q3
    MT9M114_write_cmos_sensor(0x36E6, 0x8173); 	// P_G2_P2Q4
    MT9M114_write_cmos_sensor(0x3700, 0xC0EC); 	// P_G1_P3Q0
    MT9M114_write_cmos_sensor(0x3702, 0x432C); 	// P_G1_P3Q1
    MT9M114_write_cmos_sensor(0x3704, 0xA10E); 	// P_G1_P3Q2
    MT9M114_write_cmos_sensor(0x3706, 0x9A2E); 	// P_G1_P3Q3
    MT9M114_write_cmos_sensor(0x3708, 0x72F1); 	// P_G1_P3Q4
    MT9M114_write_cmos_sensor(0x370A, 0xA80C); 	// P_R_P3Q0
    MT9M114_write_cmos_sensor(0x370C, 0xDDEB); 	// P_R_P3Q1
    MT9M114_write_cmos_sensor(0x370E, 0x94EF); 	// P_R_P3Q2
    MT9M114_write_cmos_sensor(0x3710, 0x9FCD); 	// P_R_P3Q3
    MT9M114_write_cmos_sensor(0x3712, 0x3652); 	// P_R_P3Q4
    MT9M114_write_cmos_sensor(0x3714, 0x328C); 	// P_B_P3Q0
    MT9M114_write_cmos_sensor(0x3716, 0x676D); 	// P_B_P3Q1
    MT9M114_write_cmos_sensor(0x3718, 0xA06F); 	// P_B_P3Q2
    MT9M114_write_cmos_sensor(0x371A, 0xC3ED); 	// P_B_P3Q3
    MT9M114_write_cmos_sensor(0x371C, 0x30D2); 	// P_B_P3Q4
    MT9M114_write_cmos_sensor(0x371E, 0x052C); 	// P_G2_P3Q0
    MT9M114_write_cmos_sensor(0x3720, 0x3EAD); 	// P_G2_P3Q1
    MT9M114_write_cmos_sensor(0x3722, 0xFAB0); 	// P_G2_P3Q2
    MT9M114_write_cmos_sensor(0x3724, 0xF8AD); 	// P_G2_P3Q3
    MT9M114_write_cmos_sensor(0x3726, 0x1E93); 	// P_G2_P3Q4
    MT9M114_write_cmos_sensor(0x3740, 0x5DB0); 	// P_G1_P4Q0
    MT9M114_write_cmos_sensor(0x3742, 0xF34D); 	// P_G1_P4Q1
    MT9M114_write_cmos_sensor(0x3744, 0x5172); 	// P_G1_P4Q2
    MT9M114_write_cmos_sensor(0x3746, 0xD86E); 	// P_G1_P4Q3
    MT9M114_write_cmos_sensor(0x3748, 0xB0B6); 	// P_G1_P4Q4
    MT9M114_write_cmos_sensor(0x374A, 0x0B71); 	// P_R_P4Q0
    MT9M114_write_cmos_sensor(0x374C, 0x844F); 	// P_R_P4Q1
    MT9M114_write_cmos_sensor(0x374E, 0x35F2); 	// P_R_P4Q2
    MT9M114_write_cmos_sensor(0x3750, 0x5870); 	// P_R_P4Q3
    MT9M114_write_cmos_sensor(0x3752, 0xD216); 	// P_R_P4Q4
    MT9M114_write_cmos_sensor(0x3754, 0x5150); 	// P_B_P4Q0
    MT9M114_write_cmos_sensor(0x3756, 0x9A0E); 	// P_B_P4Q1
    MT9M114_write_cmos_sensor(0x3758, 0x2F52); 	// P_B_P4Q2
    MT9M114_write_cmos_sensor(0x375A, 0x5E2E); 	// P_B_P4Q3
    MT9M114_write_cmos_sensor(0x375C, 0xA036); 	// P_B_P4Q4
    MT9M114_write_cmos_sensor(0x375E, 0x4FF0); 	// P_G2_P4Q0
    MT9M114_write_cmos_sensor(0x3760, 0xFDCC); 	// P_G2_P4Q1
    MT9M114_write_cmos_sensor(0x3762, 0x5972); 	// P_G2_P4Q2
    MT9M114_write_cmos_sensor(0x3764, 0x934F); 	// P_G2_P4Q3
    MT9M114_write_cmos_sensor(0x3766, 0xAE16); 	// P_G2_P4Q4
    MT9M114_write_cmos_sensor(0x3784, 0x0280); 	// CENTER_COLUMN
    MT9M114_write_cmos_sensor(0x3782, 0x01C4); 	// CENTER_ROW
    MT9M114_write_cmos_sensor(0x37C0, 0x0000); 	// P_GR_Q5
    MT9M114_write_cmos_sensor(0x37C2, 0x0000); 	// P_RD_Q5
    MT9M114_write_cmos_sensor(0x37C4, 0x0000); 	// P_BL_Q5
    MT9M114_write_cmos_sensor(0x37C6, 0x0000); 	// P_GB_Q5
*/
#else
    MT9M114_write_cmos_sensor(0x3640, 0x0310); 	// P_G1_P0Q0
    MT9M114_write_cmos_sensor(0x3642, 0x22EB); 	// P_G1_P0Q1
    MT9M114_write_cmos_sensor(0x3644, 0x42B0); 	// P_G1_P0Q2
    MT9M114_write_cmos_sensor(0x3646, 0x10CB); 	// P_G1_P0Q3
    MT9M114_write_cmos_sensor(0x3648, 0x20F1); 	// P_G1_P0Q4
    MT9M114_write_cmos_sensor(0x364A, 0x0130); 	// P_R_P0Q0
    MT9M114_write_cmos_sensor(0x364C, 0x290B); 	// P_R_P0Q1
    MT9M114_write_cmos_sensor(0x364E, 0x7A30); 	// P_R_P0Q2
    MT9M114_write_cmos_sensor(0x3650, 0x4CCA); 	// P_R_P0Q3
    MT9M114_write_cmos_sensor(0x3652, 0x5E31); 	// P_R_P0Q4
    MT9M114_write_cmos_sensor(0x3654, 0x01B0); 	// P_B_P0Q0
    MT9M114_write_cmos_sensor(0x3656, 0x100C); 	// P_B_P0Q1
    MT9M114_write_cmos_sensor(0x3658, 0x21B0); 	// P_B_P0Q2
    MT9M114_write_cmos_sensor(0x365A, 0x984D); 	// P_B_P0Q3
    MT9M114_write_cmos_sensor(0x365C, 0x1C11); 	// P_B_P0Q4
    MT9M114_write_cmos_sensor(0x365E, 0x00B0); 	// P_G2_P0Q0
    MT9M114_write_cmos_sensor(0x3660, 0x6DEA); 	// P_G2_P0Q1
    MT9M114_write_cmos_sensor(0x3662, 0x5050); 	// P_G2_P0Q2
    MT9M114_write_cmos_sensor(0x3664, 0x3C4C); 	// P_G2_P0Q3
    MT9M114_write_cmos_sensor(0x3666, 0x0B31); 	// P_G2_P0Q4
    MT9M114_write_cmos_sensor(0x3680, 0xB66A); 	// P_G1_P1Q0
    MT9M114_write_cmos_sensor(0x3682, 0xF728); 	// P_G1_P1Q1
    MT9M114_write_cmos_sensor(0x3684, 0x520F); 	// P_G1_P1Q2
    MT9M114_write_cmos_sensor(0x3686, 0x110B); 	// P_G1_P1Q3
    MT9M114_write_cmos_sensor(0x3688, 0xFB2F); 	// P_G1_P1Q4
    MT9M114_write_cmos_sensor(0x368A, 0xEB8B); 	// P_R_P1Q0
    MT9M114_write_cmos_sensor(0x368C, 0xB90A); 	// P_R_P1Q1
    MT9M114_write_cmos_sensor(0x368E, 0x35AF); 	// P_R_P1Q2
    MT9M114_write_cmos_sensor(0x3690, 0x522A); 	// P_R_P1Q3
    MT9M114_write_cmos_sensor(0x3692, 0x864E); 	// P_R_P1Q4
    MT9M114_write_cmos_sensor(0x3694, 0xEA49); 	// P_B_P1Q0
    MT9M114_write_cmos_sensor(0x3696, 0xDAEC); 	// P_B_P1Q1
    MT9M114_write_cmos_sensor(0x3698, 0x400B); 	// P_B_P1Q2
    MT9M114_write_cmos_sensor(0x369A, 0x9D4E); 	// P_B_P1Q3
    MT9M114_write_cmos_sensor(0x369C, 0xCCAE); 	// P_B_P1Q4
    MT9M114_write_cmos_sensor(0x369E, 0x5DA7); 	// P_G2_P1Q0
    MT9M114_write_cmos_sensor(0x36A0, 0xD02C); 	// P_G2_P1Q1
    MT9M114_write_cmos_sensor(0x36A2, 0x502D); 	// P_G2_P1Q2
    MT9M114_write_cmos_sensor(0x36A4, 0xC7CD); 	// P_G2_P1Q3
    MT9M114_write_cmos_sensor(0x36A6, 0xC190); 	// P_G2_P1Q4
    MT9M114_write_cmos_sensor(0x36C0, 0x63D0); 	// P_G1_P2Q0
    MT9M114_write_cmos_sensor(0x36C2, 0x6FED); 	// P_G1_P2Q1
    MT9M114_write_cmos_sensor(0x36C4, 0x28B3); 	// P_G1_P2Q2
    MT9M114_write_cmos_sensor(0x36C6, 0xC2CF); 	// P_G1_P2Q3
    MT9M114_write_cmos_sensor(0x36C8, 0xBB13); 	// P_G1_P2Q4
    MT9M114_write_cmos_sensor(0x36CA, 0x0E51); 	// P_R_P2Q0
    MT9M114_write_cmos_sensor(0x36CC, 0x058E); 	// P_R_P2Q1
    MT9M114_write_cmos_sensor(0x36CE, 0x4DB3); 	// P_R_P2Q2
    MT9M114_write_cmos_sensor(0x36D0, 0xFBCF); 	// P_R_P2Q3
    MT9M114_write_cmos_sensor(0x36D2, 0xD5B3); 	// P_R_P2Q4
    MT9M114_write_cmos_sensor(0x36D4, 0x3330); 	// P_B_P2Q0
    MT9M114_write_cmos_sensor(0x36D6, 0x484C); 	// P_B_P2Q1
    MT9M114_write_cmos_sensor(0x36D8, 0x1093); 	// P_B_P2Q2
    MT9M114_write_cmos_sensor(0x36DA, 0x162C); 	// P_B_P2Q3
    MT9M114_write_cmos_sensor(0x36DC, 0xFC32); 	// P_B_P2Q4
    MT9M114_write_cmos_sensor(0x36DE, 0x5D90); 	// P_G2_P2Q0
    MT9M114_write_cmos_sensor(0x36E0, 0x120E); 	// P_G2_P2Q1
    MT9M114_write_cmos_sensor(0x36E2, 0x2B53); 	// P_G2_P2Q2
    MT9M114_write_cmos_sensor(0x36E4, 0x850F); 	// P_G2_P2Q3
    MT9M114_write_cmos_sensor(0x36E6, 0xD6D3); 	// P_G2_P2Q4
    MT9M114_write_cmos_sensor(0x3700, 0xEDAA); 	// P_G1_P3Q0
    MT9M114_write_cmos_sensor(0x3702, 0xDFED); 	// P_G1_P3Q1
    MT9M114_write_cmos_sensor(0x3704, 0xA3D1); 	// P_G1_P3Q2
    MT9M114_write_cmos_sensor(0x3706, 0x770F); 	// P_G1_P3Q3
    MT9M114_write_cmos_sensor(0x3708, 0x6A90); 	// P_G1_P3Q4
    MT9M114_write_cmos_sensor(0x370A, 0x606D); 	// P_R_P3Q0
    MT9M114_write_cmos_sensor(0x370C, 0xF30D); 	// P_R_P3Q1
    MT9M114_write_cmos_sensor(0x370E, 0xA550); 	// P_R_P3Q2
    MT9M114_write_cmos_sensor(0x3710, 0x28B0); 	// P_R_P3Q3
    MT9M114_write_cmos_sensor(0x3712, 0xF231); 	// P_R_P3Q4
    MT9M114_write_cmos_sensor(0x3714, 0x1CCF); 	// P_B_P3Q0
    MT9M114_write_cmos_sensor(0x3716, 0x532B); 	// P_B_P3Q1
    MT9M114_write_cmos_sensor(0x3718, 0x8C31); 	// P_B_P3Q2
    MT9M114_write_cmos_sensor(0x371A, 0x1BD1); 	// P_B_P3Q3
    MT9M114_write_cmos_sensor(0x371C, 0x6B31); 	// P_B_P3Q4
    MT9M114_write_cmos_sensor(0x371E, 0x4C0D); 	// P_G2_P3Q0
    MT9M114_write_cmos_sensor(0x3720, 0xD5CC); 	// P_G2_P3Q1
    MT9M114_write_cmos_sensor(0x3722, 0x9692); 	// P_G2_P3Q2
    MT9M114_write_cmos_sensor(0x3724, 0x12F1); 	// P_G2_P3Q3
    MT9M114_write_cmos_sensor(0x3726, 0x2853); 	// P_G2_P3Q4
    MT9M114_write_cmos_sensor(0x3740, 0x1611); 	// P_G1_P4Q0
    MT9M114_write_cmos_sensor(0x3742, 0xF08E); 	// P_G1_P4Q1
    MT9M114_write_cmos_sensor(0x3744, 0x1371); 	// P_G1_P4Q2
    MT9M114_write_cmos_sensor(0x3746, 0xD2B0); 	// P_G1_P4Q3
    MT9M114_write_cmos_sensor(0x3748, 0xCD36); 	// P_G1_P4Q4
    MT9M114_write_cmos_sensor(0x374A, 0x5631); 	// P_R_P4Q0
    MT9M114_write_cmos_sensor(0x374C, 0x046E); 	// P_R_P4Q1
    MT9M114_write_cmos_sensor(0x374E, 0x6F53); 	// P_R_P4Q2
    MT9M114_write_cmos_sensor(0x3750, 0x8FD2); 	// P_R_P4Q3
    MT9M114_write_cmos_sensor(0x3752, 0xAA37); 	// P_R_P4Q4
    MT9M114_write_cmos_sensor(0x3754, 0x4151); 	// P_B_P4Q0
    MT9M114_write_cmos_sensor(0x3756, 0x6F8E); 	// P_B_P4Q1
    MT9M114_write_cmos_sensor(0x3758, 0x6172); 	// P_B_P4Q2
    MT9M114_write_cmos_sensor(0x375A, 0xA512); 	// P_B_P4Q3
    MT9M114_write_cmos_sensor(0x375C, 0xD9F6); 	// P_B_P4Q4
    MT9M114_write_cmos_sensor(0x375E, 0x1711); 	// P_G2_P4Q0
    MT9M114_write_cmos_sensor(0x3760, 0xCF6A); 	// P_G2_P4Q1
    MT9M114_write_cmos_sensor(0x3762, 0xE3CD); 	// P_G2_P4Q2
    MT9M114_write_cmos_sensor(0x3764, 0x9C92); 	// P_G2_P4Q3
    MT9M114_write_cmos_sensor(0x3766, 0xB2F6); 	// P_G2_P4Q4
    MT9M114_write_cmos_sensor(0x3782, 0x01E0); 	// CENTER_ROW
    MT9M114_write_cmos_sensor(0x3784, 0x0278); 	// CENTER_COLUMN

    MT9M114_write_cmos_sensor(0x37C0, 0xEF67); 	//  P_GR_Q5
    MT9M114_write_cmos_sensor(0x37C2, 0xCC2A); 	//  P_RD_Q5
    MT9M114_write_cmos_sensor(0x37C4, 0xFECA); 	//  P_BL_Q5
    MT9M114_write_cmos_sensor(0x37C6, 0xDCE8); 	//  P_GB_Q5
#endif
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	//  LOGICAL addressing
    MT9M114_write_cmos_sensor(0xC960, 0x0AF0); 	//  CAM_PGA_L_CONFIG_COLOUR_TEMP
    MT9M114_write_cmos_sensor(0xC962, 0x7963); 	//  CAM_PGA_L_CONFIG_GREEN_RED_Q14

    MT9M114_write_cmos_sensor(0xC966, 0x786D); 	//  CAM_PGA_L_CONFIG_GREEN_BLUE_Q14

    MT9M114_write_cmos_sensor(0xC964, 0x6481); 	// CAM_PGA_L_CONFIG_RED_Q14      // 1002
    MT9M114_write_cmos_sensor(0xC968, 0x82D8); 	// CAM_PGA_L_CONFIG_BLUE_Q14     // 1019                                        
    MT9M114_write_cmos_sensor(0xC96A, 0x0DAC); 	// CAM_PGA_M_CONFIG_COLOUR_TEMP  // 1002
    MT9M114_write_cmos_sensor(0xC96C, 0x86A3); 	// CAM_PGA_M_CONFIG_GREEN_RED_Q14                             
    MT9M114_write_cmos_sensor(0xC970, 0x80F2); 	// CAM_PGA_M_CONFIG_GREEN_BLUE_Q14                          
    MT9M114_write_cmos_sensor(0xC972, 0x8888); 	// CAM_PGA_M_CONFIG_BLUE_Q14                                                       
    MT9M114_write_cmos_sensor(0xC96E, 0x89AE); 	// CAM_PGA_M_CONFIG_RED_Q14           //1023
    MT9M114_write_cmos_sensor(0xC974, 0x1964); 	//  CAM_PGA_R_CONFIG_COLOUR_TEMP      
    MT9M114_write_cmos_sensor(0xC976, 0x8BD4); 	//  CAM_PGA_R_CONFIG_GREEN_RED_Q14    //1023
    MT9M114_write_cmos_sensor(0xC978, 0x7870); 	// CAM_PGA_R_CONFIG_RED_Q14           //1023
    MT9M114_write_cmos_sensor(0xC97A, 0x6B90); 	//  CAM_PGA_R_CONFIG_GREEN_BLUE_Q14   //1023
    MT9M114_write_cmos_sensor(0xC97C, 0x7AC8); 	// CAM_PGA_R_CONFIG_BLUE_Q14          //1019
    MT9M114_write_cmos_sensor(0xC95E, 0x0003); 	//  CAM_PGA_PGA_CONTROL

//*Step5-Abico_CCM_AWB		//AWB & CCM
//Default AWB_CCM
    MT9M114_write_cmos_sensor(0x098E, 0x4892); 	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_CCM_L_0]
    MT9M114_write_cmos_sensor(0xC892, 0x0267); 	// CAM_AWB_CCM_L_0
    MT9M114_write_cmos_sensor(0xC894, 0xFF1A); 	// CAM_AWB_CCM_L_1
    MT9M114_write_cmos_sensor(0xC896, 0xFFB3); 	// CAM_AWB_CCM_L_2
    MT9M114_write_cmos_sensor(0xC898, 0xFF80); 	// CAM_AWB_CCM_L_3
    MT9M114_write_cmos_sensor(0xC89A, 0x0166); 	// CAM_AWB_CCM_L_4
    MT9M114_write_cmos_sensor(0xC89C, 0x0003); 	// CAM_AWB_CCM_L_5
    MT9M114_write_cmos_sensor(0xC89E, 0xFF9A); 	// CAM_AWB_CCM_L_6
    MT9M114_write_cmos_sensor(0xC8A0, 0xFEB4); 	// CAM_AWB_CCM_L_7
    MT9M114_write_cmos_sensor(0xC8A2, 0x024D); 	// CAM_AWB_CCM_L_8
    MT9M114_write_cmos_sensor(0xC8A4, 0x01BF); 	// CAM_AWB_CCM_M_0
    MT9M114_write_cmos_sensor(0xC8A6, 0xFF01); 	// CAM_AWB_CCM_M_1
    MT9M114_write_cmos_sensor(0xC8A8, 0xFFF3); 	// CAM_AWB_CCM_M_2
    MT9M114_write_cmos_sensor(0xC8AA, 0xFF75); 	// CAM_AWB_CCM_M_3
    MT9M114_write_cmos_sensor(0xC8AC, 0x0198); 	// CAM_AWB_CCM_M_4
    MT9M114_write_cmos_sensor(0xC8AE, 0xFFFD); 	// CAM_AWB_CCM_M_5
    MT9M114_write_cmos_sensor(0xC8B0, 0xFF9A); 	// CAM_AWB_CCM_M_6
    MT9M114_write_cmos_sensor(0xC8B2, 0xFEE7); 	// CAM_AWB_CCM_M_7
    MT9M114_write_cmos_sensor(0xC8B4, 0x02A8); 	// CAM_AWB_CCM_M_8
    MT9M114_write_cmos_sensor(0xC8B6, 0x01D9); 	// CAM_AWB_CCM_R_0
    MT9M114_write_cmos_sensor(0xC8B8, 0xFF26); 	// CAM_AWB_CCM_R_1
    MT9M114_write_cmos_sensor(0xC8BA, 0xFFF3); 	// CAM_AWB_CCM_R_2
    MT9M114_write_cmos_sensor(0xC8BC, 0xFFB3); 	// CAM_AWB_CCM_R_3
    MT9M114_write_cmos_sensor(0xC8BE, 0x0132); 	// CAM_AWB_CCM_R_4
    MT9M114_write_cmos_sensor(0xC8C0, 0xFFE8); 	// CAM_AWB_CCM_R_5
    MT9M114_write_cmos_sensor(0xC8C2, 0xFFDA); 	// CAM_AWB_CCM_R_6
    MT9M114_write_cmos_sensor(0xC8C4, 0xFECD); 	// CAM_AWB_CCM_R_7
    MT9M114_write_cmos_sensor(0xC8C6, 0x02C2); 	// CAM_AWB_CCM_R_8
    MT9M114_write_cmos_sensor(0xC8C8, 0x0075); 	// CAM_AWB_CCM_L_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8CA, 0x011C); 	// CAM_AWB_CCM_L_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8CC, 0x009A); 	// CAM_AWB_CCM_M_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8CE, 0x0105); 	// CAM_AWB_CCM_M_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8D0, 0x00A4); 	// CAM_AWB_CCM_R_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8D2, 0x00AC); 	// CAM_AWB_CCM_R_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8D4, 0x0A8C); 	// CAM_AWB_CCM_L_CTEMP
    MT9M114_write_cmos_sensor(0xC8D6, 0x0F0A); 	// CAM_AWB_CCM_M_CTEMP
    MT9M114_write_cmos_sensor(0xC8D8, 0x1964); 	// CAM_AWB_CCM_R_CTEMP
    MT9M114_write_cmos_sensor(0xC914, 0x0000); 	// CAM_STAT_AWB_CLIP_WINDOW_XSTART
    MT9M114_write_cmos_sensor(0xC916, 0x0000); 	// CAM_STAT_AWB_CLIP_WINDOW_YSTART
    MT9M114_write_cmos_sensor(0xC918, 0x04FF); 	// CAM_STAT_AWB_CLIP_WINDOW_XEND
    MT9M114_write_cmos_sensor(0xC91A, 0x02CF); 	// CAM_STAT_AWB_CLIP_WINDOW_YEND
//0107    MT9M114_write_cmos_sensor(0xC904, 0x0033); 	// CAM_AWB_AWB_XSHIFT_PRE_ADJ
//0107    MT9M114_write_cmos_sensor(0xC906, 0x0040); 	// CAM_AWB_AWB_YSHIFT_PRE_ADJ
//0107    MT9M114_write_cmos_sensor_8(0xC8F2, 0x03); 	// CAM_AWB_AWB_XSCALE   
//0107    MT9M114_write_cmos_sensor_8(0xC8F3, 0x02); 	// CAM_AWB_AWB_YSCALE   
//0107    MT9M114_write_cmos_sensor(0xC906, 0x003C); 	// CAM_AWB_AWB_YSHIFT_PRE_ADJ
//0107    MT9M114_write_cmos_sensor(0xC8F4, 0x0000); 	// CAM_AWB_AWB_WEIGHTS_0
//0107    MT9M114_write_cmos_sensor(0xC8F6, 0x0000); 	// CAM_AWB_AWB_WEIGHTS_1
//0107    MT9M114_write_cmos_sensor(0xC8F8, 0x0000); 	// CAM_AWB_AWB_WEIGHTS_2
//0107    MT9M114_write_cmos_sensor(0xC8FA, 0xE724); 	// CAM_AWB_AWB_WEIGHTS_3
//0107    MT9M114_write_cmos_sensor(0xC8FC, 0x1583); 	// CAM_AWB_AWB_WEIGHTS_4
//0107    MT9M114_write_cmos_sensor(0xC8FE, 0x2045); 	// CAM_AWB_AWB_WEIGHTS_5
//0107    MT9M114_write_cmos_sensor(0xC900, 0x03FF); 	// CAM_AWB_AWB_WEIGHTS_6
//0107    MT9M114_write_cmos_sensor(0xC902, 0x007C); 	// CAM_AWB_AWB_WEIGHTS_7
    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
    MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012
    MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002
    MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
    MT9M114_write_cmos_sensor_8(0xC911, 0x78); 	// CAM_AWB_K_B_R      // 1019                    
    MT9M114_write_cmos_sensor(0xC90A, 0x1194); 	// CAM_AWB_TINTS_CTEMP_THRESHOLD   // 1002

    mDELAY(10);
//[CCM]
//[Color Correction Matrices 07/02/12 20:05:06 - A-1040SOC - REV2 ]
    MT9M114_write_cmos_sensor(0x098E, 0x0000); // LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor(0xC892, 0x01FF); // CAM_AWB_CCM_L_0
    MT9M114_write_cmos_sensor(0xC894, 0xFEC4); // CAM_AWB_CCM_L_1
    MT9M114_write_cmos_sensor(0xC896, 0x003D); // CAM_AWB_CCM_L_2
    MT9M114_write_cmos_sensor(0xC898, 0xFFB3); // CAM_AWB_CCM_L_3
    MT9M114_write_cmos_sensor(0xC89A, 0x0143); // CAM_AWB_CCM_L_4
    MT9M114_write_cmos_sensor(0xC89C, 0x000A); // CAM_AWB_CCM_L_5
    MT9M114_write_cmos_sensor(0xC89E, 0xFF7B); // CAM_AWB_CCM_L_6
    MT9M114_write_cmos_sensor(0xC8A0, 0xFEE6); // CAM_AWB_CCM_L_7
    MT9M114_write_cmos_sensor(0xC8A2, 0x029F); // CAM_AWB_CCM_L_8
    MT9M114_write_cmos_sensor(0xC8C8, 0x006D); // CAM_AWB_CCM_L_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8CA, 0x011C); // CAM_AWB_CCM_L_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8A4, 0x020A); // CAM_AWB_CCM_M_0
    MT9M114_write_cmos_sensor(0xC8A6, 0xFEDD); // CAM_AWB_CCM_M_1
    MT9M114_write_cmos_sensor(0xC8A8, 0x0019); // CAM_AWB_CCM_M_2
    MT9M114_write_cmos_sensor(0xC8AA, 0xFFB3); // CAM_AWB_CCM_M_3
    MT9M114_write_cmos_sensor(0xC8AC, 0x0177); // CAM_AWB_CCM_M_4
    MT9M114_write_cmos_sensor(0xC8AE, 0xFFD6); // CAM_AWB_CCM_M_5
    MT9M114_write_cmos_sensor(0xC8B0, 0xFFBF); // CAM_AWB_CCM_M_6
    MT9M114_write_cmos_sensor(0xC8B2, 0xFF15); // CAM_AWB_CCM_M_7
    MT9M114_write_cmos_sensor(0xC8B4, 0x022C); // CAM_AWB_CCM_M_8
    MT9M114_write_cmos_sensor(0xC8CC, 0x0094); // CAM_AWB_CCM_M_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8CE, 0x0101); // CAM_AWB_CCM_M_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8B6, 0x01DB); // CAM_AWB_CCM_R_0
    MT9M114_write_cmos_sensor(0xC8B8, 0xFF5D); // CAM_AWB_CCM_R_1
    MT9M114_write_cmos_sensor(0xC8BA, 0xFFC8); // CAM_AWB_CCM_R_2
    MT9M114_write_cmos_sensor(0xC8BC, 0xFFA6); // CAM_AWB_CCM_R_3
    MT9M114_write_cmos_sensor(0xC8BE, 0x0167); // CAM_AWB_CCM_R_4
    MT9M114_write_cmos_sensor(0xC8C0, 0xFFF3); // CAM_AWB_CCM_R_5
    MT9M114_write_cmos_sensor(0xC8C2, 0xFFEF); // CAM_AWB_CCM_R_6
    MT9M114_write_cmos_sensor(0xC8C4, 0xFF7C); // CAM_AWB_CCM_R_7
    MT9M114_write_cmos_sensor(0xC8C6, 0x0195); // CAM_AWB_CCM_R_8
    MT9M114_write_cmos_sensor(0xC8D0, 0x00AD); // CAM_AWB_CCM_R_RG_GAIN
    MT9M114_write_cmos_sensor(0xC8D2, 0x008E); // CAM_AWB_CCM_R_BG_GAIN
    MT9M114_write_cmos_sensor(0xC8D4, 0x09C4); // CAM_AWB_CCM_L_CTEMP
    MT9M114_write_cmos_sensor(0xC8D6, 0x0D67); // CAM_AWB_CCM_M_CTEMP
    MT9M114_write_cmos_sensor(0xC8D8, 0x1964); // CAM_AWB_CCM_R_CTEMP
    MT9M114_write_cmos_sensor(0xC8EC, 0x09C4); // CAM_AWB_COLOR_TEMPERATURE_MIN
    MT9M114_write_cmos_sensor(0xC8EE, 0x1964); // CAM_AWB_COLOR_TEMPERATURE_MAX
    MT9M114_write_cmos_sensor_8(0xC8F2, 0x03); // CAM_AWB_AWB_XSCALE
    MT9M114_write_cmos_sensor_8(0xC8F3, 0x02); // CAM_AWB_AWB_YSCALE
    
    //0107 MT9M114_write_cmos_sensor(0xC8F4, 0xD748); // CAM_AWB_AWB_WEIGHTS_0
    //0107 MT9M114_write_cmos_sensor(0xC8F6, 0x2E1D); // CAM_AWB_AWB_WEIGHTS_1
    //0107 MT9M114_write_cmos_sensor(0xC8F8, 0x43AA); // CAM_AWB_AWB_WEIGHTS_2
    //0107 MT9M114_write_cmos_sensor(0xC8FA, 0x6A43); // CAM_AWB_AWB_WEIGHTS_3
    //0107 MT9M114_write_cmos_sensor(0xC8FC, 0xA9F2); // CAM_AWB_AWB_WEIGHTS_4
    //0107 MT9M114_write_cmos_sensor(0xC8FE, 0xB566); // CAM_AWB_AWB_WEIGHTS_5
    //0107 MT9M114_write_cmos_sensor(0xC900, 0x9392); // CAM_AWB_AWB_WEIGHTS_6
    //0107 MT9M114_write_cmos_sensor(0xC902, 0x6A65); // CAM_AWB_AWB_WEIGHTS_7
    //0107 MT9M114_write_cmos_sensor(0xC904, 0x003E); // CAM_AWB_AWB_XSHIFT_PRE_ADJ
    //0107 MT9M114_write_cmos_sensor(0xC906, 0x0034); // CAM_AWB_AWB_YSHIFT_PRE_ADJ
    MT9M114_write_cmos_sensor(0xC8F4, 0x0000); // CAM_AWB_AWB_WEIGHTS_0       //0107
    MT9M114_write_cmos_sensor(0xC8F6, 0x0000); // CAM_AWB_AWB_WEIGHTS_1       //0107
    MT9M114_write_cmos_sensor(0xC8F8, 0x0000); // CAM_AWB_AWB_WEIGHTS_2       //0107
    MT9M114_write_cmos_sensor(0xC8FA, 0xE724); // CAM_AWB_AWB_WEIGHTS_3       //0107
    MT9M114_write_cmos_sensor(0xC8FC, 0x1583); // CAM_AWB_AWB_WEIGHTS_4       //0107
    MT9M114_write_cmos_sensor(0xC8FE, 0x2045); // CAM_AWB_AWB_WEIGHTS_5       //0107
    MT9M114_write_cmos_sensor(0xC900, 0x03FF); // CAM_AWB_AWB_WEIGHTS_6       //0107
    MT9M114_write_cmos_sensor(0xC902, 0x007C); // CAM_AWB_AWB_WEIGHTS_7       //0107
    MT9M114_write_cmos_sensor(0xC904, 0x004A); // CAM_AWB_AWB_XSHIFT_PRE_ADJ  //0107
    MT9M114_write_cmos_sensor(0xC906, 0x003C); // CAM_AWB_AWB_YSHIFT_PRE_ADJ  //0107

    
//SW4-L1-HL-Camera-AEAWBFineTune-00+{_20120221
//[AE target]
    
    MT9M114_write_cmos_sensor(0x098E,0xC87A);  // LOGICAL_ADDRESS_ACCESS [CAM_AET_TARGET_AVERAGE_LUMA]
    MT9M114_write_cmos_sensor_8(0xC87A, 0x34); 	// CAM_AET_TARGET_AVERAGE_LUMA         // DR TEST
    MT9M114_write_cmos_sensor_8(0xC87B, 0x34); 	// CAM_AET_TARGET_AVERAGE_LUMA_DARK    // DR TEST
// added 1030 more center-weighted AE.                                                
                                                                                              
	                         
	MT9M114_write_cmos_sensor(0xA404, 0x0001); 	//1127 AE_RULE_ALGO Adaptive weighted based on zone luma (lowlights) 
	MT9M114_write_cmos_sensor(0xCC08, 0x0004); 
	//1212 kevin modified AE weight avoid over-exporsure
	MT9M114_write_cmos_sensor_8(0xA407, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_0_0                
	MT9M114_write_cmos_sensor_8(0xA408, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_0_1                
	MT9M114_write_cmos_sensor_8(0xA409, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_0_2                
	MT9M114_write_cmos_sensor_8(0xA40A, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_0_3                
	MT9M114_write_cmos_sensor_8(0xA40B, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_0_4                
	MT9M114_write_cmos_sensor_8(0xA40C, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_1_0                
	MT9M114_write_cmos_sensor_8(0xA40D, 0x19); 	// AE_RULE_AE_WEIGHT_TABLE_1_1                
	MT9M114_write_cmos_sensor_8(0xA40E, 0x4B); 	// AE_RULE_AE_WEIGHT_TABLE_1_2                
	MT9M114_write_cmos_sensor_8(0xA40F, 0x19); 	// AE_RULE_AE_WEIGHT_TABLE_1_3                
	MT9M114_write_cmos_sensor_8(0xA410, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_1_4                
	MT9M114_write_cmos_sensor_8(0xA411, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_2_0                
	MT9M114_write_cmos_sensor_8(0xA412, 0x4B); 	// AE_RULE_AE_WEIGHT_TABLE_2_1                
	MT9M114_write_cmos_sensor_8(0xA413, 0x4B); 	// AE_RULE_AE_WEIGHT_TABLE_2_2                
	MT9M114_write_cmos_sensor_8(0xA414, 0x4B); 	// AE_RULE_AE_WEIGHT_TABLE_2_3                
	MT9M114_write_cmos_sensor_8(0xA415, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_2_4                
	MT9M114_write_cmos_sensor_8(0xA416, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_3_0                
	MT9M114_write_cmos_sensor_8(0xA417, 0x19); 	// AE_RULE_AE_WEIGHT_TABLE_3_1                
	MT9M114_write_cmos_sensor_8(0xA418, 0x4B); 	// AE_RULE_AE_WEIGHT_TABLE_3_2                
	MT9M114_write_cmos_sensor_8(0xA419, 0x19); 	// AE_RULE_AE_WEIGHT_TABLE_3_3                
	MT9M114_write_cmos_sensor_8(0xA41A, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_3_4                
	MT9M114_write_cmos_sensor_8(0xA41B, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_4_0                
	MT9M114_write_cmos_sensor_8(0xA41C, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_4_1                
	MT9M114_write_cmos_sensor_8(0xA41D, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_4_2                
	MT9M114_write_cmos_sensor_8(0xA41E, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_4_3                
	MT9M114_write_cmos_sensor_8(0xA41F, 0x00); 	// AE_RULE_AE_WEIGHT_TABLE_4_4

    MT9M114_write_cmos_sensor_8(0xC87A,0x3F);  // 1012 FOR LV 

//skip Step6

//*Step7-CPIPE_Preference	//Color Pipe preference settings, if any
    MT9M114_write_cmos_sensor(0x098E, 0x4926); 	// LOGICAL_ADDRESS_ACCESS [CAM_LL_START_BRIGHTNESS]
    MT9M114_write_cmos_sensor(0xC926, 0x0020); 	// CAM_LL_START_BRIGHTNESS
    MT9M114_write_cmos_sensor(0xC928, 0x009A); 	// CAM_LL_STOP_BRIGHTNESS
    MT9M114_write_cmos_sensor(0xC946, 0x0070); 	// CAM_LL_START_GAIN_METRIC
    MT9M114_write_cmos_sensor(0xC948, 0x00F3); 	// CAM_LL_STOP_GAIN_METRIC
    MT9M114_write_cmos_sensor(0xC952, 0x0020); 	// CAM_LL_START_TARGET_LUMA_BM
    MT9M114_write_cmos_sensor(0xC954, 0x009A); 	// CAM_LL_STOP_TARGET_LUMA_BM
//  MT9M114_write_cmos_sensor_8(0xC92A, 0xc3); 	// CAM_LL_START_SATURATION    // 1012
    MT9M114_write_cmos_sensor_8(0xC92A, 0x89); 	// CAM_LL_START_SATURATION    // 1023
    MT9M114_write_cmos_sensor_8(0xC92B, 0x4B); 	// CAM_LL_END_SATURATION
    MT9M114_write_cmos_sensor_8(0xC92C, 0x00); 	// CAM_LL_START_DESATURATION
    MT9M114_write_cmos_sensor_8(0xC92D, 0xFF); 	// CAM_LL_END_DESATURATION
    MT9M114_write_cmos_sensor_8(0xC92E, 0x3C); 	// CAM_LL_START_DEMOSAIC
    MT9M114_write_cmos_sensor_8(0xC92F, 0x04); 	// CAM_LL_START_AP_GAIN		//0103 improve sharpness
    MT9M114_write_cmos_sensor_8(0xC930, 0x02); 	// CAM_LL_START_AP_THRESH       //0103 improve sharpness
    MT9M114_write_cmos_sensor_8(0xC931, 0x64); 	// CAM_LL_STOP_DEMOSAIC
    MT9M114_write_cmos_sensor_8(0xC932, 0x01); 	// CAM_LL_STOP_AP_GAIN
    MT9M114_write_cmos_sensor_8(0xC933, 0x0C); 	// CAM_LL_STOP_AP_THRESH
    MT9M114_write_cmos_sensor_8(0xC934, 0x3C); 	// CAM_LL_START_NR_RED
    MT9M114_write_cmos_sensor_8(0xC935, 0x3C); 	// CAM_LL_START_NR_GREEN
    MT9M114_write_cmos_sensor_8(0xC936, 0x3C); 	// CAM_LL_START_NR_BLUE
    MT9M114_write_cmos_sensor_8(0xC937, 0x0F); 	// CAM_LL_START_NR_THRESH
    MT9M114_write_cmos_sensor_8(0xC938, 0x64); 	// CAM_LL_STOP_NR_RED
    MT9M114_write_cmos_sensor_8(0xC939, 0x64); 	// CAM_LL_STOP_NR_GREEN
    MT9M114_write_cmos_sensor_8(0xC93A, 0x64); 	// CAM_LL_STOP_NR_BLUE
    MT9M114_write_cmos_sensor_8(0xC93B, 0x32); 	// CAM_LL_STOP_NR_THRESH
    MT9M114_write_cmos_sensor(0xC93C, 0x0020); 	// CAM_LL_START_CONTRAST_BM
    MT9M114_write_cmos_sensor(0xC93E, 0x009A); 	// CAM_LL_STOP_CONTRAST_BM
    MT9M114_write_cmos_sensor(0xC940, 0x00DC); 	// CAM_LL_GAMMA
    MT9M114_write_cmos_sensor_8(0xC942, 0x38); 	// CAM_LL_START_CONTRAST_GRADIENT
    MT9M114_write_cmos_sensor_8(0xC943, 0x30); 	// CAM_LL_STOP_CONTRAST_GRADIENT
    MT9M114_write_cmos_sensor_8(0xC944, 0x50); 	// CAM_LL_START_CONTRAST_LUMA_PERCENTAGE
    MT9M114_write_cmos_sensor_8(0xC945, 0x19); 	// CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE
    MT9M114_write_cmos_sensor(0xC94A, 0x0230); 	// CAM_LL_START_FADE_TO_BLACK_LUMA
    MT9M114_write_cmos_sensor(0xC94C, 0x0010); 	// CAM_LL_STOP_FADE_TO_BLACK_LUMA
    MT9M114_write_cmos_sensor(0xC94E, 0x01CD); 	// CAM_LL_CLUSTER_DC_TH_BM
    MT9M114_write_cmos_sensor_8(0xC950, 0x05); 	// CAM_LL_CLUSTER_DC_GATE_PERCENTAGE
    MT9M114_write_cmos_sensor_8(0xC951, 0x40); 	// CAM_LL_SUMMING_SENSITIVITY_FACTOR
    MT9M114_write_cmos_sensor_8(0xC87B, 0x39); 	// CAM_AET_TARGET_AVERAGE_LUMA_DARK  // 1019
    MT9M114_write_cmos_sensor_8(0xC878, 0x0E); 	// CAM_AET_AEMODE
    MT9M114_write_cmos_sensor(0xC890, 0x0080); 	// CAM_AET_TARGET_GAIN
    MT9M114_write_cmos_sensor(0xC886, 0x0100); 	// CAM_AET_AE_MAX_VIRT_AGAIN

    MT9M114_write_cmos_sensor(0xC87C, 0x0010); 	// CAM_AET_BLACK_CLIPPING_TARGET      // 0830 for DR test

    MT9M114_write_cmos_sensor_8(0xB42A, 0x05); 	// CCM_DELTA_GAIN
    MT9M114_write_cmos_sensor_8(0xA80A, 0x20); 	// AE_TRACK_AE_TRACKING_DAMPENING_SPEED

//*Step8-Features		//Ports, special features, etc.
#ifdef NONE_MTK_CAMERA_SETTING
    if(MT9M114_READ_ID == MT9M114_READ_ID_1)
        MT9M114_write_cmos_sensor(0x001E, 0x0777); 	// PAD_SLEW     // ABICO
    else
        MT9M114_write_cmos_sensor(0x001E, 0x0577); 	// PAD_SLEW     // LITEO
#else
    // set Mirror & Flip
#endif  
//  
    

//Final-Change-Config
    MT9M114_write_cmos_sensor(0x098E, 0xDC00); 	// LOGICAL_ADDRESS_ACCESS [SYSMGR_NEXT_STATE]
    MT9M114_write_cmos_sensor_8(0xDC00, 0x28); 	// SYSMGR_NEXT_STATE

    MT9M114_write_cmos_sensor(0x0080, 0x8002); 	// COMMAND_REGISTER

}
//>2012/12/3 alberthsiao-18115

void MT9M114_LiteOn_Preview(void)
{
return;
//[MT9M114_1280x720_720p_MIPI_768_fixed_30fps_EXTCLK_24]                               
    MT9M114_write_cmos_sensor(0x098E, 0x1000);                                                                  
    MT9M114_write_cmos_sensor_8(0xC97E, 0x01);		//cam_sysctl_pll_enable = 1                                    
    MT9M114_write_cmos_sensor(0xC980, 0x0120);		//cam_sysctl_pll_divider_m_n = 288                           
    MT9M114_write_cmos_sensor(0xC982, 0x0700);		//cam_sysctl_pll_divider_p = 1792                            
//REG = 0xC984, 0x8041		//cam_port_output_control = 32833                          
    MT9M114_write_cmos_sensor(0xC984, 0x8001);		//cam_port_output_control = 32833                            
    MT9M114_write_cmos_sensor(0xC988, 0x0F00);		//cam_port_mipi_timing_t_hs_zero = 3840                      
    MT9M114_write_cmos_sensor(0xC98A, 0x0B07);		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823             
    MT9M114_write_cmos_sensor(0xC98C, 0x0D01);		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329             
    MT9M114_write_cmos_sensor(0xC98E, 0x071D);		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821           
    MT9M114_write_cmos_sensor(0xC990, 0x0006);		//cam_port_mipi_timing_t_lpx = 6                             
    MT9M114_write_cmos_sensor(0xC992, 0x0A0C);		//cam_port_mipi_timing_init_timing = 2572                    
    MT9M114_write_cmos_sensor(0xC800, 0x007C);		//cam_sensor_cfg_y_addr_start = 124                          
    MT9M114_write_cmos_sensor(0xC802, 0x0004);		//cam_sensor_cfg_x_addr_start = 4                            
    MT9M114_write_cmos_sensor(0xC804, 0x0353);		//cam_sensor_cfg_y_addr_end = 851                            
    MT9M114_write_cmos_sensor(0xC806, 0x050B);		//cam_sensor_cfg_x_addr_end = 1291                           
    MT9M114_write_cmos_sensor(0xC808, 0x02DC);		//cam_sensor_cfg_pixclk = 48000000
    MT9M114_write_cmos_sensor(0xC80A, 0x6C00);    
    MT9M114_write_cmos_sensor(0xC80C, 0x0001);		//cam_sensor_cfg_row_speed = 1                               
    MT9M114_write_cmos_sensor(0xC80E, 0x00DB);		//cam_sensor_cfg_fine_integ_time_min = 219                   
    MT9M114_write_cmos_sensor(0xC810, 0x05BD);		//cam_sensor_cfg_fine_integ_time_max = 1469                  
    MT9M114_write_cmos_sensor(0xC812, 0x03E8);		//cam_sensor_cfg_frame_length_lines = 1000                   
    MT9M114_write_cmos_sensor(0xC814, 0x0640);		//cam_sensor_cfg_line_length_pck = 1600                      
    MT9M114_write_cmos_sensor(0xC816, 0x0060);		//cam_sensor_cfg_fine_correction = 96                        
    MT9M114_write_cmos_sensor(0xC818, 0x02D3);		//cam_sensor_cfg_cpipe_last_row = 723                        
    MT9M114_write_cmos_sensor(0xC826, 0x0020);		//cam_sensor_cfg_reg_0_data = 32                             
    MT9M114_write_cmos_sensor(0xC834, 0x0000);		//cam_sensor_control_read_mode = 0                           
    MT9M114_write_cmos_sensor(0xC854, 0x0000);		//cam_crop_window_xoffset = 0                                
    MT9M114_write_cmos_sensor(0xC856, 0x0000);		//cam_crop_window_yoffset = 0                                
    MT9M114_write_cmos_sensor(0xC858, 0x0500);		//cam_crop_window_width = 1280                               
    MT9M114_write_cmos_sensor(0xC85A, 0x02D0);		//cam_crop_window_height = 720                               
    MT9M114_write_cmos_sensor_8(0xC85C, 0x03);		//cam_crop_cropmode = 3                                        
    MT9M114_write_cmos_sensor(0xC868, 0x0500);		//cam_output_width = 1280                                    
    MT9M114_write_cmos_sensor(0xC86A, 0x02D0);		//cam_output_height = 720                                    
    MT9M114_write_cmos_sensor_8(0xC878, 0x00);		//cam_aet_aemode = 0                                           
    MT9M114_write_cmos_sensor(0xC88C, 0x1E00);		//cam_aet_max_frame_rate = 7680                              
//REG = 0xC88E, 0x1E00		//cam_aet_min_frame_rate = 7680                            
                                                                                     
    MT9M114_write_cmos_sensor(0xC88E, 0x0780);		//cam_aet_min_frame_rate = 7680   // 7.5fps min              
    MT9M114_write_cmos_sensor(0xC914, 0x0000);		//cam_stat_awb_clip_window_xstart = 0                        
    MT9M114_write_cmos_sensor(0xC916, 0x0000);		//cam_stat_awb_clip_window_ystart = 0                        
    MT9M114_write_cmos_sensor(0xC918, 0x04FF);		//cam_stat_awb_clip_window_xend = 1279                       
    MT9M114_write_cmos_sensor(0xC91A, 0x02CF);		//cam_stat_awb_clip_window_yend = 719                        
    MT9M114_write_cmos_sensor(0xC91C, 0x0000);		//cam_stat_ae_initial_window_xstart = 0                      
    MT9M114_write_cmos_sensor(0xC91E, 0x0000);		//cam_stat_ae_initial_window_ystart = 0                      
    MT9M114_write_cmos_sensor(0xC920, 0x00FF);		//cam_stat_ae_initial_window_xend = 255                      
    MT9M114_write_cmos_sensor(0xC922, 0x008F);		//cam_stat_ae_initial_window_yend = 143       
                                                         
}

void MT9M114_LiteOn_Capture(void)
{
return;
//*Step2-PLL_Timing_Full resolution			//PLL and Timing
//[MT9M114_1280x960_MIPI_768_fixed_30fps_EXTCLK_24]
    MT9M114_write_cmos_sensor(0x98E, 0x1000);
    MT9M114_write_cmos_sensor_8(0xC97E, 0x01);		//cam_sysctl_pll_enable = 1
    MT9M114_write_cmos_sensor(0xC980, 0x0120);		//cam_sysctl_pll_divider_m_n = 288
    MT9M114_write_cmos_sensor(0xC982, 0x0700);		//cam_sysctl_pll_divider_p = 1792
    MT9M114_write_cmos_sensor(0xC984, 0x8040);		//cam_port_output_control = 32769
    MT9M114_write_cmos_sensor(0xC988, 0x0F00);		//cam_port_mipi_timing_t_hs_zero = 3840
    MT9M114_write_cmos_sensor(0xC98A, 0x0B07);		//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
    MT9M114_write_cmos_sensor(0xC98C, 0x0D01);		//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
    MT9M114_write_cmos_sensor(0xC98E, 0x071D);		//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
    MT9M114_write_cmos_sensor(0xC990, 0x0006);		//cam_port_mipi_timing_t_lpx = 6
    MT9M114_write_cmos_sensor(0xC992, 0x0A0C);		//cam_port_mipi_timing_init_timing = 2572
    MT9M114_write_cmos_sensor(0xC800, 0x0004);		//cam_sensor_cfg_y_addr_start = 4
    MT9M114_write_cmos_sensor(0xC802, 0x0004);		//cam_sensor_cfg_x_addr_start = 4
    MT9M114_write_cmos_sensor(0xC804, 0x03CB);		//cam_sensor_cfg_y_addr_end = 971
    MT9M114_write_cmos_sensor(0xC806, 0x050B);		//cam_sensor_cfg_x_addr_end = 1291
    MT9M114_write_cmos_sensor(0xC808, 0x02DC);		//cam_sensor_cfg_pixclk = 48000000
    MT9M114_write_cmos_sensor(0xC80A, 0x6C00);    
    MT9M114_write_cmos_sensor(0xC80C, 0x0001);		//cam_sensor_cfg_row_speed = 1
    MT9M114_write_cmos_sensor(0xC80E, 0x00DB);		//cam_sensor_cfg_fine_integ_time_min = 219
    MT9M114_write_cmos_sensor(0xC810, 0x05B3);		//cam_sensor_cfg_fine_integ_time_max = 1459
    MT9M114_write_cmos_sensor(0xC812, 0x03EE);		//cam_sensor_cfg_frame_length_lines = 1006
    MT9M114_write_cmos_sensor(0xC814, 0x0636);		//cam_sensor_cfg_line_length_pck = 1590
    MT9M114_write_cmos_sensor(0xC816, 0x0060);		//cam_sensor_cfg_fine_correction = 96
    MT9M114_write_cmos_sensor(0xC818, 0x03C3);		//cam_sensor_cfg_cpipe_last_row = 963
    MT9M114_write_cmos_sensor(0xC826, 0x0020);		//cam_sensor_cfg_reg_0_data = 32
    MT9M114_write_cmos_sensor(0xC834, 0x0000);		//cam_sensor_control_read_mode = 0
    MT9M114_write_cmos_sensor(0xC854, 0x0000);		//cam_crop_window_xoffset = 0
    MT9M114_write_cmos_sensor(0xC856, 0x0000);		//cam_crop_window_yoffset = 0
    MT9M114_write_cmos_sensor(0xC858, 0x0500);		//cam_crop_window_width = 1280
    MT9M114_write_cmos_sensor(0xC85A, 0x03C0);		//cam_crop_window_height = 960
    MT9M114_write_cmos_sensor_8(0xC85C, 0x03);		//cam_crop_cropmode = 3
    MT9M114_write_cmos_sensor(0xC868, 0x0500);		//cam_output_width = 1280
    MT9M114_write_cmos_sensor(0xC86A, 0x03C0);		//cam_output_height = 960
    MT9M114_write_cmos_sensor_8(0xC878, 0x00);		//cam_aet_aemode = 0
    MT9M114_write_cmos_sensor(0xC88C, 0x1E02);		//cam_aet_max_frame_rate = 7682
//REG = 0xC88E, 0x1E02		//cam_aet_min_frame_rate = 7682

    MT9M114_write_cmos_sensor(0xC88E, 0x0780);		//cam_aet_min_frame_rate = 7680

    MT9M114_write_cmos_sensor(0xC914, 0x0000);		//cam_stat_awb_clip_window_xstart = 0
    MT9M114_write_cmos_sensor(0xC916, 0x0000);		//cam_stat_awb_clip_window_ystart = 0
    MT9M114_write_cmos_sensor(0xC918, 0x04FF);		//cam_stat_awb_clip_window_xend = 1279
    MT9M114_write_cmos_sensor(0xC91A, 0x03BF);		//cam_stat_awb_clip_window_yend = 959
    MT9M114_write_cmos_sensor(0xC91C, 0x0000);		//cam_stat_ae_initial_window_xstart = 0
    MT9M114_write_cmos_sensor(0xC91E, 0x0000);		//cam_stat_ae_initial_window_ystart = 0
    MT9M114_write_cmos_sensor(0xC920, 0x00FF);		//cam_stat_ae_initial_window_xend = 255
    MT9M114_write_cmos_sensor(0xC922, 0x00BF);		//cam_stat_ae_initial_window_yend = 1

}

void MT9M114_LiteOn_WB_AWB(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xC909, 0x02); 	// CAM_AWB_AWBMODE
    MT9M114_write_cmos_sensor(0xAC04, 0x0288);  // AWB_ALGO  // enable calculate color ratio  //1031
//1031     MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
//1031     MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
//1031 // REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L
//1031 
//1031     MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002
//1031 
//1031     MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
//1031     MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R
//1031 
//1031     MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012
//1031 
//1031     MT9M114_write_cmos_sensor(0xC90A, 0x1194); 	// CAM_AWB_TINTS_CTEMP_THRESHOLD   // 1002

}    

void MT9M114_LiteOn_WB_D65(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
//  MT9M114_write_cmos_sensor_8(0xC909, 0x02); 	// CAM_AWB_AWBMODE
    MT9M114_write_cmos_sensor_8(0xC909, 0x00); 	// CAM_AWB_AWBMODE   //1031            
//1031    MT9M114_write_cmos_sensor(0xC8F0, 0x1964); 	// CAM_AWB_COLOR_TEMPERATURE   
//1031                                                
//1031    //MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L                 
//1031    //                                            
//1031    //MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L                 
//1031                                                
//1031                                                
//1031    //MT9M114_write_cmos_sensor_8(0xC90F, 0x7C); 	// CAM_AWB_K_R_R                 
//1031    //MT9M114_write_cmos_sensor_8(0xC911, 0x78); 	// CAM_AWB_K_B_R                 
//1031                                                
//1031    //MT9M114_write_cmos_sensor_8(0xC90F, 0x9C); 	// CAM_AWB_K_R_R                 
//1031    //MT9M114_write_cmos_sensor_8(0xC911, 0xB8);  // CAM_AWB_K_B_R                 
//1031    MT9M114_write_cmos_sensor_8(0xC90F, 0x85); 	// CAM_AWB_K_R_R      // 1024
//1031    MT9M114_write_cmos_sensor_8(0xC910, 0x70); 	// CAM_AWB_K_G_R
//1031    MT9M114_write_cmos_sensor_8(0xC911, 0xA0); 	// CAM_AWB_K_B_R      // 1024
//1031
//1031
//1031    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012
//1031    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
//1031    MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
    
MT9M114_write_cmos_sensor(0xAC04, 0x0208);    // AWB_ALGO    // disable calculate ratios //1031
MT9M114_write_cmos_sensor(0xC8F0, 0x1964);      // CAM_AWB_COLOR_TEMPERATURE 6500        //1031
//MT9M114_write_cmos_sensor_8(0xAC12, 0x00c4); // AWB_R_GAIN                               //1031
//MT9M114_write_cmos_sensor_8(0xAC14, 0x0090); // AWB_B_GAIN                                 //1031
MT9M114_write_cmos_sensor(0xAC12, 0x00BB); // AWB_R_GAIN                               //0110
MT9M114_write_cmos_sensor(0xAC14, 0x009F); // AWB_B_GAIN                              //0110 

}                                                                                                                                                
                                                          
void MT9M114_LiteOn_WB_CWF(void)                          
{                                                         
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xC909, 0x00); 	// CAM_AWB_AWBMODE
    MT9M114_write_cmos_sensor(0xC8F0, 0x10D6); 	// CAM_AWB_COLOR_TEMPERATURE

}   

void MT9M114_LiteOn_WB_TL84(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xC909, 0x00); 	// CAM_AWB_AWBMODE
//1031 //REG= 0xC8F0, 0x0E74 	// CAM_AWB_COLOR_TEMPERATURE
//1031     MT9M114_write_cmos_sensor_8(0xC8F0, 0x0FE8);// CAM_AWB_COLOR_TEMPERATURE 4072
//1031 //  REG= 0xC8F0, 3864	// CAM_AWB_COLOR_TEMPERATURE
//1031 
//1031 //    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
//1031 //    MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
//1031 //    REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L
//1031 
//1031 //    MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002
//1031 
//1031 //    MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
//1031 //    MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R
//1031 
//1031 //    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012
//1031 
//1031 //    MT9M114_write_cmos_sensor(0xC8F0, 0x0f18); 	// CAM_AWB_COLOR_TEMPERATURE
//1031 
//1031 //    MT9M114_write_cmos_sensor_8(0xC90E, 0x68); 	// CAM_AWB_K_B_L
//1031 //    MT9M114_write_cmos_sensor_8(0xC90E, 0x78); 	// CAM_AWB_K_B_L
//1031       MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002
//1031       MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
//1031       MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R
//1031                                               
//1031       MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
//1031       MT9M114_write_cmos_sensor_8(0xC90D, 0xA0); 	// CAM_AWB_K_G_L  // REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L
//1031       MT9M114_write_cmos_sensor_8(0xC90E, 0x70); 	// CAM_AWB_K_B_L  // 1012  
MT9M114_write_cmos_sensor(0xAC04, 0x0208);  //1031    // AWB_ALGO    // disable calculate ratios
MT9M114_write_cmos_sensor(0xC8F0, 0x0E47);  //1031      // CAM_AWB_COLOR_TEMPERATURE  3655 0e47
MT9M114_write_cmos_sensor(0xAC12, 0x0093);  //0110 // AWB_R_GAIN 90
MT9M114_write_cmos_sensor(0xAC14, 0x00F0);  //1031 // AWB_B_GAIN f0

}

void MT9M114_LiteOn_WB_A(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xC909, 0x00); 	// CAM_AWB_AWBMODE
//1031 //  REG= 0xC8F0, 0x0A8C 	// CAM_AWB_COLOR_TEMPERATURE
//1031 
//1031 //    MT9M114_write_cmos_sensor(0xC8F0, 0x0D50); 	// CAM_AWB_COLOR_TEMPERATURE
//1031     MT9M114_write_cmos_sensor(0xC8F0, 0x09C4); 	// CAM_AWB_COLOR_TEMPERATURE
//1031 //  REG= 0xC8F0, 2500 	// CAM_AWB_COLOR_TEMPERATURE
//1031 
//1031 //    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
//1031 //    MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
//1031 //// REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L
//1031 //
//1031 //    MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002
//1031 //
//1031 //    MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
//1031 //    MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R
//1031 //
//1031 //    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L   //1012
//1031 //
//1031 //    MT9M114_write_cmos_sensor_8(0xC90C, 0x60); 	// CAM_AWB_K_R_L
//1031 //    MT9M114_write_cmos_sensor_8(0xC90E, 0xBC); 	// CAM_AWB_K_B_L
//1031       MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002
//1031       MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
//1031       MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R
//1031                                               
//1031       MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
//1031       MT9M114_write_cmos_sensor_8(0xC90D, 0x70); 	// CAM_AWB_K_G_L  // REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L
//1031       MT9M114_write_cmos_sensor_8(0xC90E, 0x80); 	// CAM_AWB_K_B_L  // 1012     
MT9M114_write_cmos_sensor(0xAC04, 0x0208);  //1031    // AWB_ALGO    // disable calculate ratios  
MT9M114_write_cmos_sensor(0xC8F0, 0x0B47);  //1031      // CAM_AWB_COLOR_TEMPERATURE   2887  0b47
//MT9M114_write_cmos_sensor(0xAC12, 0x0083);  //1031 // AWB_R_GAIN
MT9M114_write_cmos_sensor(0xAC12, 0x0078);  //0110 // AWB_R_GAIN 0080
MT9M114_write_cmos_sensor(0xAC14, 0x0120);  //1010 // AWB_B_GAIN 011c

}

void MT9M114_LiteOn_WB_D50(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xC909, 0x00); 	// CAM_AWB_AWBMODE
//1031  //  MT9M114_write_cmos_sensor(0xC8F0, 0x1921); 	// CAM_AWB_COLOR_TEMPERATURE
//1031      MT9M114_write_cmos_sensor(0xC8F0, 0x14B6); 	// CAM_AWB_COLOR_TEMPERATURE
//1031  //REG= 0xC8F0, 5302 	// CAM_AWB_COLOR_TEMPERATURE
//1031  
//1031  //    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
//1031  //    MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
//1031  //// REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L
//1031  //
//1031  //    MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002
//1031  //
//1031  //    MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
//1031  //    MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R
//1031  //
//1031  //    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012
//1031  //
//1031  //    MT9M114_write_cmos_sensor_8(0xC911, 0x78); 	// CAM_AWB_K_B_R
//1031  //    MT9M114_write_cmos_sensor_8(0xC90F, 0x70); 	// CAM_AWB_K_R_R
//1031  //
//1031  //    MT9M114_write_cmos_sensor_8(0xC90C, 0x70); 	// CAM_AWB_K_R_L
//1031  //
//1031  //    MT9M114_write_cmos_sensor_8(0xC911, 0x98); 	// CAM_AWB_K_B_R
//1031  //    MT9M114_write_cmos_sensor_8(0xC90F, 0xA0); 	// CAM_AWB_K_R_R
//1031  //    MT9M114_write_cmos_sensor(0xC90A, 0x1964); 	// CAM_AWB_TINTS_CTEMP_THRESHOLD
//1031  //    MT9M114_write_cmos_sensor(0xC8F0, 0x1770); 	// CAM_AWB_COLOR_TEMPERATURE
//1031      MT9M114_write_cmos_sensor_8(0xC90F, 0x90); 	// CAM_AWB_K_R_R      // 1002
//1031      MT9M114_write_cmos_sensor_8(0xC910, 0x86); 	// CAM_AWB_K_G_R
//1031      MT9M114_write_cmos_sensor_8(0xC911, 0x70); 	// CAM_AWB_K_B_R
//1031                                              
//1031      MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
//1031      MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L  // REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L
//1031      MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012       

MT9M114_write_cmos_sensor(0xAC04, 0x0208);  //1031    // AWB_ALGO    // disable calculate ratios
MT9M114_write_cmos_sensor(0xC8F0, 0x1388);  //1031      // CAM_AWB_COLOR_TEMPERATURE 5000  1388
MT9M114_write_cmos_sensor(0xAC12, 0x00A0);  //0110 // AWB_R_GAIN  b4
MT9M114_write_cmos_sensor(0xAC14, 0x00C0);  //0110 // AWB_B_GAIN  a0

}

void MT9M114_LiteOn_WB_H(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0x0000); 	// LOGICAL_ADDRESS_ACCESS
    MT9M114_write_cmos_sensor_8(0xC909, 0x00); 	// CAM_AWB_AWBMODE
// REG= 0xC8F0, 0x0A8C 	// CAM_AWB_COLOR_TEMPERATURE

    MT9M114_write_cmos_sensor(0xC8F0, 0x0D50); 	// CAM_AWB_COLOR_TEMPERATURE

    MT9M114_write_cmos_sensor_8(0xC90C, 0x80); 	// CAM_AWB_K_R_L
    MT9M114_write_cmos_sensor_8(0xC90D, 0x80); 	// CAM_AWB_K_G_L
// REG= 0xC90E, 0x80 	// CAM_AWB_K_B_L

    MT9M114_write_cmos_sensor_8(0xC90F, 0x80); 	// CAM_AWB_K_R_R      // 1002

    MT9M114_write_cmos_sensor_8(0xC910, 0x80); 	// CAM_AWB_K_G_R
    MT9M114_write_cmos_sensor_8(0xC911, 0x80); 	// CAM_AWB_K_B_R

    MT9M114_write_cmos_sensor_8(0xC90E, 0x74); 	// CAM_AWB_K_B_L  // 1012

    MT9M114_write_cmos_sensor_8(0xC90C, 0x60); 	// CAM_AWB_K_R_L
    MT9M114_write_cmos_sensor_8(0xC90E, 0xBC); 	// CAM_AWB_K_B_L

}

void MT9M114_LiteOn_Anti_Banding_50HZ(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0xCC03); 	// LOGICAL_ADDRESS_ACCESS [UVC_POWER_LINE_FREQUENCY_CONTROL]
    MT9M114_write_cmos_sensor_8(0xCC03, 0x01); 	// UVC_POWER_LINE_FREQUENCY_CONTROL

}

void MT9M114_LiteOn_Anti_Banding_60HZ(void)
{
    MT9M114_write_cmos_sensor(0x098E, 0xCC03); 	// LOGICAL_ADDRESS_ACCESS [UVC_POWER_LINE_FREQUENCY_CONTROL]
    MT9M114_write_cmos_sensor_8(0xCC03, 0x02); 	// UVC_POWER_LINE_FREQUENCY_CONTROL

}
//>2012/11/28 alberthsiao-17860
