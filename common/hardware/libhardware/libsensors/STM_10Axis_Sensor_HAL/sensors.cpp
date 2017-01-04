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


#include <hardware/sensors.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <stdlib.h>

#include <linux/input.h>
#include <cutils/log.h>

#include "sensors.h"
#include "configuration.h"

#include "LightSensor.h"

#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
#include "MagnSensor.h"
#endif
#if (SENSORS_GYROSCOPE_ENABLE == 1)
#include "GyroSensor.h"
#endif
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
#include "AccelSensor.h"
#endif
#if (SENSOR_FUSION_ENABLE == 1)
#include "iNemoEngineSensor.h"
#endif
#if (SENSORS_PRESSURE_ENABLE == 1)
#include "PressTempSensor.h"
#endif

#if (SENSORS_VIRTUAL_GYROSCOPE_ENABLE == 1)
#include "VirtualGyroSensor.h"
#endif






#include <cutils/properties.h>
/*****************************************************************************/

#define DELAY_OUT_TIME			0x7FFFFFFF

#define LIGHT_SENSOR_POLLTIME		2000000000

/*****************************************************************************/



static int open_sensors(const struct hw_module_t* module, const char* id, struct hw_device_t** device);

//void get_ref(sensors_module_t *sm);

static struct hw_module_methods_t sensors_module_methods = {
	open: open_sensors
};

int sensors__get_sensors_list(struct sensors_module_t* module, struct sensor_t const** list)
{
	*list = sSensorList;
	return ARRAY_SIZE(sSensorList);
};

struct sensors_module_t HAL_MODULE_INFO_SYM = {
	common: {
		tag: HARDWARE_MODULE_TAG,
		version_major: 1,
		version_minor: 0,
		id: SENSORS_HARDWARE_MODULE_ID,
		name: "STMicroelectronics Sensor module",
		author: "STMicroelectronics",
		methods: &sensors_module_methods,
		dso: NULL,
		reserved: { },
	},
	get_sensors_list: sensors__get_sensors_list,
};

/*void get_ref(sensors_module_t *sm) {
	sm = &HAL_MODULE_INFO_SYM;
	return;
}*/

struct sensors_poll_context_t {
	struct sensors_poll_device_t device;

	sensors_poll_context_t();
	~sensors_poll_context_t();
	int activate(int handle, int enabled);
	int setDelay(int handle, int64_t ns);
	int pollEvents(sensors_event_t* data, int count);

private:
	enum {
#if (SENSORS_GYROSCOPE_ENABLE == 1)
		gyro = 0,
#endif
#if (SENSORS_VIRTUAL_GYROSCOPE_ENABLE == 1)
		virtual_gyro,
#endif
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
		accel,
#endif
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
		magn,
#endif
#if (SENSOR_FUSION_ENABLE == 1)
		inemo,
#endif
#if (SENSORS_TEMPERATURE_ENABLE == 1) || (SENSORS_PRESSURE_ENABLE == 1)
		presstemp,
#endif
		light,
		numSensorDrivers,
		numFds,
	};

	static const size_t wake = numFds - 1;
	static const char WAKE_MESSAGE = 'W';
	struct pollfd mPollFds[numFds];
	int mWritePipeFd;
	SensorBase* mSensors[numSensorDrivers];

