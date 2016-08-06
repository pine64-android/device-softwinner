/*
 * Copyright (C) 2013-2014 STMicroelectronics
 * Author:	Denis Ciocca - <denis.ciocca@st.com>
 *
 * iNemoEngine GeoMag Fusion API 6Axis (Accel & Magn)
 * V. 2.2
 */

#ifndef INEMO_ENGINE_GEOMAG_API_H_
#define INEMO_ENGINE_GEOMAG_API_H_

#include <stdio.h>
#include <stdlib.h>

/*
 * struct iNemoGeoMagSensorsData - Sensors data
 * @accel: Accelerometer data, ENU coordinate system.		[m/s^2]
 * @magn: Magnetometer data, ENU coordinate system.		[uT]
 */
typedef struct {
	float accel[3];
	float magn[3];
} iNemoGeoMagSensorsData;

/*
 * iNemoEngine_GeoMag_API_Initialization: Initialize GeoMag Fusion library,
 *	to use on every enable
 */
void iNemoEngine_GeoMag_API_Initialization();

/*
 * iNemoEngine_GeoMag_API_Run: run sensor fusion algorithm
 * @deltatime: time interval between samples.			[ms]
 * @sdata: sensors data.
 */
void iNemoEngine_GeoMag_API_Run(int deltatime, iNemoGeoMagSensorsData *sdata);

/*
 * iNemoEngine_GeoMag_API_Get_Quaternion: get GeoMag quaternion
 * @quaternion: quaternion data [4 elements].
 *
 * Return values:
 *	0 on successful,
 *	Negative number on failure (data not available).
 */
int iNemoEngine_GeoMag_API_Get_Quaternion(float *quaternion);

/*
 * iNemoEngine_GeoMag_API_Get_Hpr: get Heading, Pitch and Roll
 * @Hpr: Orientation array [3 elements]
 *
 * Return values:
 *	0 on successful,
 *	Negative number on failure (data not available).
 */
int iNemoEngine_GeoMag_API_Get_Hpr(float *Hpr);

/*
 * iNemoEngine_GeoMag_API_Get_LinAcc: get Linear Accelerations
 * @Lacc: Linear acceleration array [3 elements]
 *
 * Return values:
 *	0 on successful,
 *	Negative number on failure (data not available).
 */
int iNemoEngine_GeoMag_API_Get_LinAcc(float *Lacc);

/*
 * iNemoEngine_GeoMag_API_Get_Gravity: get Gravity
 * @Gravity: Gravity array [3 elements]
 *
 * Return values:
 *	0 on successful,
 *	Negative number on failure (data not available).
 */
int iNemoEngine_GeoMag_API_Get_Gravity(float *Gravity);

/*
 * iNemoEngine_GeoMag_API_Get_Lib_Version: get library version
 * @version: library version.					[string]
 *
 * Return values:
 *	number of characters.
 */
int iNemoEngine_GeoMag_API_Get_Lib_Version(char *version);

/*
 * iNemoEngine_GeoMag_API_Get_VirtualGyro: get Virtual Gyro angular rate
 * gyro_vel: angular rate [4 elements].				[rad/sec]
 *
 * Return values:
 *	0 on successful,
 *	Negative number on failure (data not available).
 */
int iNemoEngine_GeoMag_API_Get_VirtualGyro(float *gyro_vel);

#endif /* INEMO_ENGINE_GEOMAG_API_H_ */
