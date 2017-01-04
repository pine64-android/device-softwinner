/*
te * Copyright (C) 2012 STMicroelectronics
 * Matteo Dameno, Ciocca Denis, Alberto Marinoni - Motion MEMS Product Div.
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

#include "configuration.h"
#if (SENSOR_FUSION_ENABLE == 1)

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <cutils/log.h>
#include <linux/time.h>

#include "iNemoEngineSensor.h"
#include "iNemoEngineAPI.h"

#if (SENSORS_ACCELEROMETER_ENABLE == 1)
#include SENSOR_ACC_INCLUDE_FILE_NAME
#endif

#if (SENSORS_GYROSCOPE_ENABLE == 1)
#include SENSOR_GYR_INCLUDE_FILE_NAME
#endif

#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
#include SENSOR_MAG_INCLUDE_FILE_NAME
#endif

#define FETCH_FULL_EVENT_BEFORE_RETURN		0

/*****************************************************************************/
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
MagnSensor* iNemoEngineSensor::mag = NULL;
#endif
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
AccelSensor* iNemoEngineSensor::acc = NULL;
#endif
#if (SENSORS_GYROSCOPE_ENABLE == 1)
GyroSensor* iNemoEngineSensor::gyr = NULL;
#endif
int iNemoEngineSensor::status = 0;
int iNemoEngineSensor::mEnabled = 0;
int iNemoEngineSensor::startup_samples = DEFAULT_SAMPLES_TO_DISCARD;
int iNemoEngineSensor::samples_to_discard = DEFAULT_SAMPLES_TO_DISCARD;
int iNemoEngineSensor::DecimationBuffer[numSensors] = {0};
int iNemoEngineSensor::Orientation_decimation_count = 0;
int iNemoEngineSensor::Gravity_decimation_count = 0;
int iNemoEngineSensor::LinearAcceleration_decimation_count = 0;
int iNemoEngineSensor::RotationMatrix_decimation_count = 0;
int iNemoEngineSensor::GameRotation_decimation_count = 0;
int iNemoEngineSensor::UncalibGyro_decimation_count = 0;
int iNemoEngineSensor::CalibGyro_decimation_count = 0;
int64_t iNemoEngineSensor::DelayBuffer[numSensors] = {0};
int64_t iNemoEngineSensor::Gyro_Delay_ms = GYR_DEFAULT_DELAY*1000000;

struct timespec old_time, new_time;


int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
	int64_t timeAft;
	int64_t timeBef;
	int64_t timeDiff;

	timeBef = (int64_t)((int64_t)(timeB_p->tv_sec * 1000000000L) + timeB_p->tv_nsec);
	timeAft = (int64_t)((int64_t)(timeA_p->tv_sec * 1000000000L) + timeA_p->tv_nsec);

	if (timeAft > timeBef)
		timeDiff = timeAft - timeBef;
	else
		timeDiff = timeBef - timeAft;

	return (int64_t)timeDiff;
}

iNemoEngineSensor::iNemoEngineSensor()
#if (!SENSORS_GYROSCOPE_ENABLE && SENSORS_VIRTUAL_GYROSCOPE_ENABLE)
       : SensorBase(NULL, SENSOR_DATANAME_ACCELEROMETER),
#else
        : SensorBase(NULL, SENSOR_DATANAME_GYROSCOPE),
