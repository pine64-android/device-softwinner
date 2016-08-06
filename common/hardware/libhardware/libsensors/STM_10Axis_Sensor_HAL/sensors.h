/*
 * Copyright (C) 2012 STMicroelectronics
 * Matteo Dameno, Denis Ciocca, Alberto Marinoni - Motion MEMS Product Div.
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

#ifndef ANDROID_SENSORS_H
#define ANDROID_SENSORS_H

#include <math.h>
#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <linux/input.h>

#include <hardware/hardware.h>
#include <hardware/sensors.h>
#include "configuration.h"

__BEGIN_DECLS

#define SUPPORT_SENSORS_NUMBER          (20)
#define ICHAR                           (';')
/*****************************************************************************/

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define ID_BASE					SENSORS_HANDLE_BASE
#define ID_ACCELEROMETER			(ID_BASE+0)
#define ID_MAGNETIC_FIELD			(ID_BASE+1)
#define ID_ORIENTATION				(ID_BASE+2)
#define ID_GYROSCOPE				(ID_BASE+3)
#define ID_LIGHT				(ID_BASE+4)
#define ID_PRESSURE				(ID_BASE+5)
#define ID_TEMPERATURE				(ID_BASE+6)
#define ID_PROXIMITY				(ID_BASE+7)
#define ID_GRAVITY				(ID_BASE+8)
#define ID_LINEAR_ACCELERATION			(ID_BASE+9)
#define ID_ROTATION_VECTOR			(ID_BASE+10)
#define ID_GAME_ROTATION			(ID_BASE+11)
#define ID_UNCALIB_GYROSCOPE			(ID_BASE+12)
#define ID_SIGNIFICANT_MOTION			(ID_BASE+13)
#define ID_UNCALIB_MAGNETIC_FIELD		(ID_BASE+14)
#define ID_GEOMAG_ROTATION_VECTOR		(ID_BASE+15)
#define ID_SENSOR_FUSION			(ID_BASE+16)
#define ID_VIRTUAL_GYROSCOPE			(ID_BASE+17)

#define SENSORS_ACCELEROMETER_HANDLE		ID_ACCELEROMETER
#define SENSORS_MAGNETIC_FIELD_HANDLE		ID_MAGNETIC_FIELD
#define SENSORS_ORIENTATION_HANDLE		ID_ORIENTATION
#define SENSORS_LIGHT_HANDLE			ID_LIGHT
#define SENSORS_PROXIMITY_HANDLE		ID_PROXIMITY
#define SENSORS_GYROSCOPE_HANDLE		ID_GYROSCOPE
#define SENSORS_GRAVITY_HANDLE			ID_GRAVITY
#define SENSORS_LINEAR_ACCELERATION_HANDLE	ID_LINEAR_ACCELERATION
#define SENSORS_ROTATION_VECTOR_HANDLE		ID_ROTATION_VECTOR
#define SENSORS_PRESSURE_HANDLE			ID_PRESSURE
#define SENSORS_TEMPERATURE_HANDLE		ID_TEMPERATURE
#define SENSORS_GAME_ROTATION_HANDLE		ID_GAME_ROTATION
#define SENSORS_UNCALIB_GYROSCOPE_HANDLE	ID_UNCALIB_GYROSCOPE
#define SENSORS_SIGNIFICANT_MOTION_HANDLE	ID_SIGNIFICANT_MOTION
#define SENSORS_UNCALIB_MAGNETIC_FIELD_HANDLE	ID_UNCALIB_MAGNETIC_FIELD
#define SENSORS_GEOMAG_ROTATION_VECTOR_HANDLE	ID_GEOMAG_ROTATION_VECTOR
#define SENSORS_SENSOR_FUSION_HANDLE		ID_SENSOR_FUSION
#define SENSORS_VIRTUAL_GYROSCOPE_HANDLE	ID_VIRTUAL_GYROSCOPE

/*****************************************************************************/
/* EVENT TYPE */
/*****************************************************************************/

