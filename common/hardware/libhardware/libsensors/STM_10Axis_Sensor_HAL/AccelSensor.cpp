/*
 * Copyright (C) 2013 STMicroelectronics
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
#if (SENSORS_ACCELEROMETER_ENABLE == 1)

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/select.h>
#include <dlfcn.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include "AccelSensor.h"
#include SENSOR_ACC_INCLUDE_FILE_NAME

#define FETCH_FULL_EVENT_BEFORE_RETURN		0
//#define DEBUG_SENSOR 1

/*****************************************************************************/

sensors_vec_t  AccelSensor::dataBuffer;
int AccelSensor::mEnabled = 0;
int64_t AccelSensor::delayms = 0;
int AccelSensor::current_fullscale = 0;
int64_t AccelSensor::setDelayBuffer[numSensors] = {0};
int AccelSensor::DecimationBuffer[numSensors] = {0};
int AccelSensor::Acc_decimation_count = 0;
pthread_mutex_t AccelSensor::dataMutex;

AccelSensor::AccelSensor()
	: SensorBase(NULL, SENSOR_DATANAME_ACCELEROMETER),   
	mInputReader(4),
	mHasPendingEvent(false),
	direct_x(0), 
	direct_y(0), 
	direct_z(0), 
	direct_xy(-1)
{
	pthread_mutex_init(&dataMutex, NULL);
	if (strlen(SENSOR_DATANAME_ACCELEROMETER)) {
                if(! gsensor_cfg())
                        ALOGE("gsensor config error!\n"); 
        }

	memset(mPendingEvents, 0, sizeof(mPendingEvents));

	mPendingEvents[Acceleration].version = sizeof(sensors_event_t);
	mPendingEvents[Acceleration].sensor = ID_ACCELEROMETER;
	mPendingEvents[Acceleration].type = SENSOR_TYPE_ACCELEROMETER;
	mPendingEvents[Acceleration].acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;

#if SENSORS_SIGNIFICANT_MOTION_ENABLE == 1
	mPendingEvents[SignificantMotion].version = sizeof(sensors_event_t);
	mPendingEvents[SignificantMotion].sensor = ID_SIGNIFICANT_MOTION;
	mPendingEvents[SignificantMotion].type = SENSOR_TYPE_SIGNIFICANT_MOTION;
	mPendingEvents[SignificantMotion].acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;
#endif

	if (data_fd) {
		STLOGI("AccelSensor::AccelSensor accel_device_sysfs_path:(%s)", sysfs_device_path);
	} else {
		STLOGE("AccelSensor::AccelSensor accel_device_sysfs_path:(%s) not found", sysfs_device_path);
	}

	data_raw[0] = data_raw[1] = data_raw[2] = 0.0;

#if ACC_CALIBRATION_ENABLE_FILE
	pStoreCalibration = StoreCalibration::getInstance();
#endif
}

AccelSensor::~AccelSensor()
{
	if (mEnabled) {
		enable(SENSORS_ACCELEROMETER_HANDLE, 0, 0);
	}
	pthread_mutex_destroy(&dataMutex);
}

char* AccelSensor::get_cfg_value(char *buf) {
        int j = 0;
        int k = 0; 
        char* val;
        
        val = strtok(buf, "=");
        if (val != NULL){
                val = strtok(NULL, " \n\r\t");
        }
        buf = val;
        
        return buf;
}