	int handleToDriver(int handle) const
	{
		switch (handle) {
#if (SENSORS_ACCELEROMETER_ENABLE == 1)
			case SENSORS_ACCELEROMETER_HANDLE:
				return accel;
#endif

#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
			case SENSORS_MAGNETIC_FIELD_HANDLE:
				return magn;
#endif

#if ((SENSORS_ORIENTATION_ENABLE == 1) && ((SENSOR_FUSION_ENABLE) || (MAG_CALIBRATION_ENABLE)))
			case SENSORS_ORIENTATION_HANDLE:
#if (SENSOR_FUSION_ENABLE == 1)
				return inemo;
#else
				return magn;
#endif
#endif

#if ((SENSORS_GRAVITY_ENABLE == 1) && ((SENSOR_FUSION_ENABLE) || (MAG_CALIBRATION_ENABLE)))
			case SENSORS_GRAVITY_HANDLE:
#if (SENSOR_FUSION_ENABLE == 1)
				return inemo;
#else
				return magn;
#endif
#endif

#if ((SENSORS_LINEAR_ACCELERATION_ENABLE == 1) && ((SENSOR_FUSION_ENABLE) || (MAG_CALIBRATION_ENABLE)))
			case SENSORS_LINEAR_ACCELERATION_HANDLE:
#if (SENSOR_FUSION_ENABLE == 1)
				return inemo;
#else
				return magn;
#endif
#endif

#if (SENSORS_ROTATION_VECTOR_ENABLE == 1) && (SENSOR_FUSION_ENABLE == 1)
			case SENSORS_ROTATION_VECTOR_HANDLE:
				return inemo;
#endif
#if (SENSORS_PRESSURE_ENABLE == 1)
			case SENSORS_PRESSURE_HANDLE:
				return presstemp;
#endif
#if (SENSORS_TEMPERATURE_ENABLE == 1)
			case SENSORS_TEMPERATURE_HANDLE:
				return presstemp;
#endif
#if (SENSORS_GYROSCOPE_ENABLE == 1)
			case SENSORS_GYROSCOPE_HANDLE:
#if (GYROSCOPE_GBIAS_ESTIMATION_FUSION == 1)
				return inemo;
#else
				return gyro;
#endif
#endif
#if (SENSORS_VIRTUAL_GYROSCOPE_ENABLE == 1)
			case SENSORS_VIRTUAL_GYROSCOPE_HANDLE:
				return virtual_gyro;
#endif
#if (SENSORS_GAME_ROTATION_ENABLE == 1) && (SENSOR_FUSION_ENABLE == 1)
			case SENSORS_GAME_ROTATION_HANDLE:
				return inemo;
#endif
#if ((SENSORS_UNCALIB_GYROSCOPE_ENABLE == 1) && ((GYROSCOPE_GBIAS_ESTIMATION_FUSION == 1) || (GYROSCOPE_GBIAS_ESTIMATION_STANDALONE == 1)))
			case SENSORS_UNCALIB_GYROSCOPE_HANDLE:
#if (GYROSCOPE_GBIAS_ESTIMATION_FUSION == 1)
				return inemo;
#else
				return gyro;
#endif
#endif
#if (SENSORS_SIGNIFICANT_MOTION_ENABLE == 1)
			case SENSORS_SIGNIFICANT_MOTION_HANDLE:
				return accel;
#endif
#if SENSORS_UNCALIB_MAGNETIC_FIELD_ENABLE
			case SENSORS_UNCALIB_MAGNETIC_FIELD_HANDLE:
				return magn;
#endif
#if SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE
			case SENSORS_GEOMAG_ROTATION_VECTOR_HANDLE:
				return magn;
#endif
			case SENSORS_LIGHT_HANDLE:
				return light;
		}
		return -EINVAL;
	}
};

/*****************************************************************************/

sensors_poll_context_t::sensors_poll_context_t()
{
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
	mSensors[magn] = new MagnSensor();
	mPollFds[magn].fd = mSensors[magn]->getFd();
	mPollFds[magn].events = POLLIN;
	mPollFds[magn].revents = 0;
#endif

#if (SENSORS_GYROSCOPE_ENABLE == 1)
	mSensors[gyro] = new GyroSensor();
	mPollFds[gyro].fd = mSensors[gyro]->getFd();
	mPollFds[gyro].events = POLLIN;
	mPollFds[gyro].revents = 0;
#endif

#if (SENSORS_VIRTUAL_GYROSCOPE_ENABLE == 1)
	mSensors[virtual_gyro] = new VirtualGyroSensor();
	mPollFds[virtual_gyro].fd = mSensors[virtual_gyro]->getFd();
	mPollFds[virtual_gyro].events = POLLIN;
	mPollFds[virtual_gyro].revents = 0;
#endif

#if (SENSORS_ACCELEROMETER_ENABLE == 1)
	mSensors[accel] = new AccelSensor();
	mPollFds[accel].fd = mSensors[accel]->getFd();
	mPollFds[accel].events = POLLIN;
	mPollFds[accel].revents = 0;
#endif

#if (SENSOR_FUSION_ENABLE == 1)
	mSensors[inemo] = new iNemoEngineSensor();
	mPollFds[inemo].fd = mSensors[inemo]->getFd();
	mPollFds[inemo].events = POLLIN;
	mPollFds[inemo].revents = 0;
#endif

#if (SENSORS_TEMPERATURE_ENABLE == 1) || (SENSORS_PRESSURE_ENABLE == 1)
	mSensors[presstemp] = new PressTempSensor();
	mPollFds[presstemp].fd = mSensors[presstemp]->getFd();
	mPollFds[presstemp].events = POLLIN;
	mPollFds[presstemp].revents = 0;
#endif

        mSensors[light] = new LightSensor();
        mPollFds[light].fd = mSensors[light]->getFd();
        mPollFds[light].events = POLLIN;
        mPollFds[light].revents = 0;


        

	int wakeFds[2];
	int result = pipe(wakeFds);
	STLOGE_IF(result<0, "error creating wake pipe (%s)", strerror(errno));
	fcntl(wakeFds[0], F_SETFL, O_NONBLOCK);
	fcntl(wakeFds[1], F_SETFL, O_NONBLOCK);
	mWritePipeFd = wakeFds[1];

	mPollFds[wake].fd = wakeFds[0];
	mPollFds[wake].events = POLLIN;
	mPollFds[wake].revents = 0;
}

