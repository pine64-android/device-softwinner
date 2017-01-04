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
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)

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

#include "MagnSensor.h"
#include SENSOR_MAG_INCLUDE_FILE_NAME

#if ((SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1) || ((SENSOR_GEOMAG_ENABLE == 1) && (SENSOR_FUSION_ENABLE == 0)))
#include "iNemoEngineGeoMagAPI.h"
#endif

#define FETCH_FULL_EVENT_BEFORE_RETURN		0
#define MS2_TO_MG(x)				(x*102.040816327f)
#define UT_TO_MGAUSS(x)				(x*10.0f)
#define MGAUSS_TO_UT(x)				(x/10.0f)

/*****************************************************************************/

sensors_vec_t  MagnSensor::dataBuffer;
int MagnSensor::freq = 0;
int MagnSensor::count_call_calibration = 0;
int MagnSensor::mEnabled = 0;
int64_t MagnSensor::delayms = 0;
int MagnSensor::current_fullscale = 0;
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
AccelSensor* MagnSensor::acc = NULL;
#endif
static int calibration_running;
int8_t MagnSensor::status = SENSOR_STATUS_ACCURACY_HIGH;
int64_t MagnSensor::setDelayBuffer[numSensors] = {0};
int MagnSensor::DecimationBuffer[numSensors] = {0};
int MagnSensor::Mag_decimation_count = 0;
int MagnSensor::MagUncalib_decimation_count = 0;
int MagnSensor::GeoMagnRotVect_decimation_count = 0;
int MagnSensor::Orientation_decimation_count = 0;
int MagnSensor::Gravity_Accel_decimation_count = 0;
int MagnSensor::Linear_Accel_decimation_count = 0;
pthread_mutex_t MagnSensor::dataMutex;

MagnSensor::MagnSensor()
	: SensorBase(NULL, SENSOR_DATANAME_MAGNETIC_FIELD),
	mInputReader(4),
	mHasPendingEvent(false)
{
	int i, err;

	pthread_mutex_init(&dataMutex, NULL);

	memset(mPendingEvent, 0, sizeof(mPendingEvent));

	mPendingEvent[MagneticField].version = sizeof(sensors_event_t);
	mPendingEvent[MagneticField].sensor = ID_MAGNETIC_FIELD;
	mPendingEvent[MagneticField].type = SENSOR_TYPE_MAGNETIC_FIELD;
	memset(mPendingEvent[MagneticField].data, 0, sizeof(mPendingEvent[MagneticField].data));
	mPendingEvent[MagneticField].magnetic.status = SENSOR_STATUS_UNRELIABLE;

#if (SENSORS_UNCALIB_MAGNETIC_FIELD_ENABLE == 1)
	mPendingEvent[UncalibMagneticField].version = sizeof(sensors_event_t);
	mPendingEvent[UncalibMagneticField].sensor = ID_UNCALIB_MAGNETIC_FIELD;
	mPendingEvent[UncalibMagneticField].type = SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED;
	memset(mPendingEvent[UncalibMagneticField].data, 0, sizeof(mPendingEvent[UncalibMagneticField].data));
	mPendingEvent[UncalibMagneticField].magnetic.status = SENSOR_STATUS_UNRELIABLE;
#endif

#if ((SENSOR_FUSION_ENABLE == 0) && (MAG_CALIBRATION_ENABLE == 1))
#if (SENSORS_ORIENTATION_ENABLE == 1)
	mPendingEvent[Orientation].version = sizeof(sensors_event_t);
	mPendingEvent[Orientation].sensor = ID_ORIENTATION;
	mPendingEvent[Orientation].type = SENSOR_TYPE_ORIENTATION;
	memset(mPendingEvent[Orientation].data, 0, sizeof(mPendingEvent[Orientation].data));
	mPendingEvent[Orientation].orientation.status = SENSOR_STATUS_UNRELIABLE;
#endif
#if (SENSORS_GRAVITY_ENABLE == 1)
	mPendingEvent[Gravity_Accel].version = sizeof(sensors_event_t);
	mPendingEvent[Gravity_Accel].sensor = ID_GRAVITY;
	mPendingEvent[Gravity_Accel].type = SENSOR_TYPE_GRAVITY;
	memset(mPendingEvent[Gravity_Accel].data, 0, sizeof(mPendingEvent[Gravity_Accel].data));
	mPendingEvent[Gravity_Accel].acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;
#endif
#if (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)
	mPendingEvent[Linear_Accel].version = sizeof(sensors_event_t);
	mPendingEvent[Linear_Accel].sensor = ID_LINEAR_ACCELERATION;
	mPendingEvent[Linear_Accel].type = SENSOR_TYPE_LINEAR_ACCELERATION;
	memset(mPendingEvent[Linear_Accel].data, 0, sizeof(mPendingEvent[Linear_Accel].data));
	mPendingEvent[Linear_Accel].acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;
#endif
#endif

#if (SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1)
	mPendingEvent[GeoMagRotVect_Magnetic].version = sizeof(sensors_event_t);
	mPendingEvent[GeoMagRotVect_Magnetic].sensor = ID_GEOMAG_ROTATION_VECTOR;
	mPendingEvent[GeoMagRotVect_Magnetic].type = SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR;
	memset(mPendingEvent[GeoMagRotVect_Magnetic].data, 0, sizeof(mPendingEvent[GeoMagRotVect_Magnetic].data));
	mPendingEvent[GeoMagRotVect_Magnetic].magnetic.status = SENSOR_STATUS_UNRELIABLE;

	sData.accel[0] = sData.accel[1] = sData.accel[2] = 0.0f;
	sData.magn[0] = sData.magn[1] = sData.magn[2] = 0.0f;
	iNemoEngine_GeoMag_API_Initialization();

#endif

	if (data_fd) {
		STLOGI("MagnSensor::MagnSensor magn_device_sysfs_path:(%s)", sysfs_device_path);
	} else {
		STLOGE("MagnSensor::MagnSensor magn_device_sysfs_path:(%s) not found", sysfs_device_path);
	}

	for(i=0; i<3; i++) {
		data_raw[i] = 0;
	}

#if (((MAG_CALIBRATION_ENABLE == 1)&&(SENSORS_ACCELEROMETER_ENABLE == 1)) || (SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1))
	acc = new AccelSensor();
#endif

#if (MAG_CALIBRATION_ENABLE_FILE == 1)
	pStoreCalibration = StoreCalibration::getInstance();
#endif

#if (MAG_SI_COMPENSATION_ENABLED == 1)
	if (compass_API_loadSIMatrixFromFile(DEFAULT_SI_MATRIX_FILEPATH) != 0)
		ALOGE("MagnSensor: error while loading SI file %s\n",
						DEFAULT_SI_MATRIX_FILEPATH);
#endif
}

