/*
 * Copyright (C) 2014 STMicroelectronics
 * API Author:	Denis Ciocca - <denis.ciocca@st.com>
 * V. 1.00
 *
 * iNemoEngine gBias estimation library
 * Author:	Antonio Micali - <antonio.micali@st.com>
 */

#ifndef INEMOENGINE_GBIAS_ESTIMATION_API_H
#define INEMOENGINE_GBIAS_ESTIMATION_API_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


/*
 * struct iNemoGbiasInitData - Library initialization data
 * @Gbias_threshold_accel: Accelerometer Gbias threshold.		(DEFAULT: 0.3)
 * @Gbias_threshold_gyro: Gyroscope Gbias threshold.			(DEFAULT: 0.4)
 * @accel_max_range: maximum range of measurement.			(DEFAULT: 19.6 m/s^2)
 * @gyro_max_range: maximum range of measurement.			(DEFAULT: 40 rad/s)
 * @gbias_file: File used to store gbias data.				(DEFAULT: /data/iNemo_gbias.dat)
 */
typedef struct {
	float Gbias_threshold_accel;
	float Gbias_threshold_gyro;
	float accel_max_range;
	float gyro_max_range;
	char *gbias_file;			/* set NULL to use default value */
} iNemoGbiasInitData;


/*
 * iNemoEngine_API_gbias_Initialization: Initialize gBias estimation library,
 * 	to use only during the first library initialization
 * @init_data: Pointer to iNemoGbiasInitData structure:
 * 	If init_data pointer is NULL will be used the default values.
 */
void iNemoEngine_API_gbias_Initialization(iNemoGbiasInitData *init_data);

/*
 * iNemoEngine_API_gbias_enable: enable or disable gBias estimation algorithm
 * @enable: true (enable), false (disable).
 */
void iNemoEngine_API_gbias_enable(bool enable);

/*
 * iNemoEngine_API_gbias_set_frequency: gBias estimation algorithm frequency
 * @frequency: gyro sampling frequency.				[Hz]
 */
void iNemoEngine_API_gbias_set_frequency(unsigned int frequency);

/*
 * iNemoEngine_API_set_accel_data: set Accelerometer data
 * @accel_data: accelerometer data. [3 elements]		[m/s^2]
 */
void iNemoEngine_API_set_accel_data(float *accel_data);

/*
 * iNemoEngine_API_gbias_Run: run gBias estimation algorithm
 * @gyro_data: gyroscope data. [3 elements]			[rad/s]
 */
void iNemoEngine_API_gbias_Run(float *gyro_data);

/*
 * iNemoEngine_API_Get_gbias: get gBias values
 * @euler: gbias data [3 elements].				[rad/s]
 *
 * Return values:
 * 	0 on successful,
 * 	Negative number on failure (data not available).
 */
int iNemoEngine_API_Get_gbias(float *gbias_out);

#endif /* INEMOENGINE_GBIAS_ESTIMATION_API_H */
