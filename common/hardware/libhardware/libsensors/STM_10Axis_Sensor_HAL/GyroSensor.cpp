/*
 * Copyright (C) 2012 STMicroelectronics
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
#if (SENSORS_GYROSCOPE_ENABLE == 1)

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

#include "GyroSensor.h"
#include SENSOR_GYR_INCLUDE_FILE_NAME

#define FETCH_FULL_EVENT_BEFORE_RETURN		0

/*****************************************************************************/

sensors_vec_t  GyroSensor::dataBuffer;
int GyroSensor::mEnabled = 0;
int64_t GyroSensor::delayms = 0;
int GyroSensor::startup_samples = DEFAULT_SAMPLES_TO_DISCARD;
int GyroSensor::current_fullscale = 0;
int GyroSensor::samples_to_discard = DEFAULT_SAMPLES_TO_DISCARD;
float GyroSensor::gbias_out[3] = {0};
int64_t GyroSensor::setDelayBuffer[numSensors] = {0};
int GyroSensor::DecimationBuffer[numSensors] = {0};
int GyroSensor::Gyro_decimation_count = 0;
int GyroSensor::GyroUncalib_decimation_count = 0;
pthread_mutex_t GyroSensor::dataMutex;
#if (SENSORS_ACCELEROMETER_ENABLE == 1) && (GYROSCOPE_GBIAS_ESTIMATION_STANDALONE == 1)
AccelSensor* GyroSensor::acc = NULL;
#endif

GyroSensor::GyroSensor()
	: SensorBase(NULL, SENSOR_DATANAME_GYROSCOPE),
	mInputReader(4),
	mHasPendingEvent(false)
{
	pthread_mutex_init(&dataMutex, NULL);

#if (GYROSCOPE_GBIAS_ESTIMATION_FUSION == 0)
	mPendingEvent[Gyro].version = sizeof(sensors_event_t);
	mPendingEvent[Gyro].sensor = ID_GYROSCOPE;
	mPendingEvent[Gyro].type = SENSOR_TYPE_GYROSCOPE;
	mPendingEvent[Gyro].gyro.status = SENSOR_STATUS_ACCURACY_HIGH;

#if ((SENSORS_UNCALIB_GYROSCOPE_ENABLE == 1) && (GYROSCOPE_GBIAS_ESTIMATION_STANDALONE == 1))
	mPendingEvent[GyroUncalib].version = sizeof(sensors_event_t);
	mPendingEvent[GyroUncalib].sensor = ID_UNCALIB_GYROSCOPE;
	mPendingEvent[GyroUncalib].type = SENSOR_TYPE_GYROSCOPE_UNCALIBRATED;
	mPendingEvent[GyroUncalib].gyro.status = SENSOR_STATUS_ACCURACY_HIGH;
#endif
#endif

	if (data_fd) {
		STLOGI("GyroSensor::GyroSensor gyro_device_sysfs_path:(%s)", sysfs_device_path);
	} else {
		STLOGE("GyroSensor::GyroSensor gyro_device_sysfs_path:(%s) not found", sysfs_device_path);
	}

	data_raw[0] = data_raw[1] = data_raw[2] = 0.0;

#if GYRO_CALIBRATION_ENABLE_FILE
	pStoreCalibration = StoreCalibration::getInstance();
#endif
#if (GYROSCOPE_GBIAS_ESTIMATION_STANDALONE == 1)
	iNemoEngine_API_gbias_Initialization(NULL);
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
	acc = new AccelSensor();
#endif
#endif
}

GyroSensor::~GyroSensor()
{
	if (mEnabled) {
		enable(SENSORS_GYROSCOPE_HANDLE, 0, 0);
	}
#if ((SENSORS_ACCELEROMETER_ENABLE == 1) && (GYROSCOPE_GBIAS_ESTIMATION_STANDALONE == 1))
	acc->~AccelSensor();
#endif
	pthread_mutex_destroy(&dataMutex);
}

int GyroSensor::setInitialState()
{
	struct input_absinfo absinfo_x;
	struct input_absinfo absinfo_y;
	struct input_absinfo absinfo_z;
	int clockid = CLOCK_BOOTTIME;

	if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_GYRO_X), &absinfo_x) &&
		!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_GYRO_Y), &absinfo_y) &&
		!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_GYRO_Z), &absinfo_z))
	{
		mHasPendingEvent = true;	static int DecimationBuffer[numSensors];
	}

	setFullScale(SENSORS_GYROSCOPE_HANDLE, GYRO_DEFAULT_FULLSCALE);
	
	if (!ioctl(data_fd, EVIOCSCLOCKID, &clockid))
		ALOGE("GyroSensor : set EVIOCSCLOCKID = %d\n",clockid);
	else
		ALOGE("GyroSensor : set EVIOCSCLOCKID failed ! \n");

	startup_samples = samples_to_discard;
	Gyro_decimation_count = 0;
	GyroUncalib_decimation_count = 0;

	return 0;
}