MagnSensor::~MagnSensor() {
	if (mEnabled) {
		enable(SENSORS_MAGNETIC_FIELD_HANDLE, 0, 0);
		mEnabled = 0;
	}
	pthread_mutex_destroy(&dataMutex);
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
	acc->~AccelSensor();
#endif
}

int MagnSensor::setInitialState()
{
	struct input_absinfo absinfo_x;
	struct input_absinfo absinfo_y;
	struct input_absinfo absinfo_z;
	float value;
	int clockid = CLOCK_BOOTTIME;

#if (MAG_CALIBRATION_ENABLE == 1) && (SENSORS_ACCELEROMETER_ENABLE == 1)
	data_read = 0;
	compass_API_Init(MAG_DEFAULT_RANGE, 0, DEFAULT_CALIB_DATA_FILE);
#endif

	if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_MAG_X), &absinfo_x) &&
		!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_MAG_Y), &absinfo_y) &&
		!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_MAG_Z), &absinfo_z))
	{
		mHasPendingEvent = true;
	}

	setFullScale(SENSORS_MAGNETIC_FIELD_HANDLE, MAGN_DEFAULT_FULLSCALE);
	
	if (!ioctl(data_fd, EVIOCSCLOCKID, &clockid))
		ALOGE("MagnSensor : set EVIOCSCLOCKID = %d\n",clockid);
	else
		ALOGE("MagnSensor : set EVIOCSCLOCKID failed ! \n");

	Mag_decimation_count = 0;
	MagUncalib_decimation_count = 0;
	Orientation_decimation_count = 0;
	GeoMagnRotVect_decimation_count = 0;
	Linear_Accel_decimation_count = 0;
	Gravity_Accel_decimation_count = 0;

	return 0;
}

