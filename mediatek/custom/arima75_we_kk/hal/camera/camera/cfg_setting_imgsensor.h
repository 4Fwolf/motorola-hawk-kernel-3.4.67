#ifndef _CFG_SETTING_IMGSENSOR_H_
#define _CFG_SETTING_IMGSENSOR_H_


/*******************************************************************************
* Image Sensor Orientation
*******************************************************************************/
SensorOrientation_T const&
getSensorOrientation()
{
    static SensorOrientation_T inst = {
        u4Degree_0  : 90,   //  main sensor in degree (0, 90, 180, 270)
        u4Degree_1  : 270,    //  sub  sensor in degree (0, 90, 180, 270)
    };
    //
    char str_value[PROPERTY_VALUE_MAX] = {'\0'};
    char str_default[PROPERTY_VALUE_MAX] = {'\0'};
    //
    ::sprintf(str_default, "%d", inst.u4Degree_0);
    property_get("persist.imgsensor0.orientation", str_value, str_default);
    property_set("persist.imgsensor0.orientation", str_value);
    inst.u4Degree_0 = ::atoi(str_value);
    //
    ::sprintf(str_default, "%d", inst.u4Degree_1);
    property_get("persist.imgsensor1.orientation", str_value, str_default);
    property_set("persist.imgsensor1.orientation", str_value);
    inst.u4Degree_1 = ::atoi(str_value);
    //
    return inst;
}


/*******************************************************************************
* Return fake orientation for front sensor or not
*******************************************************************************/
MBOOL isRetFakeSubOrientation()
{
	return MTRUE;  // MTRUE: return degree 90 for front sensor in degree 0 or 180; MFALSE: not return fake orientation.
}


/*******************************************************************************
* Sensor Input Data Bit Order
*   Return:
*       0   : raw data input [9:2]
*       1   : raw data input [7:0]
*       -1  : error
*******************************************************************************/
MINT32
getSensorInputDataBitOrder(EDevId const eDevId)
{
    switch  (eDevId)
    {
    case eDevId_ImgSensor0:
        return  0;
    case eDevId_ImgSensor1:
        return  0;
    default:
        break;
    }
    return  -1;
}


/*******************************************************************************
* Sensor Pixel Clock Inverse in PAD side.
*   Return:
*       0   : no inverse
*       1   : inverse
*       -1  : error
*******************************************************************************/
MINT32
getSensorPadPclkInv(EDevId const eDevId)
{
    switch  (eDevId)
    {
    case eDevId_ImgSensor0:
        return  0;
    case eDevId_ImgSensor1:
        return  0;
    default:
        break;
    }
    return  -1;
}

MINT32
getSensorViewAngle(EDevId const eDevId)
{
    switch  (eDevId)
    {
    case eDevId_ImgSensor0:
        return  ((58<<16)|(49)); //MSB16bits: horizontal view angle, LSB16bits: vertical view angle
    case eDevId_ImgSensor1:
        return  ((54<<16)|(40));//MSB16bits: horizontal view angle, LSB16bits: vertical view angle
    //case eDevId_ImgSensor2:
    //    return  0;  //MSB16bits: horizontal view angle, LSB16bits: vertical view angle
    default:
        break;
    }
    return  -1;
}




#endif //  _CFG_SETTING_IMGSENSOR_H_

