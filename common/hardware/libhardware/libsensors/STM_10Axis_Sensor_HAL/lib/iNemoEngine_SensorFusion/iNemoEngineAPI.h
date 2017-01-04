/*
 * Copyright (C) 2013 STMicroelectronics
 * Author:	Denis Ciocca - <denis.ciocca@st.com>
 *		Matteo Dameno - <matteo.dameno@st.com>
 *
 * iNemoEngine SensorFusion API 9Axis + 6Axis (Accel & Gyro)
 * V. 2.58
 */

#ifndef INEMO_ENGINE_API_H
#define INEMO_ENGINE_API_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


/*
 * struct iNemoInitData - Library initialization data
 * @LocalEarthMagField: Magnetic local Earth field. [uT]		(DEFAULT: 50.0f)
 * 			Max value: 60.0f
 * 			Min value: 30.0f
 * @GbiasLearningMode: Gyroscope Gbias learning mode:			(DEFAULT: 2)
 * 			1 - static learning,
 * 			2 - dynamic learning,
 * 			3 - static learning when still.
 * @Gbias_threshold_accel: Accelerometer Gbias threshold.		(DEFAULT: 525e-6)
 * @Gbias_threshold_magn: Magnetometer Gbias threshold.			(DEFAULT: 230e-6)
 * @Gbias_threshold_gyro: Gyroscope Gbias threshold.			(DEFAULT: 340e-6)
 * @ATime : Merge rate to the accelerometer.				(DEFAULT: 0.889f)
 * @MTime: Merge rate to the magnetometer.				(DEFAULT: 3.000f)
 * @PTime: Merge rate to the magnetometer / accelerometer frame.	(DEFAULT: 0.578f)
 * @FrTime: Merge rate to the accelerometer frame.			(DEFAULT: 0.667f)
 * @gbias_file: File used to store gbias data.				(DEFAULT: /data/iNemo_gbias.dat)
 */
typedef struct {
	float LocalEarthMagField;
	short GbiasLearningMode;
	float Gbias_threshold_accel;
	float Gbias_threshold_magn;
	float Gbias_threshold_gyro;
	float ATime;				/* set -1 to use default value */
	float MTime;				/* set -1 to use default value */
	float PTime;				/* set -1 to use default value */
	float FrTime;				/* set -1 to use default value */
	char *gbias_file;			/* set NULL to use default value */
} iNemoInitData;

/*
 * struct iNemoDebugInitData - Debug library init flags
 * @accel_flag: enable/disable accelerometer (0: disabled, 1: enabled).
 * @magn_flag: enable/disable magnetometer (0: disabled, 1: enabled).
 * @gyro_flag: enable/disable gyroscope (0: disabled, 1: enabled).
 */
typedef struct {
	bool accel_flag;
	bool magn_flag;
	bool gyro_flag;
} iNemoDebugInitData;

/*
 * struct iNemoDebugOutData - Debug output data
 * @Bias_meas: indicate that the bias is being measured.
 * @MA_detect: indicate the presence of a mag anomally.
 */
typedef struct {
	unsigned char Bias_meas;
	unsigned char MA_detect;
} iNemoDebugOutData;

/*
 * struct iNemoSensorsData - Sensors data
 * @accel: Accelerometer data, ENU coordinate system.		[m/s^2]
 * @magn: Magnetometer data, ENU coordinate system.		[uT]
 * @gyro: Gyroscope data, ENU coordinate system.		[rad/s]
 */
typedef struct {
	float accel[3];
	float magn[3];
	float gyro[3];
} iNemoSensorsData;


/*
 * iNemoEngine_API_Initialization: Initialize SensorFusion library,
 * 	to use only during the first library initialization
 * @init_data: Pointer to iNemoInitData structure:
 * 	If init_data pointer is NULL will be used the default values.
 * @debug_data: Pointer to iNemoDebugInitData structure:
 * 	If debug_data pointer is NULL, DEGUB mode is disabled.
 *
 * Return values:
 * 	0 on successful,
 * 	Negative number on failure.
 */
int iNemoEngine_API_Initialization(iNemoInitData *init_data,
						iNemoDebugInitData *debug_data);