int MagnSensor::enable(int32_t handle, int en, int type)
{
	int err = 0;
	int flags = en ? 1 : 0;
	int what = -1;

	switch(handle) {

		case SENSORS_MAGNETIC_FIELD_HANDLE:
			what = MagneticField;
			break;

#if (SENSORS_UNCALIB_MAGNETIC_FIELD_ENABLE == 1)
		case SENSORS_UNCALIB_MAGNETIC_FIELD_HANDLE:
			what = UncalibMagneticField;
			break;
#endif

#if (SENSOR_FUSION_ENABLE == 1)
		case SENSORS_SENSOR_FUSION_HANDLE:
			what = iNemoMagnetic;
			break;
#endif

#if (SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1)
		case SENSORS_GEOMAG_ROTATION_VECTOR_HANDLE:
			what = GeoMagRotVect_Magnetic;
			break;
#endif
#if ((SENSOR_FUSION_ENABLE == 0) && (MAG_CALIBRATION_ENABLE == 1))
#if (SENSORS_ORIENTATION_ENABLE == 1)
		case SENSORS_ORIENTATION_HANDLE:
			what = Orientation;
			break;
#endif
#if (SENSORS_GRAVITY_ENABLE == 1)
		case SENSORS_GRAVITY_HANDLE:
			what = Gravity_Accel;
			break;
#endif
#if (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)
		case SENSORS_LINEAR_ACCELERATION_HANDLE:
			what = Linear_Accel;
			break;
#endif
#endif
#if (SENSORS_VIRTUAL_GYROSCOPE_ENABLE == 1)
		case SENSORS_VIRTUAL_GYROSCOPE_HANDLE:
			what = VirtualGyro;
			break;
#endif
		default:
			return -1;
	}

	if (flags) {
		if (!mEnabled) {
			setInitialState();
			err = writeEnable(SENSORS_MAGNETIC_FIELD_HANDLE, flags);
			if(err >= 0) {
				err = 0;
			}
		}

#if ((MAG_CALIBRATION_ENABLE == 1) && (SENSORS_ACCELEROMETER_ENABLE == 1))
		acc->enable(SENSORS_MAGNETIC_FIELD_HANDLE, flags, 2);	// Axl enable
				//acc->setDelay(SENSORS_MAGNETIC_FIELD_HANDLE, (1000000000/CALIBRATION_FREQUENCY));
#endif
#if (SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1)
		if (what == GeoMagRotVect_Magnetic)
				acc->enable(SENSORS_GEOMAG_ROTATION_VECTOR_HANDLE, flags, 3);	// Axl enable
#endif
#if (SENSOR_FUSION_ENABLE == 0)
#if (SENSORS_ORIENTATION_ENABLE == 1)
		if (what == Orientation)
				acc->enable(SENSORS_ORIENTATION_HANDLE, flags, 4);	// Axl enable
#endif
#if (SENSORS_GRAVITY_ENABLE == 1)
		if (what == Gravity_Accel)
				acc->enable(SENSORS_GRAVITY_HANDLE, flags, 5);	// Axl enable
#endif
#if (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)
		if (what == Linear_Accel)
				acc->enable(SENSORS_LINEAR_ACCELERATION_HANDLE, flags, 6);	// Axl enable
#endif
#endif
		mEnabled |= (1<<what);

	} else {

		mEnabled &= ~(1<<what);

		if (!mEnabled) {
			err = writeEnable(SENSORS_MAGNETIC_FIELD_HANDLE, flags);
			if(err >= 0) {
				err = 0;
			}
		}
#if ((MAG_CALIBRATION_ENABLE == 1) && (SENSORS_ACCELEROMETER_ENABLE == 1))
			acc->enable(SENSORS_MAGNETIC_FIELD_HANDLE, flags, 2);	// Axl disable
#endif
#if (SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1)
				if (what == GeoMagRotVect_Magnetic)
					acc->enable(SENSORS_GEOMAG_ROTATION_VECTOR_HANDLE, flags, 3);	// Axl enable
#endif
#if (SENSOR_FUSION_ENABLE == 0)
#if (SENSORS_ORIENTATION_ENABLE == 1)
				if (what == Orientation)
					acc->enable(SENSORS_ORIENTATION_HANDLE, flags, 4);	// Axl enable
#endif
#if (SENSORS_GRAVITY_ENABLE == 1)
				if (what == Gravity_Accel)
					acc->enable(SENSORS_GRAVITY_HANDLE, flags, 5);	// Axl enable
#endif
#if (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)
				if (what == Linear_Accel)
					acc->enable(SENSORS_ORIENTATION_HANDLE, flags, 6);	// Axl enable
#endif
#endif
		setDelay(handle,200000000);
	}

	if(err >= 0 ) {
		STLOGD("MagSensor::enable(%d), handle: %d, what: %d, mEnabled: %x",flags, handle, what, mEnabled);
	} else {
		STLOGE("MagSensor::enable(%d), handle: %d, what: %d, mEnabled: %x",flags, handle, what, mEnabled);
	}

	return err;
}

