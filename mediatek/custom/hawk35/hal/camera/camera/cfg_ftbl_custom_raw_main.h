#ifndef _CFG_FTBL_CUSTOM_RAW_MAIN_H_
#define _CFG_FTBL_CUSTOM_RAW_MAIN_H_


//  1: Enable the custom-specific features of RAW-Main sensor.
//  0: Disable the custom-specific features of RAW-Main sensor.
#define CUSTOM_FEATURE_RAW_MAIN 1


namespace NSRAW     {
namespace NSMain    {
GETFINFO_RAW_MAIN()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //..........................................................................

#if 0
    // AF Lamp
    CONFIG_FEATURE(FID_AF_LAMP, 
        BY_DEFAULT( AF_LAMP_OFF ), 
        AF_LAMP_OFF, AF_LAMP_ON, AF_LAMP_AUTO
    )
#endif

#if 0
    //  Flash Light
    CONFIG_FEATURE(FID_AE_STROBE, 
        BY_DEFAULT(FLASHLIGHT_FORCE_OFF), 
        FLASHLIGHT_AUTO, FLASHLIGHT_FORCE_ON, FLASHLIGHT_FORCE_OFF, 
        FLASHLIGHT_REDEYE
    )
#endif
    //..........................................................................
 //<2014/06/03,kyle chang,[8315][8317][Camera]Add HDR Scenes for Hawk35 and Hawk40   
//<2012/11/27,daisy zang,[8315][8317][Camera]Remove the other Scenes beside Auto/Night/Sunset/Party for Hawk35 and Hawk40    
#if 1
    //  Scene Mode
    CONFIG_FEATURE(FID_SCENE_MODE, 
        BY_DEFAULT(SCENE_MODE_OFF), 
        SCENE_MODE_OFF,   SCENE_MODE_NIGHTSCENE, 
        SCENE_MODE_SUNSET,SCENE_MODE_PARTY,SCENE_MODE_HDR
    )
#endif
//>2012/11/27,daisy zang
//>2014/06/03,kyle chang
    //..........................................................................
#if 0
    //  Effect
    CONFIG_FEATURE(FID_COLOR_EFFECT, 
        BY_DEFAULT(MEFFECT_OFF), 
        MEFFECT_OFF,        MEFFECT_MONO,      MEFFECT_SEPIA, 
        MEFFECT_NEGATIVE,   MEFFECT_AQUA, 
        MEFFECT_BLACKBOARD, MEFFECT_WHITEBOARD
    )
#endif
    //..........................................................................
#if 1
    //  Capture Mode
    CONFIG_FEATURE(FID_CAPTURE_MODE, 
        BY_DEFAULT(CAPTURE_MODE_NORMAL), 
        CAPTURE_MODE_NORMAL,     CAPTURE_MODE_CONTINUOUS_SHOT, 
        CAPTURE_MODE_SMILE_SHOT,
        CAPTURE_MODE_BEST_SHOT,  CAPTURE_MODE_EV_BRACKET, 
        CAPTURE_MODE_MAV,        CAPTURE_MODE_HDR,
        CAPTURE_MODE_AUTORAMA,   CAPTURE_MODE_ASD
    ) //HAWK-11936--Runal--08/08/2014 Removed Face Beauty Mode
#endif
    //..........................................................................
//<2012/11/19,daisy zang,[8315][New feature][Camera]Add picture size 16:9 (6M) for hawk 4.0 and 3:2 (4.5M) for hawk3.5
   //<2012/11/13,daisy zang,[8315][Other][Camera]Change the Picture size 5M from 1920*2560 to 1944*2592 for hawk35
#if 0
    //  Capture Size
    CONFIG_FEATURE(FID_CAP_SIZE, 
        BY_DEFAULT(CAPTURE_SIZE_2560_1920), 
        CAPTURE_SIZE_640_480,
        CAPTURE_SIZE_1280_960,  CAPTURE_SIZE_1600_1200, 
        CAPTURE_SIZE_2048_1536, CAPTURE_SIZE_2560_1920
    )
#endif
#if 1
    //  Capture Size
    CONFIG_FEATURE(FID_CAP_SIZE, 
        BY_DEFAULT( CAPTURE_SIZE_2592_1944), 
        CAPTURE_SIZE_640_480,
        CAPTURE_SIZE_1280_960,  CAPTURE_SIZE_1600_1200, 
        CAPTURE_SIZE_2048_1536, CAPTURE_SIZE_2592_1728,
        CAPTURE_SIZE_2592_1944
    )
#endif
//>2012/11/13,daisy zang
//>2012/11/19,daisy zang
    //..........................................................................
#if 0
    //  Preview Size
    CONFIG_FEATURE(FID_PREVIEW_SIZE, 
        BY_DEFAULT(PREVIEW_SIZE_640_480), 
        PREVIEW_SIZE_176_144, PREVIEW_SIZE_320_240, 
        PREVIEW_SIZE_352_288, PREVIEW_SIZE_480_368,
        PREVIEW_SIZE_640_480, PREVIEW_SIZE_720_480, PREVIEW_SIZE_800_480, 
        PREVIEW_SIZE_864_480, PREVIEW_SIZE_1280_720
    )
#endif
	//..........................................................................
//<2012/11/05-josephhsu, ALPS00383678-Hawk35-preview-not-clear
#if 1
    //  Preview Size
    CONFIG_FEATURE(FID_PREVIEW_SIZE, 
        BY_DEFAULT(PREVIEW_SIZE_640_480), 
        PREVIEW_SIZE_176_144, PREVIEW_SIZE_320_240, 
        PREVIEW_SIZE_352_288, 
		PREVIEW_SIZE_384_288, // josephhsu+ 20121105 ALPS00383678-Hawk35-preview-not-clear
		PREVIEW_SIZE_480_368,
        PREVIEW_SIZE_640_480, PREVIEW_SIZE_720_480, PREVIEW_SIZE_800_480, 
        PREVIEW_SIZE_864_480, PREVIEW_SIZE_1280_720
    )
#endif
	//..........................................................................
//>2012/11/05-josephhsu
#if 0
	//	Video Preview Size
	CONFIG_FEATURE(FID_VIDEO_PREVIEW_SIZE, 
		BY_DEFAULT(VIDEO_PREVIEW_SIZE_640_480), 
		VIDEO_PREVIEW_SIZE_640_480, VIDEO_PREVIEW_SIZE_800_600
	)
#endif
    //..........................................................................
#if 0
    //  Frame Rate
    CONFIG_FEATURE(FID_FRAME_RATE, 
        BY_DEFAULT(FRAME_RATE_300FPS), 
        FRAME_RATE_150FPS, FRAME_RATE_300FPS
    )
#endif
    //..........................................................................
#if 0
    //  Frame Rate Range
    CONFIG_FEATURE(FID_FRAME_RATE_RANGE, 
        BY_DEFAULT(FRAME_RATE_RANGE_5_30_FPS), 
        FRAME_RATE_RANGE_5_30_FPS
    )
#endif
    //..........................................................................
#if 0
    //  Focus Distance Normal
    CONFIG_FEATURE(FID_FOCUS_DIST_NORMAL, 
        BY_DEFAULT(FOCUS_DIST_N_10CM), 
        FOCUS_DIST_N_10CM
    )
#endif
    //..........................................................................
#if 0
    //  Focus Distance Macro
    CONFIG_FEATURE(FID_FOCUS_DIST_MACRO, 
        BY_DEFAULT(FOCUS_DIST_M_5CM), 
        FOCUS_DIST_M_5CM
    )
#endif
    //..........................................................................
#if 0
    //  AE Flicker
    CONFIG_FEATURE(FID_AE_FLICKER, 
        BY_DEFAULT(AE_FLICKER_MODE_AUTO), 
        AE_FLICKER_MODE_60HZ, AE_FLICKER_MODE_50HZ, 
        AE_FLICKER_MODE_AUTO, AE_FLICKER_MODE_OFF
    )
#endif
    //..........................................................................
#if 0
    //  EIS
    CONFIG_FEATURE(FID_EIS, 
        BY_DEFAULT(EIS_OFF), 
        EIS_OFF, EIS_ON
    )
#endif
    //..........................................................................
#if 1
    //	ZSD
    CONFIG_FEATURE(FID_ZSD, 
        BY_DEFAULT(ZSD_OFF), 
        ZSD_OFF, ZSD_OFF
    )
#endif        
    //==========================================================================
    //..........................................................................
#if 0
    //  AE ISO
    CONFIG_FEATURE(FID_AE_ISO, 
        BY_DEFAULT(AE_ISO_AUTO), 
        AE_ISO_AUTO, AE_ISO_100, AE_ISO_200, 
        AE_ISO_400, AE_ISO_800, AE_ISO_1600
    )
#endif
    //..........................................................................
//------------------------------------------------------------------------------
END_GETFINFO_RAW_MAIN()
};  //  namespace NSMain
};  //  namespace NSRAW


#endif // _CFG_FTBL_CUSTOM_RAW_MAIN_H_