int GyroSensor::enable(int32_t handle, int en, int type)
{
	int err = 0;
	int flags = en ? 1 : 0;
	int what = -1;
	int mEnabledPrev;

	switch(handle) {
		case SENSORS_GYROSCOPE_HANDLE:
			what = Gyro;
			break;
#if ((SENSORS_UNCALIB_GYROSCOPE_ENABLE == 1) && (GYROSCOPE_GBIAS_ESTIMATION_STANDALONE == 1))
		case SENSORS_UNCALIB_GYROSCOPE_HANDLE:
			what = GyroUncalib;
			break;
#endif
#if SENSOR_FUSION_ENABLE == 1
		case SENSORS_SENSOR_FUSION_HANDLE:
			what = iNemoGyro;
			break;
#endif
		default:
			return -1;
	}

	if (flags) {

		if (!mEnabled) {
			setInitialState();
			err = writeEnable(SENSORS_GYROSCOPE_HANDLE, flags);

			if(err >= 0) {
				err = 0;
			}
		}

		mEnabled |= (1<<what);

	} else {
		mEnabledPrev = mEnabled;
		mEnabled &= ~(1<<what);

		if ((!mEnabled) & (mEnabledPrev)){
			err = writeEnable(SENSORS_GYROSCOPE_HANDLE, flags);
			if(err >= 0) {
				err = 0;
			}
		}
	}

#if (GYROSCOPE_GBIAS_ESTIMATION_STANDALONE == 1)
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
	acc->enable(SENSORS_MAGNETIC_FIELD_HANDLE, flags, 1);
#endif
	iNemoEngine_API_gbias_enable(flags);
#endif

	if(err >= 0 ) {
		STLOGD("GyroSensor::enable(%d), handle: %d, what: %d, mEnabled: %x",flags, handle, what, mEnabled);
	} else {
		STLOGE("GyroSensor::enable(%d), handle: %d, what: %d, mEnabled: %x",flags, handle, what, mEnabled);
	}

	return err;
}

bool GyroSensor::hasPendingEvents() const
{
	return mHasPendingEvent;
}

int GyroSensor::setDelay(int32_t handle, int64_t delay_ns)
{
	int what = -1;
	int kk;
	int err = 0;
	int64_t delay_ms = delay_ns/1000000;
	int64_t Min_delay_ms = 0;

	if(delay_ms == 0)
		return err;

	switch(handle) {
		case SENSORS_GYROSCOPE_HANDLE:
			what = Gyro;
			break;
#if ((SENSORS_UNCALIB_GYROSCOPE_ENABLE == 1) && (GYROSCOPE_GBIAS_ESTIMATION_STANDALONE == 1))
		case SENSORS_UNCALIB_GYROSCOPE_HANDLE:
			what = GyroUncalib;
			break;
#endif
#if SENSOR_FUSION_ENABLE == 1
		case SENSORS_SENSOR_FUSION_HANDLE:
			what = iNemoGyro;
			break;
#endif
		default:
			return -1;
	}

	// Min setDelay Definition
	setDelayBuffer[what] = delay_ms;
	for(kk = 0; kk < numSensors; kk++)
	{
		if ((mEnabled & 1<<kk) != 0)
		{
			if (!Min_delay_ms || (setDelayBuffer[kk] <= Min_delay_ms))
				Min_delay_ms = setDelayBuffer[kk];
		}
		else
			setDelayBuffer[kk] = 0;
	}

	// Min setDelay Writing
	if (mEnabled && (Min_delay_ms != delayms))
	{
		samples_to_discard = (int)(GYRO_STARTUP_TIME_MS/Min_delay_ms)+1;
		startup_samples = samples_to_discard;
		err = writeDelay(SENSORS_GYROSCOPE_HANDLE, Min_delay_ms);

		if(err >= 0) {
			err = 0;
			delayms = Min_delay_ms;
			Gyro_decimation_count = 0;
			GyroUncalib_decimation_count = 0;

#if (GYROSCOPE_GBIAS_ESTIMATION_STANDALONE == 1)
			iNemoEngine_API_gbias_set_frequency(1000.0f /
							(float)Min_delay_ms);
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
			acc->setDelay(SENSORS_UNCALIB_GYROSCOPE_HANDLE,
						1000.0f / (float)Min_delay_ms);
#endif
#endif

		}
	}

	// Decimation Definition
	for(kk = 0; kk < numSensors; kk++)
	{
		if ((mEnabled & 1<<kk) != 0)
			DecimationBuffer[kk] = setDelayBuffer[kk]/delayms;
		else
			DecimationBuffer[kk] = 0;
	}

#if DEBUG_GYROSCOPE == 1
	STLOGD("GyroSensor::setDelayBuffer[] = %lld, %lld, %lld", setDelayBuffer[0], setDelayBuffer[1], setDelayBuffer[2]);
	STLOGD("GyroSensor::Min_delay_ms = %lld, delayms = %lld, mEnabled = %d", Min_delay_ms, delayms, mEnabled);
	STLOGD("GyroSensor::samples_to_discard = %d", samples_to_discard);
	STLOGD("GyroSensor::DecimationBuffer = %d, %d, %d", DecimationBuffer[0], DecimationBuffer[1], DecimationBuffer[2]);
#endif

	return err;
}

