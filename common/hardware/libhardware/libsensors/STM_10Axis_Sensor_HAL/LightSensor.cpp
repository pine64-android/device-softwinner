/*
 * Copyright (C) 2011 Freescale Semiconductor Inc.
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
#include <cutils/log.h>
#include <cutils/properties.h>

#include "LightSensor.h"
#define LIGSENSOR_DATA_NAME  "jsa1127"
#define PATH  "/sys/class/input/input7"
/*****************************************************************************/
LightSensor::LightSensor()
        : SensorBase(NULL, LIGSENSOR_DATA_NAME),
        mEnabled(0),
        mInputReader(4),
        mThresholdLux(10),
		mHasPendingEvent(false),
        mPendingMask(0),
        mPreviousLight(0.0)
{
        char  buffer[PROPERTY_VALUE_MAX];
        memset(&mPendingEvent, 0, sizeof(mPendingEvent));
    
        mPendingEvent.version = sizeof(sensors_event_t);
        mPendingEvent.sensor = ID_LIGHT;
        mPendingEvent.type = SENSOR_TYPE_LIGHT;
        memset(mPendingEvent.data, 0, sizeof(mPendingEvent.data));

        property_get("ro.lightsensor.threshold", buffer, "10");
        mThresholdLux = atoi(buffer);
        
#ifdef SENSOR_DEBUG        
        ALOGD("%s:data_fd:%d\n", __func__,data_fd);
#endif

}

LightSensor::~LightSensor() {
        if (mEnabled) {
                enable(SENSORS_LIGHT_HANDLE, 0,0);
        }
}

int LightSensor::set_sysfs_input_attr(char *class_path,const char *attr, char *value, int len)
{
	char path[256];
	int fd;

	if (class_path == NULL || *class_path == '\0'
	    || attr == NULL || value == NULL || len < 1) {
		return -EINVAL;
	}
	
	snprintf(path, sizeof(path), "%s/%s", class_path, attr);
	path[sizeof(path) - 1] = '\0';
	fd = open(path, O_RDWR);
	
	if (fd < 0) {
		ALOGD("fd=%d,Could not open (write-only) SysFs attribute \"%s\" (%s).", fd,path, strerror(errno));
		close(fd);
		return -errno;
	}
	
	if (write(fd, value, len) < 0) {  
	        ALOGD("path:%s", path);     
	        ALOGD("Could not write SysFs attribute \"%s\" (%s).", attr, strerror(errno));
		close(fd);
		return -errno;
	}
	
	close(fd);

	return 0;
}


int LightSensor::setDelay(int32_t handle, int64_t ns)
{

	if (ns > 10240000000LL) {
		ns = 10240000000LL; /* maximum delay in nano second. */
	}
	if (ns < 312500LL) {
		ns = 312500LL; /* minimum delay in nano second. */
	}

        char buf[80];
        int bytes = sprintf(buf, "%lld", ns/1000 / 1000);
        
  
        int err = set_sysfs_input_attr(PATH,"delay",buf,bytes);


        return 0;
}

int LightSensor::enable(int32_t handle, int en, int type)
{
        char buf[2];  
        int err = -1;     
        int flags = en ? 1 : 0;
        int clockid = CLOCK_BOOTTIME;

        if (flags != mEnabled) {
	        int bytes = sprintf(buf, "%d", flags);	
	        err = set_sysfs_input_attr(PATH,"enable",buf,bytes);
	        mEnabled = flags;
	}

	if (!ioctl(data_fd, EVIOCSCLOCKID, &clockid))
		ALOGE("LightSensor : set EVIOCSCLOCKID = %d\n",clockid);
	else
		ALOGE("LightSensor : set EVIOCSCLOCKID failed ! \n");

	
	return 0;
}

bool LightSensor::hasPendingEvents() const
{
	return mHasPendingEvent;
}

#if 0
int LightSensor::setIntLux()
{
        FILE *fd = NULL;
        char buf[6];
        int n, lux, int_ht_lux, int_lt_lux;

        /* Read current lux value firstly, then change Delta value */
        strcpy(&ls_sysfs_path[ls_sysfs_path_len], "lux");
        if ((fd = fopen(ls_sysfs_path, "r")) == NULL) {
                ALOGE("Unable to open %s\n", ls_sysfs_path);
                return -1;
        }
        memset(buf, 0, 6);
        
        if ((n = fread(buf, 1, 6, fd)) < 0) {
                ALOGE("Unable to read %s\n", ls_sysfs_path);
	        return -1;
        }
        fclose(fd);

        lux = atoi(buf);
        int_ht_lux = lux + mThresholdLux;
        int_lt_lux = lux - mThresholdLux;
        DEBUG("Current light is %d lux\n", lux);

        /* Set low lux and high interrupt lux for polling */
        strcpy(&ls_sysfs_path[ls_sysfs_path_len], "int_lt_lux");
        
        fd = fopen(ls_sysfs_path, "r+");
        if (fd) {
                memset(buf, 0, 6);
                snprintf(buf, 6, "%d", int_lt_lux);
                n = fwrite(buf, 1, 6, fd);
                fclose(fd);
        } else
                ALOGE("Couldn't open %s file\n", ls_sysfs_path);
                
        strcpy(&ls_sysfs_path[ls_sysfs_path_len], "int_ht_lux");
        fd = fopen(ls_sysfs_path, "r+");
        
        if (fd) {
                memset(buf, 0, 6);
                snprintf(buf, 6, "%d", int_ht_lux);
                n = fwrite(buf, 1, 6, fd);
                fclose(fd);
        } else
                ALOGE("Couldn't open %s file\n", ls_sysfs_path);

        return 0;
        }
#endif

int LightSensor::setFullScale(int32_t handle, int value)
{
	int err = -1;
	return err;
}
int LightSensor::readEvents(sensors_event_t* data, int count)
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
        
        while (count && mInputReader.readEvent(&event)) {
                int type = event->type;
                
                if ((type == EV_ABS) || (type == EV_REL) || (type == EV_KEY)) {
                        processEvent(event->code, event->value);
                        mInputReader.next();
                } else if (type == EV_SYN) {
                        int64_t time = timevalToNano(event->time);
                        
			if (mPendingMask) {
				mPendingMask = 0;
				mPendingEvent.timestamp = time;
				
				if (mEnabled) {
					*data++ = mPendingEvent;
					count--;
					numEventReceived++;
					mPreviousLight = mPendingEvent.light;
				}				
			}
			
                        if (!mPendingMask) {
                                mInputReader.next();
                        }
                        
                } else {
                        ALOGE("AccelSensor: unknown event (type=%d, code=%d)",
                                type, event->code);
                        mInputReader.next();
                }
        }

        return numEventReceived;
}

void LightSensor::processEvent(int code, int value) {

        //if (code == EVENT_TYPE_LIGHT) {
                mPendingMask = 1;
                mPendingEvent.light = value;
                
        #ifdef SENSOR_DEBUG        
                ALOGD("light value: %f\n", mPendingEvent.light);
        #endif
                //setIntLux();
        //}
        
        
}