sensors_poll_context_t::~sensors_poll_context_t()
{
	for (int i=0 ; i<numSensorDrivers ; i++) {
		delete mSensors[i];
	}
	close(mPollFds[wake].fd);
	close(mWritePipeFd);
}

int sensors_poll_context_t::activate(int handle, int enabled)
{
	int index = handleToDriver(handle);
	if(index < 0)
		return index;

	int err =  mSensors[index]->enable(handle, enabled, 0);
	if(enabled && !err) {
		const char wakeMessage(WAKE_MESSAGE);
		int result = write(mWritePipeFd, &wakeMessage, 1);
		STLOGE_IF(result < 0, "error sending wake message (%s)", strerror(errno));
	}
	return err;
}

int sensors_poll_context_t::setDelay(int handle, int64_t ns)
{
	int index = handleToDriver(handle);
	if(index < 0)
		return index;

	return mSensors[index]->setDelay(handle, ns);
}

int sensors_poll_context_t::pollEvents(sensors_event_t* data, int count)
{
	int nbEvents = 0;
	int n = 0;

	do {
		for (int i=0 ; count && i<numSensorDrivers ; i++) {
			SensorBase* const sensor(mSensors[i]);
			if((mPollFds[i].revents & POLLIN) || (sensor->hasPendingEvents()))
			{
				int nb = sensor->readEvents(data, count);
				if (nb < count) {
					mPollFds[i].revents = 0;
				}
				count -= nb;
				nbEvents += nb;
				data += nb;
			}
		}

		if (count) {
			n = poll(mPollFds, numFds, 0);
			if (n < 0) {
				STLOGE("poll() failed (%s)", strerror(errno));
				return -errno;
			}
			if (mPollFds[wake].revents & POLLIN) {
				char msg;
				int result = read(mPollFds[wake].fd, &msg, 1);
				STLOGE_IF(result < 0, "error reading from wake pipe (%s)", strerror(errno));
				STLOGE_IF(msg != WAKE_MESSAGE, "unknown message on wake queue (0x%02x)", int(msg));
				mPollFds[wake].revents = 0;
			}
		}

	} while (n && count);

	return nbEvents;
}

/*****************************************************************************/

static int poll__close(struct hw_device_t *dev)
{
	sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
	if(ctx) {
		delete ctx;
	}
	return 0;
}

static int poll__activate(struct sensors_poll_device_t *dev, int handle, int enabled)
{
	sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
	return ctx->activate(handle, enabled);
}

static int poll__setDelay(struct sensors_poll_device_t *dev, int handle, int64_t ns)
{
	sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
	return ctx->setDelay(handle, ns);
}

static int poll__poll(struct sensors_poll_device_t *dev, sensors_event_t* data, int count)
{
	sensors_poll_context_t *ctx = (sensors_poll_context_t *)dev;
	return ctx->pollEvents(data, count);
}

/*****************************************************************************/

/** Open a new instance of a sensor device using name */
static int open_sensors(const struct hw_module_t* module, const char* id, struct hw_device_t** device)
{
	int status = -EINVAL;
    //insmodDevice();/*Automatic detection, loading device drivers */
    property_set("sys.sensors", "1");/*Modify the  enable and delay interface  group */
//    sensorsDetect();/*detect device,filling sensor_t structure */
	sensors_poll_context_t *dev = new sensors_poll_context_t();
	memset(&dev->device, 0, sizeof(sensors_poll_device_t));

	dev->device.common.tag		= HARDWARE_DEVICE_TAG;
	dev->device.common.version	= 0;
	dev->device.common.module	= const_cast<hw_module_t*>(module);
	dev->device.common.close	= poll__close;
	dev->device.activate		= poll__activate;
	dev->device.setDelay		= poll__setDelay;
	dev->device.poll		= poll__poll;

	*device = &dev->device.common;
	status = 0;

	return status;
}