#endif
	mPendingMask(0),
	mInputReader(4),
	mHasPendingEvent(false)
{
	int err;

	memset(mPendingEvents, 0, sizeof(mPendingEvents));

#if (SENSORS_ORIENTATION_ENABLE == 1)
	mPendingEvents[Orientation].version = sizeof(sensors_event_t);
	mPendingEvents[Orientation].sensor = ID_ORIENTATION;
	mPendingEvents[Orientation].type = SENSOR_TYPE_ORIENTATION;
	mPendingEvents[Orientation].orientation.status = SENSOR_STATUS_ACCURACY_HIGH;
#endif

#if (SENSORS_GRAVITY_ENABLE == 1)
	mPendingEvents[Gravity].version = sizeof(sensors_event_t);
	mPendingEvents[Gravity].sensor = ID_GRAVITY;
	mPendingEvents[Gravity].type = SENSOR_TYPE_GRAVITY;
	mPendingEvents[Gravity].acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;
#endif
#if (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)
	mPendingEvents[LinearAcceleration].version = sizeof(sensors_event_t);
	mPendingEvents[LinearAcceleration].sensor = ID_LINEAR_ACCELERATION;
	mPendingEvents[LinearAcceleration].type = SENSOR_TYPE_LINEAR_ACCELERATION;
	mPendingEvents[LinearAcceleration].acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;
#endif
#if (SENSORS_ROTATION_VECTOR_ENABLE == 1)
	mPendingEvents[RotationMatrix].version = sizeof(sensors_event_t);
	mPendingEvents[RotationMatrix].sensor = ID_ROTATION_VECTOR;
	mPendingEvents[RotationMatrix].type = SENSOR_TYPE_ROTATION_VECTOR;
#endif
#if (SENSORS_GAME_ROTATION_ENABLE == 1)
	mPendingEvents[GameRotation].version = sizeof(sensors_event_t);
	mPendingEvents[GameRotation].sensor = ID_GAME_ROTATION;
	mPendingEvents[GameRotation].type = SENSOR_TYPE_GAME_ROTATION_VECTOR;
#endif
#if (GYROSCOPE_GBIAS_ESTIMATION_FUSION == 1)
#if (SENSORS_UNCALIB_GYROSCOPE_ENABLE == 1)
	mPendingEvents[UncalibGyro].version = sizeof(sensors_event_t);
	mPendingEvents[UncalibGyro].sensor = ID_UNCALIB_GYROSCOPE;
	mPendingEvents[UncalibGyro].type = SENSOR_TYPE_GYROSCOPE_UNCALIBRATED;
	mPendingEvents[UncalibGyro].gyro.status = SENSOR_STATUS_ACCURACY_HIGH;
#endif
	mPendingEvents[CalibGyro].version = sizeof(sensors_event_t);
	mPendingEvents[CalibGyro].sensor = ID_GYROSCOPE;
	mPendingEvents[CalibGyro].type = SENSOR_TYPE_GYROSCOPE;
	mPendingEvents[CalibGyro].gyro.status = SENSOR_STATUS_ACCURACY_HIGH;
#endif

	if (data_fd) {
		STLOGI("iNemoSensor::iNemoSensor main driver device_sysfs_path:(%s)", sysfs_device_path);
	} else {
		STLOGE("iNemoSensor::iNemoSensor main driver device_sysfs_path:(%s) not found", sysfs_device_path);
	}

	init_data_api.GbiasLearningMode = 2;
	init_data_api.ATime = -1;
	init_data_api.MTime = -1;
	init_data_api.PTime = -1;
	init_data_api.FrTime = -1;
	init_data_api.gbias_file = NULL;

	debug_init_data_api.accel_flag = 0;
	debug_init_data_api.magn_flag = 0;
	debug_init_data_api.gyro_flag = 0;

#if (SENSORS_ACCELEROMETER_ENABLE == 1)
	init_data_api.Gbias_threshold_accel = ACC_GBIAS_THRESHOLD;
	debug_init_data_api.accel_flag = 1;
#endif
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
	init_data_api.LocalEarthMagField = LOCAL_EARTH_MAGNETIC_FIELD;
	init_data_api.Gbias_threshold_magn = MAG_GBIAS_THRESHOLD;
	debug_init_data_api.magn_flag = 1;
#endif
#if (SENSORS_GYROSCOPE_ENABLE == 1)
	debug_init_data_api.gyro_flag = 1;
	init_data_api.Gbias_threshold_gyro = GYR_GBIAS_THRESHOLD;
#endif

	err = iNemoEngine_API_Initialization(&init_data_api, &debug_init_data_api);
	if (err < 0)
		STLOGE("iNemoSensor:: Failed to initialize iNemoEngineAPI library");

#if (SENSORS_GYROSCOPE_ENABLE == 1)
	iNemoEngineSensor::gyr = new GyroSensor();
#endif
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
	iNemoEngineSensor::mag = new MagnSensor();
#endif
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
	iNemoEngineSensor::acc = new AccelSensor();
#endif
}