/* Event Type in accelerometer sensor: see input_set_abs_params() function in your input driver */
#define EVENT_TYPE_ACCEL_X		ABS_X
#define EVENT_TYPE_ACCEL_Y		ABS_Y
#define EVENT_TYPE_ACCEL_Z		ABS_Z


/* Event Type in magnetometer sensor: see input_set_abs_params() function in your input driver */
#define EVENT_TYPE_MAG_X		ABS_X
#define EVENT_TYPE_MAG_Y		ABS_Y
#define EVENT_TYPE_MAG_Z		ABS_Z


/* Event Type in gyroscope sensor: see input_set_abs_params() function in your input driver */
#define EVENT_TYPE_GYRO_X		ABS_X
#define EVENT_TYPE_GYRO_Y		ABS_Y
#define EVENT_TYPE_GYRO_Z		ABS_Z


/* Event Type in pressure sensor: see input_set_abs_params() function in your input driver */
#define EVENT_TYPE_PRESSURE		ABS_PR


/* Event Type in temperature sensor: see input_set_abs_params() function in your input driver */
#define EVENT_TYPE_TEMPERATURE		ABS_TEMP

/* Event Type in temperature sensor: see input_set_abs_params() function in your input driver */
#define EVENT_TYPE_SIGNIFICANT_MOTION	ABS_WHEEL
struct sensor_info{
        char sensorName[64];
        char classPath[128];
        float priData;
};
struct sensors_data{
        char name[64];
        float lsg;
};
struct sensor_extend_t{
        struct sensors_data sensors;
        struct sensor_t sList;
};
struct status{
        bool isUsed;
        bool isFound;
};
struct o_device{
        int isFind;
        char name[32];
};
extern int insmodDevice(void);
extern int sensorsDetect(void);
extern struct status seStatus[SUPPORT_SENSORS_NUMBER];
extern struct sensor_info gsensorInfo;
extern struct sensor_info magSensorInfo;
extern struct sensor_info gyrSensorInfo;
extern struct sensor_info ligSensorInfo;
extern struct sensor_info proSensorInfo;
extern struct sensor_info oriSensorInfo;
extern struct sensor_info tempSensorInfo;
extern struct sensor_info preSensorInfo;

