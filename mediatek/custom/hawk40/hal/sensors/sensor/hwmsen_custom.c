/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <hardware/sensors.h>
#include <linux/hwmsensor.h>
#include "hwmsen_custom.h"

struct sensor_t sSensorList[MAX_NUM_SENSORS] = 
{
//<2012/09/04, SunnyHu, porting g-sensor and m-sensor bmc050
	{ 
		.name       = "BMC050 Orientation sensor",
		.vendor     = "Bosch",
		.version    = 1,
		.handle     = ID_ORIENTATION,
		.type       = SENSOR_TYPE_ORIENTATION,
		.maxRange   = 360.0f,
		.resolution = 1.0f,
		.power      = 0.5f,
		.reserved   = {}
	},

	{ 
		.name       = "BMM050 Magnetic Field sensor",
		.vendor     = "BOSCH",
		.version    = 1,
		.handle     = ID_MAGNETIC,
		.type       = SENSOR_TYPE_MAGNETIC_FIELD,
		.maxRange   = 1600.0f,
		.resolution = 0.3f,
		.power      = 0.5f,
		.reserved   = {}
	}, 

	{  
		.name       = "BMA050 Accelerometer",
		.vendor     = "BOSCH",
		.version    = 1,
		.handle     = ID_ACCELEROMETER,
		.type       = SENSOR_TYPE_ACCELEROMETER,
		.maxRange   = 4.0f*9.805,
		.resolution = 4.0f*9.805/1024.0f,
		.power      = 0.03f,
		.reserved   = {}
	},        
//>2012/09/04, SunnyHu, porting g-sensor and m-sensor bmc050

#if 1
	{ 
		.name       = "tmd2771 Proximity Sensor",
		.vendor     = "Capella",
		.version    = 1,
		.handle     = ID_PROXIMITY,
		.type       = SENSOR_TYPE_PROXIMITY,
		.maxRange   = 1.00f,
		.resolution = 1.0f,
		.power      = 0.13f,
		.reserved   = {}
	},

	{ 
		.name       = "tmd2771 Light Sensor",
		.vendor     = "Capella",
		.version    = 1,
		.handle     = ID_LIGHT,
		.type       = SENSOR_TYPE_LIGHT,
		.maxRange   = 10240.0f,
		.resolution = 1.0f,
		.power      = 0.13f,
		.reserved   = {}
	},
#endif
//	{ 
//		.name       = "MPU3000  gyroscope Sensor",
//		.vendor     = "Invensensor",
//		.version    = 1,
//		.handle     = ID_GYROSCOPE,
//		.type       = SENSOR_TYPE_GYROSCOPE,
//		.maxRange   = 34.91f,
//		.resolution = 0.0107f,
//		.power      = 6.1f,
//		.reserved   = {}
//	},
	
};