iNemoEngineSensor::~iNemoEngineSensor()
{
#if (SENSORS_GYROSCOPE_ENABLE == 1)
	iNemoEngineSensor::gyr->~SensorBase();
#endif
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
	iNemoEngineSensor::mag->~SensorBase();
#endif
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
	iNemoEngineSensor::acc->~SensorBase();
#endif
}

int iNemoEngineSensor::setInitialState()
{
	int clockid = CLOCK_BOOTTIME;
	
	if (!ioctl(data_fd, EVIOCSCLOCKID, &clockid))
		ALOGE("iNemoEngineSensor : set EVIOCSCLOCKID = %d\n",clockid);
	else
		ALOGE("iNemoEngineSensor : set EVIOCSCLOCKID failed ! \n");

	startup_samples = samples_to_discard;
	return 0;
}

int iNemoEngineSensor::enable(int32_t handle, int en, int type)
{
	int err = 0;
	int what = -1;
	static int enabled = 0;

#if (SENSORS_GYROSCOPE_ENABLE == 1)
	if (iNemoEngineSensor::gyr->getFd() <= 0)
		return -1;
#endif
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
	if (iNemoEngineSensor::mag->getFd() <= 0)
		return -1;
#endif
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
	if (iNemoEngineSensor::acc->getFd() <= 0)
		return -1;
#endif

	switch(handle) {
#if (SENSORS_ORIENTATION_ENABLE == 1)
		case SENSORS_ORIENTATION_HANDLE:
			what = Orientation;
			break;
#endif
#if (SENSORS_GRAVITY_ENABLE == 1)
		case SENSORS_GRAVITY_HANDLE:
			what = Gravity;
			break;
#endif
#if (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)
		case SENSORS_LINEAR_ACCELERATION_HANDLE:
			what = LinearAcceleration;
			break;
#endif
#if (SENSORS_ROTATION_VECTOR_ENABLE == 1)
		case SENSORS_ROTATION_VECTOR_HANDLE:
			what = RotationMatrix;
			break;
#endif
#if (SENSORS_GAME_ROTATION_ENABLE == 1)
		case SENSORS_GAME_ROTATION_HANDLE:
			what = GameRotation;
			break;
#endif
#if (GYROSCOPE_GBIAS_ESTIMATION_FUSION == 1)
#if (SENSORS_UNCALIB_GYROSCOPE_ENABLE == 1)
		case SENSORS_UNCALIB_GYROSCOPE_HANDLE:
			what = UncalibGyro;
			break;
#endif
		case SENSORS_GYROSCOPE_HANDLE:
			what = CalibGyro;
			break;
#endif
		default:
			return -1;
	}

	int flags = en ? 1 : 0;

	if(flags) {
		if(mEnabled == 0) {
			enabled = 1;

#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
			iNemoEngineSensor::mag->enable(SENSORS_SENSOR_FUSION_HANDLE, 1, 1);
			iNemoEngineSensor::mag->setFullScale(SENSORS_SENSOR_FUSION_HANDLE, MAG_DEFAULT_RANGE);
#endif
#if (SENSORS_GYROSCOPE_ENABLE == 1)
			iNemoEngineSensor::gyr->enable(SENSORS_SENSOR_FUSION_HANDLE, 1, 1);
			iNemoEngineSensor::gyr->setFullScale(SENSORS_SENSOR_FUSION_HANDLE, GYRO_DEFAULT_RANGE);
#endif
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
			iNemoEngineSensor::acc->enable(SENSORS_SENSOR_FUSION_HANDLE, 1, 1);
			iNemoEngineSensor::acc->setFullScale(SENSORS_SENSOR_FUSION_HANDLE, ACC_DEFAULT_RANGE);
#endif

		}
		mEnabled |= (1<<what);
	} else {
		int tmp = mEnabled;
		mEnabled &= ~(1<<what);
		if((mEnabled == 0)&&(tmp != 0)) {
#if (SENSORS_GYROSCOPE_ENABLE == 1)
			iNemoEngineSensor::gyr->setFullScale(SENSORS_SENSOR_FUSION_HANDLE, GYRO_DEFAULT_RANGE);
			iNemoEngineSensor::gyr->enable(SENSORS_SENSOR_FUSION_HANDLE, 0, 1);
#endif
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
			iNemoEngineSensor::mag->setFullScale(SENSORS_SENSOR_FUSION_HANDLE, MAG_DEFAULT_RANGE);
			iNemoEngineSensor::mag->enable(SENSORS_SENSOR_FUSION_HANDLE, 0, 1);
#endif
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
			iNemoEngineSensor::acc->enable(SENSORS_SENSOR_FUSION_HANDLE, 0, 1);
#endif
		}
	}

	if ((handle == SENSORS_GAME_ROTATION_HANDLE) ||
		(handle == SENSORS_GYROSCOPE_HANDLE) ||
		(handle == SENSORS_UNCALIB_GYROSCOPE_HANDLE))
	{
		if ((mEnabled & (1<<GameRotation)) ||
		    (mEnabled & (1<<UncalibGyro)) ||
		    (mEnabled & (1<<CalibGyro)))
		{
				iNemoEngine_API_enable_6X(true);
#if DEBUG_INEMO_SENSOR == 1
				STLOGD( "iNemoEngineSensor: Fusion 6axis enabled");
#endif
		} else {
				iNemoEngine_API_enable_6X(false);
#if DEBUG_INEMO_SENSOR == 1
				STLOGD( "iNemoEngineSensor: Fusion 6axis disabled");
#endif
		}
	} else {
		if ((mEnabled & (1<<Orientation)) ||
			(mEnabled & (1<<Gravity)) ||
			(mEnabled & (1<<LinearAcceleration)) ||
			(mEnabled & (1<<RotationMatrix)))
		{
			iNemoEngine_API_enable_9X(true);
#if DEBUG_INEMO_SENSOR == 1
			STLOGD( "iNemoEngineSensor: Fusion 9axis enabled");
#endif
		} else {
			iNemoEngine_API_enable_9X(false);
#if DEBUG_INEMO_SENSOR == 1
			STLOGD( "iNemoEngineSensor: Fusion 9axis disabled");
#endif
		}
	}

	if (enabled == 1) {
		enabled = 0;
		setInitialState();
		clock_gettime(CLOCK_MONOTONIC, &old_time);
	}

	return err;
}