/*
 * iNemoEngine_API_get_status_6X: get 6X sensor fusion status
 *
 * Return values:
 * 	true: Sensor fusion enabled,
 * 	false: Sensor fusion disabled.
 */
bool iNemoEngine_API_get_status_6X();

/*
 * iNemoEngine_API_get_status_9X: get 9X sensor fusion status
 *
 * Return values:
 * 	true: Sensor fusion enabled,
 * 	false: Sensor fusion disabled.
 */
bool iNemoEngine_API_get_status_9X();

/*
 * iNemoEngine_API_enable_6X: enable or disable 6X sensor fusion
 * @enable: true (enable), false (disable).
 */
void iNemoEngine_API_enable_6X(bool enable);

/*
 * iNemoEngine_API_enable_9X: enable or disable 9X sensor fusion
 * @enable: true (enable), false (disable).
 */
void iNemoEngine_API_enable_9X(bool enable);

/*
 * iNemoEngine_API_Run: run sensor fusion algorithm
 * @deltatime: time difference between gyroscope samples.	[nanoseconds]
 * @sdata: sensors data.
 */
void iNemoEngine_API_Run(int64_t deltatime, iNemoSensorsData *sdata);

/*
 * iNemoEngine_API_Get_Quaternion: get 9X quaternion
 * @quaternion: quaternion data [4 elements].
 *
 * Return values:
 * 	0 on successful,
 * 	Negative number on failure (data not available).
 */
int iNemoEngine_API_Get_Quaternion(float *quaternion);

/*
 * iNemoEngine_API_Get_Euler_Angles: get euler angles
 * @euler: euler angles data [3 elements].			[degree]
 *
 * Return values:
 * 	0 on successful,
 * 	Negative number on failure (data not available).
 */
int iNemoEngine_API_Get_Euler_Angles(float *euler);

/*
 * iNemoEngine_API_Get_Gravity: get gravity
 * @gravity: gravity data [3 elements].				[m/s^2]
 *
 * Return values:
 * 	0 on successful,
 * 	Negative number on failure (data not available).
 */
int iNemoEngine_API_Get_Gravity(float *gravity);

/*
 * iNemoEngine_API_Get_Linear_acceleration: get linear acceleration
 * @linear_accel: linear acceleration data [3 elements].	[m/s^2]
 *
 * Return values:
 * 	0 on successful,
 * 	Negative number on failure (data not available).
 */
int iNemoEngine_API_Get_Linear_Acceleration(float *linear_accel);

/*
 * iNemoEngine_API_Get_6X_Quaternion: get 6X quaternion
 * @quaternion: quaternion data [4 elements].
 *
 * Return values:
 * 	0 on successful,
 * 	Negative number on failure (data not available).
 */
int iNemoEngine_API_Get_6X_Quaternion(float *quaternion);

/*
 * iNemoEngine_API_Get_6X_Euler_Angles: get Euler angles
 * @euler: Euler angles data [3 elements].			[degree]
 *
 * Return values:
 * 	0 on successful,
 * 	Negative number on failure (data not available).
 */
int iNemoEngine_API_Get_6X_Euler_Angles(float *euler);

/*
 * iNemoEngine_API_Get_Gbias: get gyroscope Gbias
 * @gbias: gbias data [3 elements].				[rad/s]
 *
 * Return values:
 * 	0 on successful,
 * 	Negative number on failure: SensorFusion (9X or 6X) disabled or
 * 		data not available!
 */
int iNemoEngine_API_Get_Gbias(float *gbias);

/*
 * iNemoEngine_API_Get_Lib_Version: get library version
 * @version: library version.					[string]
 *
 * Return values:
 * 	number of characters.
 */
int iNemoEngine_API_Get_Lib_Version(char *version);

/*
 * iNemoEngine_API_Get_Debug_Flags: get debug flags
 * @debug_out_data: debug output data.				[structure data]
 */
void iNemoEngine_API_Get_Debug_Flags(iNemoDebugOutData *debug_out_data);


#endif /* INEMO_ENGINE_API_H */