int AccelSensor::gsensor_cfg()
{
        FILE *fp;
        int name_match = 0;
        char buf[128] = {0};
        char * val;
        
        if((fp = fopen(GSENSOR_CONFIG_PATH, "rb")) == NULL) {
                ALOGD("can't not open file!\n");
                return 0;
        }
        
        while(fgets(buf, LINE_LENGTH, fp))
        {
                if (!strncmp(buf, GSENSOR_NAME, strlen(GSENSOR_NAME))) {
                        val = get_cfg_value(buf);
                        #ifdef DEBUG_SENSOR
                                ALOGD("val:%s\n",val);
                        #endif
                        name_match = (strncmp(val, SENSOR_DATANAME_ACCELEROMETER, strlen(SENSOR_DATANAME_ACCELEROMETER))) ? 0 : 1;
                                
                        if (name_match)  {
                                memset(&buf, 0, sizeof(buf));
                                continue;
                        } 
                        
                }  
                
                if(name_match ==0){
                        memset(&buf, 0, sizeof(buf));
                        continue;
                }else if(name_match < 5){
                        name_match++;
                        val = get_cfg_value(buf); 
                        #ifdef DEBUG_SENSOR
                                ALOGD("val:%s\n", val);
                        #endif
                        
                       if (!strncmp(buf,GSENSOR_DIRECTX, strlen(GSENSOR_DIRECTX))){                                
                                  direct_x = (strncmp(val, TRUE,strlen(val))) ? (-1) : 1;      
                       }
                       
                       if (!strncmp(buf, GSENSOR_DIRECTY, strlen(GSENSOR_DIRECTY))){                                         
                                  direct_y =(strncmp(val, TRUE,strlen(val))) ? (-1) : 1;      
                       }
                       
                      if (!strncmp(buf, GSENSOR_DIRECTZ, strlen(GSENSOR_DIRECTZ))){
                                 direct_z =(strncmp(val, TRUE,strlen(val))) ?  (-1) : 1; 
                       }
                       
                       if (!strncmp(buf,GSENSOR_XY, strlen(GSENSOR_XY))){
                                 direct_xy = (strncmp(val, TRUE,strlen(val))) ? 0 : 1; 
                       }
                       
                
                }else{
                        name_match = 0;
                        break;
                }
                memset(&buf, 0, sizeof(buf));
        }
    char property[PROPERTY_VALUE_MAX];
    property_get("ro.sf.hwrotation", property, 0);
    switch (atoi(property)) {
        case 90:
            direct_y = (-1) * direct_y;
            direct_xy = (direct_xy == 1) ? 0 : 1;
            break;
        case 180:
            direct_x = (-1) * direct_x;
            direct_y = (-1) * direct_y;
            break;
        case 270:
            direct_x = (-1) * direct_x;
            direct_xy = (direct_xy == 1) ? 0 : 1;
            break;
    }

        #ifdef DEBUG_SENSOR
                ALOGD("direct_x: %f,direct_y: %f,direct_z: %f,direct_xy:%d,sensor_name:%s \n",
                        direct_x, direct_y, direct_z, direct_xy, SENSOR_DATANAME_ACCELEROMETER);
        #endif
        
        if((direct_x == 0) || (direct_y == 0) || (direct_z == 0) || (direct_xy == (-1)) ) {
                return 0;
        }
        
        fclose(fp);
        return 1;
    
}

int AccelSensor::setInitialState()
{
	struct input_absinfo absinfo_x;
	struct input_absinfo absinfo_y;
	struct input_absinfo absinfo_z;
	int clockid = CLOCK_BOOTTIME;

	if (!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_ACCEL_X), &absinfo_x) &&
		!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_ACCEL_Y), &absinfo_y) &&
		!ioctl(data_fd, EVIOCGABS(EVENT_TYPE_ACCEL_Z), &absinfo_z))
	{
		mHasPendingEvent = true;
	}

	setFullScale(SENSORS_ACCELEROMETER_HANDLE, ACCEL_DEFAULT_FULLSCALE);
	
	if (!ioctl(data_fd, EVIOCSCLOCKID, &clockid))
		ALOGE("AccelSensor : set EVIOCSCLOCKID = %d\n",clockid);
	else
		ALOGE("AccelSensor : set EVIOCSCLOCKID failed ! \n");

	Acc_decimation_count = 0;

	return 0;
}