bool iNemoEngineSensor::hasPendingEvents() const
{
	return mHasPendingEvent;
}

int iNemoEngineSensor::setDelay(int32_t handle, int64_t delay_ns)
{
	int err;
	int what = -1;
	int64_t delay_ms = delay_ns/1000000;

	switch(handle) {
#if (SENSORS_ORIENTATION_ENABLE == 1)
		case SENSORS_ORIENTATION_HANDLE:
			what = Orientation;
			break;
#endif
#if (SENSORS_GRAVITY_ENABLE == 1)
		case SENSORS_GRAVITY_HANDLE:
			what = Gravity;
			break;
#endif
#if (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)
		case SENSORS_LINEAR_ACCELERATION_HANDLE:
			what = LinearAcceleration;
			break;
#endif
#if (SENSORS_ROTATION_VECTOR_ENABLE == 1)
		case SENSORS_ROTATION_VECTOR_HANDLE:
			what = RotationMatrix;
			break;
#endif
#if (SENSORS_GAME_ROTATION_ENABLE == 1)
		case SENSORS_GAME_ROTATION_HANDLE:
			what = GameRotation;
			break;
#endif
#if (GYROSCOPE_GBIAS_ESTIMATION_FUSION == 1)
#if (SENSORS_UNCALIB_GYROSCOPE_ENABLE == 1)
		case SENSORS_UNCALIB_GYROSCOPE_HANDLE:
			what = UncalibGyro;
			break;
#endif
		case SENSORS_GYROSCOPE_HANDLE:
			what = CalibGyro;
			break;
#endif
		default:
			return -1;
	}

#if (SENSORS_GYROSCOPE_ENABLE == 1)
		err = iNemoEngineSensor::gyr->setDelay(SENSORS_SENSOR_FUSION_HANDLE, (int64_t)(GYR_DEFAULT_DELAY*1000000));
		if(err < 0)
			return -1;
#endif
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
		err = iNemoEngineSensor::mag->setDelay(SENSORS_SENSOR_FUSION_HANDLE, (int64_t)(MAG_DEFAULT_DELAY*1000000));
		if(err < 0)
			return -1;
#endif
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
		err = iNemoEngineSensor::acc->setDelay(SENSORS_SENSOR_FUSION_HANDLE, (int64_t)(ACC_DEFAULT_DELAY*1000000));
		if(err < 0)
			return -1;
#endif

	// DelayBuffer
	if ((mEnabled & 1<<what) != 0) {
		DelayBuffer[what] = delay_ms;
		updateDecimations(Gyro_Delay_ms);
	} else {
		DelayBuffer[what] = 0;
	}

	return 0;
}