bool MagnSensor::hasPendingEvents() const
{
	return mHasPendingEvent;
}

int MagnSensor::setDelay(int32_t handle, int64_t delay_ns)
{
	int err = 0;
	int kk;
	int what = -1;
	int64_t delay_ms = delay_ns/1000000;
	int64_t Min_delay_ms = 0;

	if(delay_ms == 0)
		return err;

	switch(handle) {

		case SENSORS_MAGNETIC_FIELD_HANDLE:
			what = MagneticField;
			break;

#if (SENSORS_UNCALIB_MAGNETIC_FIELD_ENABLE == 1)
		case SENSORS_UNCALIB_MAGNETIC_FIELD_HANDLE:
			what = UncalibMagneticField;
			break;
#endif

#if (SENSOR_FUSION_ENABLE == 1)
		case SENSORS_SENSOR_FUSION_HANDLE:
			what = iNemoMagnetic;
			break;
#endif

#if (SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1)
		case SENSORS_GEOMAG_ROTATION_VECTOR_HANDLE:
			what = GeoMagRotVect_Magnetic;
			break;
#endif

#if ((SENSOR_FUSION_ENABLE == 0) && (MAG_CALIBRATION_ENABLE == 1))
#if (SENSORS_ORIENTATION_ENABLE == 1)
		case SENSORS_ORIENTATION_HANDLE:
			what = Orientation;
			break;
#endif
#if (SENSORS_GRAVITY_ENABLE == 1)
		case SENSORS_GRAVITY_HANDLE:
			what = Gravity_Accel;
			break;
#endif
#if (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)
		case SENSORS_LINEAR_ACCELERATION_HANDLE:
			what = Linear_Accel;
			break;
#endif
#endif
#if (SENSORS_VIRTUAL_GYROSCOPE_ENABLE == 1)
		case SENSORS_VIRTUAL_GYROSCOPE_HANDLE:
			what = VirtualGyro;
			break;
#endif
		default:
			return -1;
	}

#if ((MAG_CALIBRATION_ENABLE == 1) && (SENSORS_ACCELEROMETER_ENABLE == 1))
				acc->setDelay(SENSORS_MAGNETIC_FIELD_HANDLE, (1000000000/CALIBRATION_FREQUENCY));
#endif
#if (SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1)
				if (what == GeoMagRotVect_Magnetic)
					acc->setDelay(SENSORS_GEOMAG_ROTATION_VECTOR_HANDLE, 1000000000/GEOMAG_FREQUENCY);	// Axl enable
#endif
#if ((SENSOR_FUSION_ENABLE == 0) && (MAG_CALIBRATION_ENABLE == 1))
#if (SENSORS_ORIENTATION_ENABLE == 1)
				if (what == Orientation)
					acc->setDelay(SENSORS_ORIENTATION_HANDLE, 1000000000/GEOMAG_FREQUENCY);	// Axl enable
#endif
#if (SENSORS_GRAVITY_ENABLE == 1)
				if (what == Gravity_Accel)
					acc->setDelay(SENSORS_GRAVITY_HANDLE, 1000000000/GEOMAG_FREQUENCY);	// Axl enable
#endif
#if (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)
				if (what == Linear_Accel)
					acc->setDelay(SENSORS_LINEAR_ACCELERATION_HANDLE, 1000000000/GEOMAG_FREQUENCY);	// Axl enable
#endif
#endif

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

#if ((MAG_CALIBRATION_ENABLE == 1) && (SENSORS_ACCELEROMETER_ENABLE == 1))
	if(Min_delay_ms > (1000/CALIBRATION_FREQUENCY))
		Min_delay_ms = 1000/CALIBRATION_FREQUENCY;
#endif
#if ((SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1) | \
	(((SENSORS_ORIENTATION_ENABLE == 1) | (SENSORS_LINEAR_ACCELERATION_ENABLE == 1) | (SENSORS_GRAVITY_ENABLE == 1))  && \
			(SENSOR_FUSION_ENABLE == 0)))
	if ((what == GeoMagRotVect_Magnetic) || (what == Orientation)
			 || (what == Linear_Accel) || (what == Gravity_Accel)){
		if(Min_delay_ms > (1000/GEOMAG_FREQUENCY))
			Min_delay_ms = 1000/GEOMAG_FREQUENCY;
	}