int AccelSensor::enable(int32_t handle, int en, int type)
{
	int err = 0, errSM1 = 0, errSM2 = 0;
	int flags = en ? 1 : 0;
	int what = -1;

	switch(handle) {
		case SENSORS_ACCELEROMETER_HANDLE:
			what = Acceleration;
			break;
#if SENSORS_SIGNIFICANT_MOTION_ENABLE == 1
		case SENSORS_SIGNIFICANT_MOTION_HANDLE:
			what = SignificantMotion;
			break;
#endif
#if SENSOR_FUSION_ENABLE == 1
		case SENSORS_SENSOR_FUSION_HANDLE:
			what = iNemoAcceleration;
			break;
#endif
#if MAG_CALIBRATION_ENABLE == 1
		case SENSORS_MAGNETIC_FIELD_HANDLE:
			what = MagCalAcceleration;
			break;
#endif
#if (SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1)
		case SENSORS_GEOMAG_ROTATION_VECTOR_HANDLE:
			what = GeoMagRotVectAcceleration;
			break;
#endif
#if (SENSOR_FUSION_ENABLE == 0)
#if (SENSORS_ORIENTATION_ENABLE == 1)
		case SENSORS_ORIENTATION_HANDLE:
			what = Orientation;
			break;
#endif
#if (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)
		case SENSORS_LINEAR_ACCELERATION_HANDLE:
			what = Linear_Accel;
			break;
#endif
#if (SENSORS_GRAVITY_ENABLE == 1)
		case SENSORS_GRAVITY_HANDLE:
			what = Gravity_Accel;
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
#if SENSORS_SIGNIFICANT_MOTION_ENABLE == 1
		if (what == SignificantMotion) {
			//enable sysfs state machine
			if((mEnabled & (1<<Acceleration)) == 0x00)
				errSM1 = writeSysfsCommand(SENSORS_SIGNIFICANT_MOTION_HANDLE, SIGN_MOTION_POLL_EN_FILE_NAME, "%lld" ,0);
			errSM2 = writeEnable(SENSORS_SIGNIFICANT_MOTION_HANDLE, SIGN_MOTION_ENABLE_VALUE);

		}
#endif
		if (what == Acceleration) {
#if SENSORS_SIGNIFICANT_MOTION_ENABLE == 1
			errSM2 = writeSysfsCommand(SENSORS_ACCELEROMETER_HANDLE, SIGN_MOTION_POLL_EN_FILE_NAME, "%lld", 1);
#endif
		}
		if ((!mEnabled) && (errSM1 >=0) && (errSM2 >=0)) {
			setInitialState();
			err = writeEnable(SENSORS_ACCELEROMETER_HANDLE, flags);	// Enable Accelerometer
		}
		mEnabled |= (1<<what);

	} else {
		int tmp = mEnabled;
		mEnabled &= ~(1<<what);
#if SENSORS_SIGNIFICANT_MOTION_ENABLE == 1
		if (what == SignificantMotion)
			errSM1 = writeEnable(SENSORS_SIGNIFICANT_MOTION_HANDLE, SIGN_MOTION_DISABLE_VALUE);
#endif
		if (what == Acceleration) {
#if SENSORS_SIGNIFICANT_MOTION_ENABLE == 1
			errSM2 = writeSysfsCommand(SENSORS_ACCELEROMETER_HANDLE, SIGN_MOTION_POLL_EN_FILE_NAME, "%lld", 0);
#endif
		}
		if((!mEnabled) && (tmp != 0)) {
			err = writeEnable(SENSORS_ACCELEROMETER_HANDLE, flags);
		}
		if ( (errSM1 < 0) || (errSM2 < 0) ){
			err = -1;
			mEnabled = tmp;
		}
		setDelay(handle,200000000);
	}

	if(err >= 0 ) {
		STLOGD("AccelSensor::enable(%d), handle: %d, what: %d, mEnabled: %x",flags, handle, what, mEnabled);
	} else {
		STLOGE("AccelSensor::enable(%d), handle: %d, what: %d, mEnabled: %x",flags, handle, what, mEnabled);
	}

	return err;
}

bool AccelSensor::hasPendingEvents() const
{
	return mHasPendingEvent;
}