void iNemoEngineSensor::updateDecimations(int64_t Gyro_Delay_ms)
{
	int kk;

	samples_to_discard = (int)(GYRO_STARTUP_TIME_MS/Gyro_Delay_ms)+1;
	startup_samples = samples_to_discard;

	// Decimation Update
	for(kk = 0; kk < numSensors; kk++)
	{
		if ((mEnabled & 1<<kk) != 0)
			DecimationBuffer[kk] = DelayBuffer[kk]/Gyro_Delay_ms;
		else
			DecimationBuffer[kk] = 0;
	}

	Orientation_decimation_count = 0;
	Gravity_decimation_count = 0;
	LinearAcceleration_decimation_count = 0;
	RotationMatrix_decimation_count = 0;
	GameRotation_decimation_count = 0;
	UncalibGyro_decimation_count = 0;
	CalibGyro_decimation_count = 0;

#if DEBUG_INEMO_SENSOR == 1
	STLOGD("iNemo::Gyro Delay = %lld", Gyro_Delay_ms);
	STLOGD("iNemo::DelayBuffer = %lld, %lld, %lld", DelayBuffer[3], DelayBuffer[4], DelayBuffer[5]);
	STLOGD("iNemo::DelayBuffer = %lld, %lld, %lld", DelayBuffer[6], DelayBuffer[7], DelayBuffer[8]);
	STLOGD("iNemo::DelayBuffer = %lld", DelayBuffer[9]);
	STLOGD("iNemo::DecimationBuffer = %d, %d, %d", DecimationBuffer[3], DecimationBuffer[4], DecimationBuffer[5]);
	STLOGD("iNemo::DecimationBuffer = %d, %d, %d", DecimationBuffer[6], DecimationBuffer[7], DecimationBuffer[8]);
	STLOGD("iNemo::DecimationBuffer = %d", DecimationBuffer[9]);
#endif
}