void GyroSensor::getGyroDelay(int64_t *Gyro_Delay_ms)
{
	*Gyro_Delay_ms = delayms;

	return;
}


int GyroSensor::setFullScale(int32_t handle, int value)
{
	int err = -1;

	if(value <= 0)
		return err;
	else
		err = 0;

	if(value != current_fullscale)
	{
		err = writeFullScale(SENSORS_GYROSCOPE_HANDLE, value);

		if(err >= 0) {
			err = 0;
			current_fullscale = value;
		}
	}
	return err;
}

int GyroSensor::readEvents(sensors_event_t* data, int count)
{
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

	int numEventReceived = 0;
	input_event const* event;

#if FETCH_FULL_EVENT_BEFORE_RETURN
	again:
#endif

	while (count && mInputReader.readEvent(&event)) {

		if (event->type == EV_ABS) {

#if DEBUG_GYROSCOPE == 1
	STLOGD("GyroSensor::readEvents (event_code=%d)", event->code);
#endif

			float value = (float) event->value;
			if (event->code == EVENT_TYPE_GYRO_X) {
				data_raw[0] = value * CONVERT_GYRO_X;
			}
			else if (event->code == EVENT_TYPE_GYRO_Y) {
				data_raw[1] = value * CONVERT_GYRO_Y;
			}
			else if (event->code == EVENT_TYPE_GYRO_Z) {
				data_raw[2] = value * CONVERT_GYRO_Z;
			} else {
				STLOGE("GyroSensor: unknown event code (type = %d, code = %d)", event->type, event->code);
			}
		} else if (event->type == EV_SYN) {

			if (startup_samples) {
				startup_samples--;

#if DEBUG_GYROSCOPE == 1
				STLOGD("GyroSensor::Start-up samples = %d", startup_samples);
#endif
				goto no_data;
			}

			data_rot[0] = data_raw[0]*matrix_gyr[0][0] + data_raw[1]*matrix_gyr[1][0] + data_raw[2]*matrix_gyr[2][0];
			data_rot[1] = data_raw[0]*matrix_gyr[0][1] + data_raw[1]*matrix_gyr[1][1] + data_raw[2]*matrix_gyr[2][1];
			data_rot[2] = data_raw[0]*matrix_gyr[0][2] + data_raw[1]*matrix_gyr[1][2] + data_raw[2]*matrix_gyr[2][2];

#if GYRO_CALIBRATION_ENABLE_FILE
			data_rot[0] -= pStoreCalibration->getCalibration(StoreCalibration::GYROSCOPE, 0);
			data_rot[1] -= pStoreCalibration->getCalibration(StoreCalibration::GYROSCOPE, 1);
			data_rot[2] -= pStoreCalibration->getCalibration(StoreCalibration::GYROSCOPE, 2);
#endif

#if (GYROSCOPE_GBIAS_ESTIMATION_FUSION == 0)
			gbias_out[0] = gbias_out[1] = gbias_out[2] = 0;
#if (GYROSCOPE_GBIAS_ESTIMATION_STANDALONE == 1)
			int bias_meas;
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
			sensors_vec_t tmp_data_acc;
			AccelSensor::getBufferData(&tmp_data_acc);
			data_acc[0] = tmp_data_acc.x;
			data_acc[1] = tmp_data_acc.y;
			data_acc[2] = tmp_data_acc.z;
#else
			data_acc[0] = data_acc[1] = data_acc[2] = 0.0;
#endif
			iNemoEngine_API_set_accel_data(data_acc);
			iNemoEngine_API_gbias_Run(data_rot);
			iNemoEngine_API_Get_gbias(gbias_out);
#endif
			Gyro_decimation_count++;

			if(mEnabled & (1<<Gyro) &&
			  (Gyro_decimation_count >= DecimationBuffer[Gyro])) {
				Gyro_decimation_count = 0;
				mPendingEvent[Gyro].data[0] = data_rot[0] - gbias_out[0];
				mPendingEvent[Gyro].data[1] = data_rot[1] - gbias_out[1];
				mPendingEvent[Gyro].data[2] = data_rot[2] - gbias_out[2];
				mPendingEvent[Gyro].timestamp = timevalToNano(event->time);
				mPendingEvent[Gyro].gyro.status = SENSOR_STATUS_ACCURACY_HIGH;

				*data++ = mPendingEvent[Gyro];
				count--;
				numEventReceived++;
			}

#if ((SENSORS_UNCALIB_GYROSCOPE_ENABLE == 1) && (GYROSCOPE_GBIAS_ESTIMATION_STANDALONE == 1))
			GyroUncalib_decimation_count++;

			if(mEnabled & (1<<GyroUncalib) &&
			  (GyroUncalib_decimation_count >= DecimationBuffer[GyroUncalib])) {
				GyroUncalib_decimation_count = 0;
				mPendingEvent[GyroUncalib].uncalibrated_gyro.uncalib[0] = data_rot[0];
				mPendingEvent[GyroUncalib].uncalibrated_gyro.uncalib[1] = data_rot[1];
				mPendingEvent[GyroUncalib].uncalibrated_gyro.uncalib[2] = data_rot[2];
				mPendingEvent[GyroUncalib].uncalibrated_gyro.bias[0] = gbias_out[0];
				mPendingEvent[GyroUncalib].uncalibrated_gyro.bias[1] = gbias_out[1];
				mPendingEvent[GyroUncalib].uncalibrated_gyro.bias[2] = gbias_out[2];
				mPendingEvent[GyroUncalib].timestamp = timevalToNano(event->time);
				mPendingEvent[GyroUncalib].gyro.status = SENSOR_STATUS_ACCURACY_HIGH;

				*data++ = mPendingEvent[GyroUncalib];
				count--;
				numEventReceived++;
			}
#endif
#endif

			if(mEnabled & (1<<iNemoGyro)) {
				sensors_vec_t sData;
				sData.x = data_rot[0] - gbias_out[0];
				sData.y = data_rot[1] - gbias_out[1];
				sData.z = data_rot[2] - gbias_out[2];
//				STLOGE("Store gyro: %f, %f, %f\n",sData.x, sData.y, sData.z);
				setBufferData(&sData);
			}

#if DEBUG_GYROSCOPE == 1
			STLOGD("GyroSensor::readEvents (time = %lld), count(%d), received(%d)", mPendingEvent[Gyro].timestamp, count, numEventReceived);
#endif

		} else {
			STLOGE("GyroSensor: unknown event (type=%d, code=%d)", event->type, event->code);
		}
no_data:
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

bool GyroSensor::setBufferData(sensors_vec_t *value)
{
	pthread_mutex_lock(&dataMutex);
	dataBuffer.x = value->x;
	dataBuffer.y = value->y;
	dataBuffer.z = value->z;
	pthread_mutex_unlock(&dataMutex);

	return true;
}

bool GyroSensor::getBufferData(sensors_vec_t *lastBufferedValues)
{
	pthread_mutex_lock(&dataMutex);
	lastBufferedValues->x = dataBuffer.x;
	lastBufferedValues->y = dataBuffer.y;
	lastBufferedValues->z = dataBuffer.z;
	pthread_mutex_unlock(&dataMutex);

#if DEBUG_GYROSCOPE == 1
	STLOGD("GyroSensor: getBufferData got values: x:(%f),y:(%f), z:(%f).", lastBufferedValues->x, lastBufferedValues->y, lastBufferedValues->z);
#endif

	return true;
}

#endif /* SENSORS_GYROSCOPE_ENABLE */
