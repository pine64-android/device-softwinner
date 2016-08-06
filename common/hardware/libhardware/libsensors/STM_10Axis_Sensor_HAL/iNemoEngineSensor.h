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
#if (SENSOR_FUSION_ENABLE == 1)

#ifndef ANDROID_INEMOENGINE_SENSOR_H
#define ANDROID_INEMOENGINE_SENSOR_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "sensors.h"
#include "SensorBase.h"
#include "InputEventReader.h"
#include "AccelSensor.h"
#include "GyroSensor.h"
#include "MagnSensor.h"

extern "C"
{
	#include "iNemoEngineAPI.h"
};

/*****************************************************************************/

struct input_event;

class iNemoEngineSensor : public SensorBase
{
	enum {
		Acceleration = 0,
		MagneticField,
		AngularSpeed,
		Orientation,
		Gravity,
		LinearAcceleration,
		RotationMatrix,
		GameRotation,
		UncalibGyro,
		CalibGyro,
		numSensors
	};

	int initialized;
	static int mEnabled;
	uint32_t mPendingMask;
	InputEventCircularReader mInputReader;
	sensors_event_t mPendingEvents[numSensors];
	bool mHasPendingEvent;
	int setInitialState();

private:
	static int startup_samples;
	static int samples_to_discard;
	sensors_vec_t mSensorsBufferedVectors[3];
	iNemoInitData init_data_api;
	iNemoDebugInitData debug_init_data_api;
	char devices_sysfs_path_gyr[PATH_MAX];
	char devices_sysfs_path_acc[PATH_MAX];
	char devices_sysfs_path_mag[PATH_MAX];
	int devices_sysfs_path_gyr_len;
	int devices_sysfs_path_acc_len;
	int devices_sysfs_path_mag_len;
#if (SENSORS_GYROSCOPE_ENABLE == 1)
	static GyroSensor *gyr;
#endif
#if(SENSORS_ACCELEROMETER_ENABLE == 1)
	static AccelSensor *acc;
#endif
#if (SENSORS_MAGNETIC_FIELD_ENABLE == 1)
	static MagnSensor *mag;
#endif
	static int status;
	static int64_t Gyro_Delay_ms;
	static int64_t DelayBuffer[numSensors];
	static int DecimationBuffer[numSensors];
	static int Orientation_decimation_count;
	static int Gravity_decimation_count;
	static int LinearAcceleration_decimation_count;
	static int RotationMatrix_decimation_count;
	static int GameRotation_decimation_count;
	static int UncalibGyro_decimation_count;
	static int CalibGyro_decimation_count;

public:
	iNemoEngineSensor();
	virtual ~iNemoEngineSensor();
	virtual int readEvents(sensors_event_t* data, int count);
	virtual bool hasPendingEvents() const;
	virtual int setDelay(int32_t handle, int64_t ns);
	virtual int enable(int32_t handle, int enabled, int type);
	virtual void updateDecimations(int64_t Delay_ms);
};

#endif  // ANDROID_INEMOENGINE_SENSOR_H

#endif /* SENSOR_FUSION_ENABLE */