int iNemoEngineSensor::readEvents(sensors_event_t *data, int count)
{
	static int cont = 0;
	iNemoSensorsData sdata;
	struct timeval time;
	int64_t timeElapsed;
	float time_gyro_pollrate;
	int64_t New_Gyro_Delay_ms = GYR_DEFAULT_DELAY*1000000;
	int err;
	int numEventReceived = 0;
	input_event const* event;

	if (count < 1)
		return -EINVAL;

	if (mHasPendingEvent) {
		mHasPendingEvent = false;
		//return mEnabled ? 1 : 0;
		return 0; //for passing CTS,do not pass pendingevents.
	}

	ssize_t n = mInputReader.fill(data_fd);
	if (n < 0)
		return n;

#if FETCH_FULL_EVENT_BEFORE_RETURN
	again:
#endif

	while (count && mInputReader.readEvent(&event)) {
		if(event->type == EV_SYN) {

			if (startup_samples) {
				startup_samples--;
#if DEBUG_INEMO_SENSOR == 1
				STLOGD("iNemo::Start-up samples = %d", startup_samples);
#endif
				goto no_data;
			}

#if (SENSORS_GYROSCOPE_ENABLE == 1)
	iNemoEngineSensor::gyr->getGyroDelay(&New_Gyro_Delay_ms);
	//STLOGE("iNemo Gyro delay: %lld, %lld\n", Gyro_Delay_ms, New_Gyro_Delay_ms);
	if((New_Gyro_Delay_ms != Gyro_Delay_ms) &&
		mEnabled) {
		updateDecimations(New_Gyro_Delay_ms);
		Gyro_Delay_ms = New_Gyro_Delay_ms;
	}
#endif

#if (SENSORS_ACCELEROMETER_ENABLE == 1)
			AccelSensor::getBufferData(&mSensorsBufferedVectors[Acceleration]);
#endif
#if (SENSORS_GYROSCOPE_ENABLE == 1)
			GyroSensor::getBufferData(&mSensorsBufferedVectors[AngularSpeed]);
#endif
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
			MagnSensor::getBufferData(&mSensorsBufferedVectors[MagneticField]);
#endif

			if ((mEnabled & (1<<Orientation)) ||
				(mEnabled & (1<<Gravity)) ||
				(mEnabled & (1<<LinearAcceleration)) ||
				(mEnabled & (1<<GameRotation)) ||
				(mEnabled & (1<<UncalibGyro)) ||
				(mEnabled & (1<<RotationMatrix)) ||
				(mEnabled & (1<<CalibGyro)))
			{
				float data[5];
				sdata.accel[0] = mSensorsBufferedVectors[Acceleration].x;//m/s^2
				sdata.accel[1] = mSensorsBufferedVectors[Acceleration].y;//m/s^2
				sdata.accel[2] = mSensorsBufferedVectors[Acceleration].z;//m/s^2
				sdata.magn[0] = mSensorsBufferedVectors[MagneticField].x;//uT
				sdata.magn[1] = mSensorsBufferedVectors[MagneticField].y;//uT
				sdata.magn[2] = mSensorsBufferedVectors[MagneticField].z;//uT
				sdata.gyro[0] = mSensorsBufferedVectors[AngularSpeed].x;//rad/sec
				sdata.gyro[1] = mSensorsBufferedVectors[AngularSpeed].y;//rad/sec
				sdata.gyro[2] = mSensorsBufferedVectors[AngularSpeed].z;//rad/sec

#if DEBUG_INEMO_SENSOR == 1
				STLOGD("Acc_x=%f [m/s^2], Acc_y=%f [m/s^2], Acc_z=%f [m/s^2]", sdata.accel[0], sdata.accel[1], sdata.accel[2]);
				STLOGD("Mag_x=%f [uT], Mag_y=%f [uT], Mag_z=%f [uT]", sdata.magn[0], sdata.magn[1], sdata.magn[2]);
				STLOGD("Gyr_x=%f [rad/sec], Gyr_y=%f [rad/sec], Gyr_z=%f [rad/sec]", sdata.gyro[0], sdata.gyro[1], sdata.gyro[2]);
#endif
				time_gyro_pollrate = GYR_DEFAULT_DELAY/1000.0f;
				clock_gettime(CLOCK_MONOTONIC, &new_time);
				timeElapsed = timespecDiff(&new_time, &old_time);
				if (timeElapsed > 3*time_gyro_pollrate*1000000000)
					timeElapsed = time_gyro_pollrate*1000000000;

#if DEBUG_INEMO_SENSOR == 1
				STLOGD("acc: %f, %f, %f, Mag: %f, %f, %f, Gyro: %f, %f, %f, time: %lld\n",sdata.accel[0], sdata.accel[1], sdata.accel[2], sdata.magn[0], sdata.magn[1], sdata.magn[2], sdata.gyro[0],sdata.gyro[1], sdata.gyro[2], timeElapsed);
#endif
				iNemoEngine_API_Run(timeElapsed, &sdata);
				old_time = new_time;

				gettimeofday(&time, NULL);

#if (SENSORS_ORIENTATION_ENABLE == 1)
				Orientation_decimation_count++;
				if(mEnabled & (1<<Orientation) &&
				  (Orientation_decimation_count >= DecimationBuffer[Orientation])) {
					Orientation_decimation_count = 0;
					err = iNemoEngine_API_Get_Euler_Angles(data);
					if (err != 0) {
						goto no_data;
					}
					mPendingEvents[Orientation].orientation.status = mSensorsBufferedVectors[MagneticField].status;
					mPendingEvents[Orientation].data[0] = data[0];
					mPendingEvents[Orientation].data[1] = data[1];
					mPendingEvents[Orientation].data[2] = data[2];
					mPendingMask |= 1<<Orientation;

#if DEBUG_INEMO_SENSOR == 1
					if (!isnan(data[0])){
						STLOGD("time =  %lld, menabled = %d", timeElapsed, mEnabled);
						STLOGD("Acc_x=%f [m/s^2], Acc_y=%f [m/s^2], Acc_z=%f [m/s^2]", sdata.accel[0], sdata.accel[1], sdata.accel[2]);
						STLOGD("Mag_x=%f [uT], Mag_y=%f [uT], Mag_z=%f [uT]", sdata.magn[0], sdata.magn[1], sdata.magn[2]);
						STLOGD("Gyr_x=%f [rad/sec], Gyr_y=%f [rad/sec], Gyr_z=%f [rad/sec]", sdata.gyro[0], sdata.gyro[1], sdata.gyro[2]);
					}
#endif
				}
#endif
#if (SENSORS_GRAVITY_ENABLE == 1)
				Gravity_decimation_count++;
				if(mEnabled & (1<<Gravity) &&
				  (Gravity_decimation_count >= DecimationBuffer[Gravity])) {
					Gravity_decimation_count = 0;
					err = iNemoEngine_API_Get_Gravity(data);
					if (err != 0)
						goto no_data;

					mPendingEvents[Gravity].data[0] = data[0];
					mPendingEvents[Gravity].data[1] = data[1];
					mPendingEvents[Gravity].data[2] = data[2];
					mPendingMask |= 1<<Gravity;
				}
#endif
#if (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)
				LinearAcceleration_decimation_count++;
				if(mEnabled & (1<<LinearAcceleration) &&
				  (LinearAcceleration_decimation_count >= DecimationBuffer[LinearAcceleration])) {
					LinearAcceleration_decimation_count = 0;
					err = iNemoEngine_API_Get_Linear_Acceleration(data);
					if (err != 0)
						goto no_data;

					mPendingEvents[LinearAcceleration].data[0] = data[0];
					mPendingEvents[LinearAcceleration].data[1] = data[1];
					mPendingEvents[LinearAcceleration].data[2] = data[2];
					mPendingMask |= 1<<LinearAcceleration;
				}
#endif
#if (SENSORS_ROTATION_VECTOR_ENABLE == 1)
				RotationMatrix_decimation_count++;
				if(mEnabled & (1<<RotationMatrix) &&
				  (RotationMatrix_decimation_count >= DecimationBuffer[RotationMatrix])) {
					RotationMatrix_decimation_count = 0;
					err = iNemoEngine_API_Get_Quaternion(data);
					if (err != 0)
						goto no_data;

					mPendingEvents[RotationMatrix].data[0] = data[0];
					mPendingEvents[RotationMatrix].data[1] = data[1];
					mPendingEvents[RotationMatrix].data[2] = data[2];
					mPendingEvents[RotationMatrix].data[3] = data[3];
					mPendingEvents[RotationMatrix].data[4] = -1;
					mPendingMask |= 1<<RotationMatrix;
				}
#endif
#if (SENSORS_GAME_ROTATION_ENABLE == 1)
				GameRotation_decimation_count++;
				if(mEnabled & (1<<GameRotation) &&
				  (GameRotation_decimation_count >= DecimationBuffer[GameRotation])) {
					GameRotation_decimation_count = 0;
					err = iNemoEngine_API_Get_6X_Quaternion(data);
					if (err != 0)
						goto no_data;

					mPendingEvents[GameRotation].data[0] = data[0];
					mPendingEvents[GameRotation].data[1] = data[1];
					mPendingEvents[GameRotation].data[2] = data[2];
					mPendingEvents[GameRotation].data[3] = data[3];
					mPendingMask |= 1<<GameRotation;
				}
#endif
#if (GYROSCOPE_GBIAS_ESTIMATION_FUSION == 1)
#if (SENSORS_UNCALIB_GYROSCOPE_ENABLE == 1)
				UncalibGyro_decimation_count++;
				if(mEnabled & (1<<UncalibGyro) &&
				  (UncalibGyro_decimation_count >= DecimationBuffer[UncalibGyro])) {
					UncalibGyro_decimation_count = 0;
					err = iNemoEngine_API_Get_Gbias(data);
					if (err != 0)
						goto no_data;

					int i;
					for (i = 0 ; i < 3; i++) {
						mPendingEvents[UncalibGyro].uncalibrated_gyro.uncalib[i] = sdata.gyro[i];
						mPendingEvents[UncalibGyro].uncalibrated_gyro.bias[i] = data[i];
					}
					mPendingMask |= 1<<UncalibGyro;
				}
#endif
				CalibGyro_decimation_count++;
				if(mEnabled & (1<<CalibGyro) &&
				  (CalibGyro_decimation_count >= DecimationBuffer[CalibGyro])) {
					CalibGyro_decimation_count = 0;
					err = iNemoEngine_API_Get_Gbias(data);
					if (err != 0)
						goto no_data;

					int i;
					for (i = 0 ; i < 3; i++) {
						mPendingEvents[CalibGyro].data[i] = sdata.gyro[i] - data[i];
					}
					mPendingMask |= 1<<CalibGyro;
				}
#endif
			}

no_data:
			int64_t time = timevalToNano(event->time);
			for (int j=0 ; count && mPendingMask && j<numSensors ; j++) {
				if (mPendingMask & (1<<j)) {
					mPendingMask &= ~(1<<j);
					mPendingEvents[j].timestamp = time;
					if (mEnabled & (1<<j)) {
						*data++ = mPendingEvents[j];
						count--;
						numEventReceived++;
					}
				}
			}
		}
		mInputReader.next();
	}
#if FETCH_FULL_EVENT_BEFORE_RETURN
	/* if we didn't read a complete event, see if we can fill and
	try again instead of returning with nothing and redoing poll. */
	if (numEventReceived == 0 && mEnabled == 1) {
		n = mInputReader.fill(data_fd);
		if (n)
			goto again;
	}
#endif


	return numEventReceived;
}

#endif /* SENSOR_FUSION_ENABLE */