int AccelSensor::setDelay(int32_t handle, int64_t delay_ns)
{
	int err = 0;
	int kk;
	int what = -1;
	int64_t delay_ms = delay_ns/1000000;
	int64_t Min_delay_ms = 0;

	if(delay_ms == 0)
		return err;

	switch(handle) {
		case SENSORS_ACCELEROMETER_HANDLE:
			what = Acceleration;
			break;
#if SENSORS_SIGNIFICANT_MOTION_ENABLE == 1
		case SENSORS_SIGNIFICANT_MOTION_HANDLE:
			what = SignificantMotion;
			break;
#endif
#if SENSOR_FUSION_ENABLE == 1
		case SENSORS_SENSOR_FUSION_HANDLE:
			what = iNemoAcceleration;
			break;
#endif
#if MAG_CALIBRATION_ENABLE == 1
		case SENSORS_MAGNETIC_FIELD_HANDLE:
			what = MagCalAcceleration;
			break;
#endif
#if SENSORS_GEOMAG_ROTATION_VECTOR_ENABLE == 1
		case SENSORS_GEOMAG_ROTATION_VECTOR_HANDLE:
			what = GeoMagRotVectAcceleration;
			break;
#endif
#if (SENSOR_FUSION_ENABLE == 0)
#if (SENSORS_ORIENTATION_ENABLE == 1)
		case SENSORS_ORIENTATION_HANDLE:
			what = Orientation;
			break;
#endif
#if (SENSORS_LINEAR_ACCELERATION_ENABLE == 1)
		case SENSORS_LINEAR_ACCELERATION_HANDLE:
			what = Linear_Accel;
			break;
#endif
#if (SENSORS_GRAVITY_ENABLE == 1)
		case SENSORS_GRAVITY_HANDLE:
			what = Gravity_Accel;
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

	if (mEnabled & (1<<SignificantMotion))
	{
		delay_ms = ACC_DEFAULT_DELAY;
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


	if (mEnabled && (Min_delay_ms != delayms))
	{
		err = writeDelay(SENSORS_ACCELEROMETER_HANDLE, Min_delay_ms);

		if(err >= 0) {
			err = 0;
			delayms = Min_delay_ms;

			Acc_decimation_count = 0;
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

#if DEBUG_ACCELEROMETER == 1
	STLOGD("AccSensor::setDelayBuffer[] = %lld, %lld, %lld, %lld, %lld, %lld, %lld, %lld", setDelayBuffer[0], setDelayBuffer[1], setDelayBuffer[2], setDelayBuffer[3], setDelayBuffer[4], setDelayBuffer[5], setDelayBuffer[6], setDelayBuffer[7]);
	STLOGD("AccSensor::Min_delay_ms = %lld, delayms = %lld, mEnabled = %d", Min_delay_ms, delayms, mEnabled);
	STLOGD("AccSensor::DecimationBuffer = %d, %d, %d, %d, %d, %d, %d, %d", DecimationBuffer[0], DecimationBuffer[1], DecimationBuffer[2], DecimationBuffer[3], DecimationBuffer[4], DecimationBuffer[5], DecimationBuffer[6], DecimationBuffer[7]);
#endif

	return err;
}

int AccelSensor::setFullScale(int32_t handle, int value)
{
	int err = -1;

	if(value <= 0)
		return err;
	else
		err = 0;

	if(value != current_fullscale)
	{
		err = writeFullScale(SENSORS_ACCELEROMETER_HANDLE, value);

		if(err >= 0) {
			err = 0;
			current_fullscale = value;
		}
	}
	return err;
}

int AccelSensor::readEvents(sensors_event_t* data, int count)
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

#if DEBUG_ACCELEROMETER == 1
	STLOGD("AccelSensor::readEvents (event_code=%d)", event->code);
#endif

			float value = (float) event->value;
			if (event->code == EVENT_TYPE_ACCEL_X) {
					if(direct_xy) {
							data_raw[1] = value * CONVERT_A_Y * direct_y;
                    }else {
                    		data_raw[0] = value * CONVERT_A_X * direct_x;
                    }
			}
			else if (event->code == EVENT_TYPE_ACCEL_Y) {
					if(direct_xy) {
							data_raw[0] = value * CONVERT_A_X * direct_x;
                    }else {
                    		data_raw[1] = value * CONVERT_A_Y * direct_y;
                    }
			}
			else if (event->code == EVENT_TYPE_ACCEL_Z) {
				data_raw[2] = value * CONVERT_A_Z * direct_z;
			}
#if SENSORS_SIGNIFICANT_MOTION_ENABLE == 1
			else if (event->code == EVENT_TYPE_SIGNIFICANT_MOTION) {

				if(mEnabled & (1<<SignificantMotion)) {
					mPendingEvents[SignificantMotion].data[0] = value;
					mPendingEvents[SignificantMotion].timestamp = timevalToNano(event->time);
					mPendingMask |= 1<<SignificantMotion;
#if DEBUG_ACCELEROMETER == 1
					STLOGD("AccelSensor::SignificantMotion event type (type = %d, code = %d)", event->type, event->code);
#endif

				}
			}
#endif
			else {
				STLOGE("AccelSensor: unknown event code (type = %d, code = %d)", event->type, event->code);
			}
		} else if (event->type == EV_SYN) {

			data_rot[0] = data_raw[0]*matrix_acc[0][0] + data_raw[1]*matrix_acc[1][0] + data_raw[2]*matrix_acc[2][0];
			data_rot[1] = data_raw[0]*matrix_acc[0][1] + data_raw[1]*matrix_acc[1][1] + data_raw[2]*matrix_acc[2][1];
			data_rot[2] = data_raw[0]*matrix_acc[0][2] + data_raw[1]*matrix_acc[1][2] + data_raw[2]*matrix_acc[2][2];

#if ACC_CALIBRATION_ENABLE_FILE
			data_rot[0] -= pStoreCalibration->getCalibration(StoreCalibration::ACCELEROMETER_BIAS, 0);
			data_rot[1] -= pStoreCalibration->getCalibration(StoreCalibration::ACCELEROMETER_BIAS, 1);
			data_rot[2] -= pStoreCalibration->getCalibration(StoreCalibration::ACCELEROMETER_BIAS, 2);
			data_rot[0] *= pStoreCalibration->getCalibration(StoreCalibration::ACCELEROMETER_GAIN, 0);
			data_rot[1] *= pStoreCalibration->getCalibration(StoreCalibration::ACCELEROMETER_GAIN, 1);
			data_rot[2] *= pStoreCalibration->getCalibration(StoreCalibration::ACCELEROMETER_GAIN, 2);
#endif

			Acc_decimation_count++;

			if ((mEnabled & (1<<Acceleration)) &&
			   (Acc_decimation_count >= DecimationBuffer[Acceleration])) {
				Acc_decimation_count = 0;

				mPendingEvents[Acceleration].data[0] = data_rot[0];
				mPendingEvents[Acceleration].data[1] = data_rot[1];
				mPendingEvents[Acceleration].data[2] = data_rot[2];
				mPendingEvents[Acceleration].timestamp = timevalToNano(event->time);
				mPendingMask |= 1<<Acceleration;
			}

			if ((mEnabled & (1<<iNemoAcceleration)) ||
				(mEnabled & (1<<MagCalAcceleration)) ||
				(mEnabled & (1<<GeoMagRotVectAcceleration)) ||
				(mEnabled & (1<<Orientation)) ||
				(mEnabled & (1<<Linear_Accel)) ||
				(mEnabled & (1<<Gravity_Accel)))
			{
				sensors_vec_t sData;
				sData.x = data_rot[0];
				sData.y = data_rot[1];
				sData.z = data_rot[2];
				setBufferData(&sData);
			}

#if DEBUG_ACCELEROMETER == 1
			STLOGE("AccelSensor(Acceleration)::readEvents (time = %lld), count(%d), received(%d)", mPendingEvents[Acceleration].timestamp, count, numEventReceived);
#endif
		} else {
			STLOGE("AccelSensor: unknown event type (type = %d, code = %d)", event->type, event->code);
		}

		for (int j=0 ; count && mPendingMask && j<numSensors ; j++) {
			if (mPendingMask & (1<<j)) {
				mPendingMask &= ~(1<<j);
#if SENSORS_SIGNIFICANT_MOTION_ENABLE == 1
				if((j == SignificantMotion) && mPendingEvents[j].data[0] == 0.0f)
					enable(SENSORS_SIGNIFICANT_MOTION_HANDLE, 0, 0);
#endif
				if (mEnabled & (1<<j)) {
					*data++ = mPendingEvents[j];
					count--;
					numEventReceived++;
				}
			}
		}

		mInputReader.next();
	}

#if FETCH_FULL_EVENT_BEFORE_RETURN
	/* if we didn't read a complete event, see if we can fill and
	try again instead of returning with nothing and redoing poll. */
	if (numEventReceived == 0 && (mEnabled > 0)) {
		n = mInputReader.fill(data_fd);
		if (n)
			goto again;
	}
#endif

	return numEventReceived;
}

bool AccelSensor::setBufferData(sensors_vec_t *value)
{
	pthread_mutex_lock(&dataMutex);
	dataBuffer.x = value->x;
	dataBuffer.y = value->y;
	dataBuffer.z = value->z;
	pthread_mutex_unlock(&dataMutex);

	return true;
}

bool AccelSensor::getBufferData(sensors_vec_t *lastBufferedValues)
{
	pthread_mutex_lock(&dataMutex);

	lastBufferedValues->x = dataBuffer.x;
	lastBufferedValues->y = dataBuffer.y;
	lastBufferedValues->z = dataBuffer.z;

#if DEBUG_ACCELEROMETER == 1
	STLOGD("AccelSensor: getBufferData got values: x:(%f),y:(%f), z:(%f).", lastBufferedValues->x, lastBufferedValues->y, lastBufferedValues->z);
#endif
	pthread_mutex_unlock(&dataMutex);

	return true;
}

#endif /* SENSORS_ACCELEROMETER_ENABLE */