/* The SENSORS Module */
static const struct sensor_t sSensorList[] = {
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
	{
	SENSOR_ACC_LABEL,
	"STMicroelectronics",
	1,
	SENSORS_ACCELEROMETER_HANDLE,
	SENSOR_TYPE_ACCELEROMETER,
	ACCEL_MAX_RANGE,
	0.0f,
	ACCEL_POWER_CONSUMPTION,
	(1000*1000)/ACCEL_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_ACCELEROMETER,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{ }
	},
#endif
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
	{
	SENSOR_MAGN_LABEL,
	"STMicroelectronics",
	1,
	SENSORS_MAGNETIC_FIELD_HANDLE,
	SENSOR_TYPE_MAGNETIC_FIELD,
	MAGN_MAX_RANGE,
	0.0f,
	MAGN_POWER_CONSUMPTION,
	(1000*1000)/MAGN_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_MAGNETIC_FIELD,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{ }
	},
#endif
#if (SENSORS_GYROSCOPE_ENABLE == 1)
	{
	SENSOR_GYRO_LABEL,
	"STMicroelectronics",
	1,
	SENSORS_GYROSCOPE_HANDLE,
	SENSOR_TYPE_GYROSCOPE,
	GYRO_MAX_RANGE,
	0.0f,
	GYRO_POWER_CONSUMPTION,
	(1000*1000)/GYRO_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_GYROSCOPE,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{ }
	},
#endif
#if ((SENSORS_UNCALIB_GYROSCOPE_ENABLE == 1) && ((GYROSCOPE_GBIAS_ESTIMATION_FUSION == 1) || (GYROSCOPE_GBIAS_ESTIMATION_STANDALONE == 1)))
	{
	SENSOR_UNCALIB_GYRO_LABEL,
	"STMicroelectronics",
	1,
	SENSORS_UNCALIB_GYROSCOPE_HANDLE,
	SENSOR_TYPE_GYROSCOPE_UNCALIBRATED,
	GYRO_MAX_RANGE,
	0.0f,
	UNCALIB_GYRO_POWER_CONSUMPTION,
	(1000*1000)/GYRO_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_GYROSCOPE_UNCALIBRATED,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{ }
	},
#endif
#if (SENSORS_SIGNIFICANT_MOTION_ENABLE == 1)
	{
	SENSOR_SIGNIFICANT_MOTION_LABEL,
	"STMicroelectronics",
	1,
	SENSORS_SIGNIFICANT_MOTION_HANDLE,
	SENSOR_TYPE_SIGNIFICANT_MOTION,
	1.0f,
	0.0f,
	ACCEL_POWER_CONSUMPTION,
	-1,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_SIGNIFICANT_MOTION,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{ }
	},
#endif
#if ((SENSORS_ORIENTATION_ENABLE == 1) && ((SENSOR_FUSION_ENABLE == 1) || (MAG_CALIBRATION_ENABLE ==1)))
	{
#if (SENSOR_FUSION_ENABLE == 1)
	"iNemoEngine Orientation sensor",
#else
#if (SENSOR_GEOMAG_ENABLE == 1)
	"iNemoEngineGeoMag Orientation sensor",
#else
	"ST Orientation sensor",
#endif
#endif
	"STMicroelectronics",
	1,
	SENSORS_ORIENTATION_HANDLE,
	SENSOR_TYPE_ORIENTATION,
	360.0f,
	0.0f,
	ORIENTATION_POWER_CONSUMPTION,
	(1000*1000)/ORIENTATION_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_ORIENTATION,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{ }
	},
#endif
#if ((SENSORS_GRAVITY_ENABLE == 1) && ((SENSOR_FUSION_ENABLE == 1) || (MAG_CALIBRATION_ENABLE ==1)))
	{
	"iNemoEngine Gravity sensor",
	"STMicroelectronics",
	1,
	SENSORS_GRAVITY_HANDLE,
	SENSOR_TYPE_GRAVITY,
	GRAVITY_EARTH,
	0.0f,
	GRAVITY_POWER_CONSUMPTION,
	(1000*1000)/FUSION_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_GRAVITY,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{}
	},
#endif
#if ((SENSORS_LINEAR_ACCELERATION_ENABLE == 1) && ((SENSOR_FUSION_ENABLE == 1) || (MAG_CALIBRATION_ENABLE ==1)))
	{
	"iNemoEngine Linear Acceleration sensor",
	"STMicroelectronics",
	1,
	SENSORS_LINEAR_ACCELERATION_HANDLE,
	SENSOR_TYPE_LINEAR_ACCELERATION,
	ACCEL_MAX_RANGE-GRAVITY_EARTH,
	0.0f,
	LINEAR_ACCEL_POWER_CONSUMPTION,
	(1000*1000)/FUSION_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_LINEAR_ACCELERATION,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{}
	},
#endif
#if ((SENSORS_ROTATION_VECTOR_ENABLE == 1) && (SENSOR_FUSION_ENABLE == 1))
	{
	"iNemoEngine Rotation_Vector sensor",
	"STMicroelectronics",
	1,
	SENSORS_ROTATION_VECTOR_HANDLE,
	SENSOR_TYPE_ROTATION_VECTOR,
#if (!SENSORS_GYROSCOPE_ENABLE && SENSORS_VIRTUAL_GYROSCOPE_ENABLE)
	VIRTUAL_GYRO_MAX_RANGE,
#else
	GYRO_MAX_RANGE,
#endif
	0.0f,
	ROT_VEC_POWER_CONSUMPTION,
	(1000*1000)/FUSION_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_ROTATION_VECTOR,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{}
	},
#endif
#if ((SENSORS_GAME_ROTATION_ENABLE == 1) && (SENSOR_FUSION_ENABLE == 1))
	{
	"iNemoEngine Game Rotation sensor",
	"STMicroelectronics",
	1,
	SENSORS_GAME_ROTATION_HANDLE,
	SENSOR_TYPE_GAME_ROTATION_VECTOR,
#if (!SENSORS_GYROSCOPE_ENABLE && SENSORS_VIRTUAL_GYROSCOPE_ENABLE)
	VIRTUAL_GYRO_MAX_RANGE,
#else
	GYRO_MAX_RANGE,
#endif
	0.0f,
	GYRO_POWER_CONSUMPTION+ACCEL_POWER_CONSUMPTION,
	(1000*1000)/FUSION_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_GAME_ROTATION_VECTOR,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{ }
	},
#endif
#if (SENSORS_PRESSURE_ENABLE == 1)
	{
	SENSOR_PRESS_LABEL,
	"STMicroelectronics",
	1,
	SENSORS_PRESSURE_HANDLE,
	SENSOR_TYPE_PRESSURE,
	PRESS_MAX_RANGE,
	0.0f,
	PRESS_TEMP_POWER_CONSUMPTION,
	(1000*1000)/PRESS_TEMP_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_PRESSURE,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{}
	},
#endif
#if (SENSORS_TEMPERATURE_ENABLE == 1)
	{
	SENSOR_TEMP_LABEL,
	"STMicroelectronics",
	1,
	SENSORS_TEMPERATURE_HANDLE,
	SENSOR_TYPE_TEMPERATURE,
	TEMP_MAX_RANGE,
	0.0f,
	PRESS_TEMP_POWER_CONSUMPTION,
	(1000*1000)/PRESS_TEMP_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_TEMPERATURE,
	0,0,SENSOR_FLAG_WAKE_UP|SENSOR_FLAG_ON_CHANGE_MODE,
#endif
	{}
	},
#endif
#if (SENSORS_UNCALIB_MAGNETIC_FIELD_ENABLE == 1)
	{
	SENSOR_UNCALIB_MAGN_LABEL,
	"STMicroelectronics",
	1,
	SENSORS_UNCALIB_MAGNETIC_FIELD_HANDLE,
	SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED,
	MAGN_MAX_RANGE,
	0.0f,
	MAGN_POWER_CONSUMPTION,
	(1000*1000)/MAGN_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_MAGNETIC_FIELD_UNCALIBRATED,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{ }
	},
#endif
#if (SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1)
	{
	"iNemoEngine Geomagnetic Rotation Vector sensor",
	"STMicroelectronics",
	1,
	SENSORS_GEOMAG_ROTATION_VECTOR_HANDLE,
	SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR,
	1.0f,
	0.0f,
	MAGN_POWER_CONSUMPTION+ACCEL_POWER_CONSUMPTION,
	(1000*1000)/MAGN_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_GEOMAGNETIC_ROTATION_VECTOR,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{ }
	},
#endif
#if (SENSORS_VIRTUAL_GYROSCOPE_ENABLE == 1)
	{
	SENSOR_VIRTUAL_GYRO_LABEL,
	"STMicroelectronics",
	1,
	SENSORS_VIRTUAL_GYROSCOPE_HANDLE,
	SENSOR_TYPE_GYROSCOPE,
	VIRTUAL_GYRO_MAX_RANGE,
	0.0f,
	VIRTUAL_GYRO_POWER_CONSUMPTION,
	(1000*1000)/VIRTUAL_GYRO_MAX_ODR,
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_GYROSCOPE,
	0,0,SENSOR_FLAG_WAKE_UP,
#endif
	{ }
	},
#endif
	{
        "jsa1127 Light sensor",
        "SOLTEAM",
        1,
        SENSORS_LIGHT_HANDLE,
        SENSOR_TYPE_LIGHT,
        100000.0f, 
	1.0f,
        0.005f, 
	0, 
#if (ANDROID_VERSION >= ANDROIS_KK)
	0,
	0,
	SENSOR_STRING_TYPE_LIGHT,
	0,0,SENSOR_FLAG_WAKE_UP|SENSOR_FLAG_ON_CHANGE_MODE,
#endif
	{ }
        },

};

__END_DECLS

#endif  // ANDROID_SENSORS_H