#endif
	if (mEnabled && (Min_delay_ms != delayms))
	{
		err = writeDelay(SENSORS_MAGNETIC_FIELD_HANDLE, Min_delay_ms);

		if(err >= 0) {
			err = 0;
			delayms = Min_delay_ms;
			freq = 1000/Min_delay_ms;
			count_call_calibration = freq/CALIBRATION_FREQUENCY;

			Mag_decimation_count = 0;
			MagUncalib_decimation_count = 0;
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

#if DEBUG_MAGNETOMETER == 1
	STLOGD("MagSensor::setDelayBuffer[] = %lld, %lld, %lld, %lld, %lld, %lld, %lld", setDelayBuffer[0], setDelayBuffer[1], setDelayBuffer[2], setDelayBuffer[3], setDelayBuffer[4], setDelayBuffer[5], setDelayBuffer[6]);
	STLOGD("MagSensor::Min_delay_ms = %lld, delayms = %lld, mEnabled = %d", Min_delay_ms, delayms, mEnabled);
	STLOGD("MagSensor::DecimationBuffer = %d, %d, %d, %d, %d, %d, %d", DecimationBuffer[0], DecimationBuffer[1], DecimationBuffer[2], DecimationBuffer[3], DecimationBuffer[4], DecimationBuffer[5], DecimationBuffer[6]);
#endif

	return err;
}

int MagnSensor::setFullScale(int32_t handle, int value)
{
	int err = -1;

	if(value <= 0)
		return err;
	else
		err = 0;

	if(value != current_fullscale)
	{
		err = writeFullScale(SENSORS_MAGNETIC_FIELD_HANDLE, value);

		if(err >= 0) {
			err = 0;
			current_fullscale = value;
		}
	}
	return err;
}

int MagnSensor::readEvents(sensors_event_t *data, int count)
{
	static struct timespec old_time, new_time;
	int64_t timeElapsed;
	struct timeval time;
	float quaternion[4];
	float Hpr[3] = {0.0f, 0.0f, 0.0f};
	float LinAcc[3] = {0.0f, 0.0f, 0.0f};
	float GravAcc[3] = {0.0f, 0.0f, 0.0f};
	int err;
	static float MagOffset[3] = {0};

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
			float value = (float) event->value;
#if (DEBUG_CALIBRATION == 1) && (MAG_CALIBRATION_ENABLE == 1) && (SENSORS_ACCELEROMETER_ENABLE == 1)
			STLOGD("Calibration Data-> OffX: %f, OffY: %f, OffZ: %f", cf.magOffX, cf.magOffY, cf.magOffZ);
#endif

			if (event->code == EVENT_TYPE_MAG_X) {
				data_raw[0] = value * CONVERT_M_X;
			} else if (event->code == EVENT_TYPE_MAG_Y) {
				data_raw[1] = value * CONVERT_M_Y;
			} else if (event->code == EVENT_TYPE_MAG_Z) {
				data_raw[2] = value * CONVERT_M_Z;
			} else {
				STLOGE("MagnSensor: unknown event code (type = %d, code = %d)", event->type, event->code);
			}
		} else if (event->type == EV_SYN) {

			data_rot[0] =	data_raw[0] * matrix_mag[0][0] +
					data_raw[1] * matrix_mag[1][0] +
					data_raw[2] * matrix_mag[2][0];
			data_rot[1] = 	data_raw[0] * matrix_mag[0][1] +
					data_raw[1] * matrix_mag[1][1] +
					data_raw[2] * matrix_mag[2][1];
			data_rot[2] = 	data_raw[0] * matrix_mag[0][2] +
					data_raw[1] * matrix_mag[1][2] +
					data_raw[2] * matrix_mag[2][2];
#if MAG_CALIBRATION_ENABLE_FILE
			data_rot[0] -= pStoreCalibration->getCalibration(StoreCalibration::MAGNETOMETER, 0);
			data_rot[1] -= pStoreCalibration->getCalibration(StoreCalibration::MAGNETOMETER, 1);
			data_rot[2] -= pStoreCalibration->getCalibration(StoreCalibration::MAGNETOMETER, 2);
#if ((MAG_CALIBRATION_ENABLE == 1) && (SENSORS_ACCELEROMETER_ENABLE == 1))
			if (pStoreCalibration->isChanged()) {
				compass_API_ResetCalibration(0);
			}
#endif
#endif
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
			AccelSensor::getBufferData(&mSensorsBufferedVectors[ID_ACCELEROMETER]);
#endif
#if (MAG_CALIBRATION_ENABLE == 1)
			compass_API_SaveMag(data_rot[0], data_rot[1], data_rot[2]);
#if (MAG_SI_COMPENSATION_ENABLED == 1)
#if (DEBUG_MAG_SI_COMPENSATION == 1)
			ALOGD("Mag RAW Data [uT]: %f  %f  %f", data_rot[0],
							data_rot[1], data_rot[2]);
#endif
			compass_API_getSICalibratedData(data_rot);
#endif
			if(data_read >= count_call_calibration) {
				data_read = 0;
				compass_API_SaveAcc(mSensorsBufferedVectors[ID_ACCELEROMETER].x,
							mSensorsBufferedVectors[ID_ACCELEROMETER].y,
							mSensorsBufferedVectors[ID_ACCELEROMETER].z);
				calibration_running = compass_API_Run();

#if DEBUG_CALIBRATION == 1
				STLOGD("Accelerometer Data [mg]: %f %f %f", data_accelerometer[0], data_accelerometer[1], data_accelerometer[2]);
				STLOGD("Calibration Running: %d, MagData [uT] -> x:%f y:%f z:%f", calibration_running, data_rot[0], data_rot[1], data_rot[2]);
#endif
			}
			data_read++;
#endif
			if ((mEnabled & (1 << MagneticField)) ||
				(mEnabled & (1 << UncalibMagneticField)) ||
				(mEnabled & (1 << GeoMagRotVect_Magnetic)) ||
				(mEnabled & (1 << Orientation)) ||
				(mEnabled & (1 << Linear_Accel)) ||
				(mEnabled & (1 << Gravity_Accel)) ||
				(mEnabled & (1 << iNemoMagnetic)) ||
				(mEnabled & (1 << VirtualGyro)))
			{
#if ((MAG_CALIBRATION_ENABLE == 1) && (SENSORS_ACCELEROMETER_ENABLE == 1))
				compass_API_getCalibrationData(&cf);
				MagOffset[0] = cf.magOffX;
				MagOffset[1] = cf.magOffY;
				MagOffset[2] = cf.magOffZ;
				status = compass_API_Get_Calibration_Accuracy();
#else
				MagOffset[0] = 0;
				MagOffset[1] = 0;
				MagOffset[2] = 0;
				status = SENSOR_STATUS_UNRELIABLE;
#endif
#if ((((SENSORS_ORIENTATION_ENABLE == 1) | (SENSORS_GRAVITY_ENABLE == 1) | (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)) && (SENSOR_GEOMAG_ENABLE == 1)) | ((SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1)))
				sData.accel[0] = mSensorsBufferedVectors[ID_ACCELEROMETER].x;
				sData.accel[1] = mSensorsBufferedVectors[ID_ACCELEROMETER].y;
				sData.accel[2] = mSensorsBufferedVectors[ID_ACCELEROMETER].z;
				sData.magn[0] = data_rot[0] - MagOffset[0];
				sData.magn[1] = data_rot[1] - MagOffset[1];
				sData.magn[2] = data_rot[2] - MagOffset[2];
				iNemoEngine_GeoMag_API_Run(MagnSensor::delayms, &sData);
#endif
				Mag_decimation_count++;
				if((mEnabled & (1<<MagneticField)) &&
				  (Mag_decimation_count >= DecimationBuffer[MagneticField])) {
					Mag_decimation_count = 0;
					mPendingEvent[MagneticField].magnetic.status = status;
					mPendingEvent[MagneticField].data[0] =
							data_rot[0] - MagOffset[0];
					mPendingEvent[MagneticField].data[1] =
							data_rot[1] - MagOffset[1];
					mPendingEvent[MagneticField].data[2] =
							data_rot[2] - MagOffset[2];
					mPendingEvent[MagneticField].timestamp =
							timevalToNano(event->time);
					*data++ = mPendingEvent[MagneticField];
					count--;
					numEventReceived++;
				}
#if (SENSORS_UNCALIB_MAGNETIC_FIELD_ENABLE == 1)
				MagUncalib_decimation_count++;
				if((mEnabled & (1<<UncalibMagneticField)) &&
				  (MagUncalib_decimation_count >= DecimationBuffer[UncalibMagneticField])) {
					MagUncalib_decimation_count = 0;
					mPendingEvent[UncalibMagneticField].magnetic.status = status;
					mPendingEvent[UncalibMagneticField].uncalibrated_magnetic.uncalib[0] = data_rot[0];
					mPendingEvent[UncalibMagneticField].uncalibrated_magnetic.uncalib[1] = data_rot[1];
					mPendingEvent[UncalibMagneticField].uncalibrated_magnetic.uncalib[2] = data_rot[2];
					mPendingEvent[UncalibMagneticField].uncalibrated_magnetic.bias[0] = MagOffset[0];
					mPendingEvent[UncalibMagneticField].uncalibrated_magnetic.bias[1] = MagOffset[1];
					mPendingEvent[UncalibMagneticField].uncalibrated_magnetic.bias[2] = MagOffset[2];
					mPendingEvent[UncalibMagneticField].timestamp = timevalToNano(event->time);
					*data++ = mPendingEvent[UncalibMagneticField];
					count--;
					numEventReceived++;
				}
#endif
#if (SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1)
				GeoMagnRotVect_decimation_count++;
				if((mEnabled & (1<<GeoMagRotVect_Magnetic)) &&
				  (GeoMagnRotVect_decimation_count >= DecimationBuffer[GeoMagRotVect_Magnetic])) {
					GeoMagnRotVect_decimation_count = 0;
					err = iNemoEngine_GeoMag_API_Get_Quaternion(quaternion);
					if (err == 0) {
						mPendingEvent[GeoMagRotVect_Magnetic].magnetic.status = status;
						mPendingEvent[GeoMagRotVect_Magnetic].data[0] = quaternion[0];
						mPendingEvent[GeoMagRotVect_Magnetic].data[1] = quaternion[1];
						mPendingEvent[GeoMagRotVect_Magnetic].data[2] = quaternion[2];
						mPendingEvent[GeoMagRotVect_Magnetic].data[3] = quaternion[3];
						mPendingEvent[GeoMagRotVect_Magnetic].data[4] = -1;
						mPendingEvent[GeoMagRotVect_Magnetic].timestamp = timevalToNano(event->time);
						*data++ = mPendingEvent[GeoMagRotVect_Magnetic];
						count--;
						numEventReceived++;
					}
				}
#endif
#if (SENSOR_FUSION_ENABLE == 0)
#if (SENSOR_GEOMAG_ENABLE == 1)
#if ((SENSORS_LINEAR_ACCELERATION_ENABLE== 1))
				Linear_Accel_decimation_count++;
				if((mEnabled & (1<<Linear_Accel)) &&
				  (Linear_Accel_decimation_count >= DecimationBuffer[Linear_Accel])) {
					Linear_Accel_decimation_count = 0;
					err = iNemoEngine_GeoMag_API_Get_LinAcc(LinAcc);
					if (err == 0) {
						mPendingEvent[Linear_Accel].data[0] = LinAcc[0];
						mPendingEvent[Linear_Accel].data[1] = LinAcc[1];
						mPendingEvent[Linear_Accel].data[2] = LinAcc[2];
						mPendingEvent[Linear_Accel].timestamp = timevalToNano(event->time);
						*data++ = mPendingEvent[Linear_Accel];
						count--;
						numEventReceived++;
					}
				}
#endif
#if ((SENSORS_GRAVITY_ENABLE == 1))
				Gravity_Accel_decimation_count++;
				if((mEnabled & (1<<Gravity_Accel)) &&
				  (Gravity_Accel_decimation_count >= DecimationBuffer[Gravity_Accel])) {
					Gravity_Accel_decimation_count = 0;
					err = iNemoEngine_GeoMag_API_Get_Gravity(GravAcc);
					if (err == 0) {
						mPendingEvent[Gravity_Accel].data[0] = GravAcc[0];
						mPendingEvent[Gravity_Accel].data[1] = GravAcc[1];
						mPendingEvent[Gravity_Accel].data[2] = GravAcc[2];
						mPendingEvent[Gravity_Accel].timestamp = timevalToNano(event->time);
						*data++ = mPendingEvent[Gravity_Accel];
						count--;
						numEventReceived++;
					}
				}
#endif
#endif
#if ((SENSORS_ORIENTATION_ENABLE == 1))
				Orientation_decimation_count++;
				if((mEnabled & (1<<Orientation)) &&
				  (Orientation_decimation_count >= DecimationBuffer[Orientation])) {
					Orientation_decimation_count = 0;
#if (SENSOR_GEOMAG_ENABLE != 1)
					orientation_data odata;
					compass_API_OrientationValues(&odata);
#else
					err = iNemoEngine_GeoMag_API_Get_Hpr(Hpr);
					if (err == 0)
#endif
					{
						mPendingEvent[Orientation].orientation.status = status;
#if (SENSOR_GEOMAG_ENABLE == 1)
						mPendingEvent[Orientation].data[0] = Hpr[0];
						mPendingEvent[Orientation].data[1] = Hpr[1];
						mPendingEvent[Orientation].data[2] = Hpr[2];
#else
						mPendingEvent[Orientation].data[0] = odata.azimuth;;
						mPendingEvent[Orientation].data[1] = odata.pitch;;
						mPendingEvent[Orientation].data[2] = odata.roll;;
#endif
						mPendingEvent[Orientation].timestamp = timevalToNano(event->time);
						*data++ = mPendingEvent[Orientation];
						count--;
						numEventReceived++;
					}
				}
#endif
#endif
#if (SENSOR_FUSION_ENABLE == 1)
				if(mEnabled & (1<<iNemoMagnetic)) {
					sensors_vec_t buffer;
					/*
					buffer.x = data_rot[0] - MGAUSS_TO_UT(MagOffset[0]);
					buffer.y = data_rot[1] - MGAUSS_TO_UT(MagOffset[1]);
					buffer.z = data_rot[2] - MGAUSS_TO_UT(MagOffset[2]);
					*/
					buffer.x = data_rot[0] - MagOffset[0];
					buffer.y = data_rot[1] - MagOffset[1];
					buffer.z = data_rot[2] - MagOffset[2];
//					STLOGD("Sava calibration data:%f, %f, %f!\n",buffer.x, buffer.y, buffer.z);	
					setBufferData(&buffer);
				}
#endif
#if DEBUG_MAGNETOMETER == 1
				STLOGD("MagnSensor::readEvents (time = %lld), count(%d), received(%d)", mPendingEvent[MagneticField].timestamp, count, numEventReceived);
#endif
			}
		} else {
			STLOGE("MagnSensor: unknown event (type = %d, code = %d)", event->type, event->code);
		}
		mInputReader.next();
	}
#if FETCH_FULL_EVENT_BEFORE_RETURN
	/* if we didn't read a complete event, see if we can fill and
	try again instead of returning with nothing and redoing poll. */
	if (numEventReceived == 0 && mEnabled != 0) {
		n = mInputReader.fill(data_fd);
		if (n)
			goto again;
	}
#endif
	return numEventReceived;
}

bool MagnSensor::setBufferData(sensors_vec_t *value)
{
	pthread_mutex_lock(&dataMutex);
	dataBuffer.x = value->x;
	dataBuffer.y = value->y;
	dataBuffer.z = value->z;
	pthread_mutex_unlock(&dataMutex);

	return true;
}

bool MagnSensor::getBufferData(sensors_vec_t *lastBufferedValues)
{
	pthread_mutex_lock(&dataMutex);
	lastBufferedValues->x = dataBuffer.x;
	lastBufferedValues->y = dataBuffer.y;
	lastBufferedValues->z = dataBuffer.z;
#if MAG_CALIBRATION_ENABLE == 1
	lastBufferedValues->status = status;
#endif
	pthread_mutex_unlock(&dataMutex);

	return true;
}

#endif /* SENSORS_MAGNETIC_FIELD_ENABLE */
