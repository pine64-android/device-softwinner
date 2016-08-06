
#include "CameraDebug.h"
#if DBG_CAMERA_FACTORY
#define LOG_NDEBUG 0
#endif
#define LOG_TAG "HALCameraFactory"
#include <cutils/log.h>

#include <binder/IPCThreadState.h>
#include <cutils/properties.h>

#include "HALCameraFactory.h"
#include "CCameraConfig.h"

extern camera_module_t HAL_MODULE_INFO_SYM;

/* A global instance of HALCameraFactory is statically instantiated and
 * initialized when camera emulation HAL is loaded.
 */
android::HALCameraFactory  gEmulatedCameraFactory;

namespace android {


#define GET_CALLING_PID	(IPCThreadState::self()->getCallingPid())

void getCallingProcessName(char *name)
{
	char proc_node[128];

	if (name == 0)
	{
		LOGE("error in params");
		return;
	}
	
	memset(proc_node, 0, sizeof(proc_node));
	sprintf(proc_node, "/proc/%d/cmdline", GET_CALLING_PID);
	int fp = ::open(proc_node, O_RDONLY);
	if (fp > 0) 
	{
		memset(name, 0, 128);
		::read(fp, name, 128);
		::close(fp);
		fp = 0;
		LOGD("Calling process is: %s", name);
	}
	else 
	{
		LOGE("Obtain calling process failed");
	}
}

HALCameraFactory::HALCameraFactory()
        : mHardwareCameras(NULL),
          mAttachedCamerasNum(0),
          mRemovableCamerasNum(0),
          mConstructedOK(false)
{
	F_LOG;

	LOGD("camera hal version: %s", CAMERA_HAL_VERSION);

	memset(&mHalCameraInfo,0,sizeof(mHalCameraInfo));

    /* Make sure that array is allocated. */
    if (mHardwareCameras == NULL) {
        mHardwareCameras = new CameraHardware*[MAX_NUM_OF_CAMERAS];
        if (mHardwareCameras == NULL) {
            LOGE("%s: Unable to allocate V4L2Camera array for %d entries",
                 __FUNCTION__, MAX_NUM_OF_CAMERAS);
            return;
        }
        memset(mHardwareCameras, 0, MAX_NUM_OF_CAMERAS * sizeof(CameraHardware*));
    }

    /* Create the cameras */
	for (int id = 0; id < MAX_NUM_OF_CAMERAS; id++)
	{
		// camera config information
		mCameraConfig[id] = new CCameraConfig(id);
		if(mCameraConfig[id] == 0)
		{
			LOGW("create CCameraConfig failed");
		}
		else
		{
			mCameraConfig[id]->initParameters();
			mCameraConfig[id]->dumpParameters();
		}
	
		mHardwareCameras[id] = new CameraHardware(&HAL_MODULE_INFO_SYM.common, mCameraConfig[id]);
		if (mHardwareCameras[id] == NULL)
		{
	        mHardwareCameras--;
	        LOGE("%s: Unable to instantiate fake camera class", __FUNCTION__);
			return;
	    }
	}

	// check camera cfg
	if (mCameraConfig[0] != NULL)
	{
		mAttachedCamerasNum = mCameraConfig[0]->numberOfCamera();

		if ((mAttachedCamerasNum == 2)
			&& (mCameraConfig[1] == NULL))
		{
			return;
		}
	}

    mConstructedOK = true;
}

HALCameraFactory::~HALCameraFactory()
{
	F_LOG;
    if (mHardwareCameras != NULL) 
	{
        for (int n = 0; n < MAX_NUM_OF_CAMERAS; n++) 
		{
            if (mHardwareCameras[n] != NULL) 
			{
                delete mHardwareCameras[n];
				mHardwareCameras[n] = NULL;
            }
        }
        delete[] mHardwareCameras;
		mHardwareCameras = NULL;
    }

	for (int n = 0; n < MAX_NUM_OF_CAMERAS; n++) 
	{
		if (mCameraConfig[n] != NULL)
		{
			delete mCameraConfig[n];
			mCameraConfig[n] = NULL;
		}
    }
}

/****************************************************************************
 * Camera HAL API handlers.
 *
 * Each handler simply verifies existence of an appropriate CameraHardware
 * instance, and dispatches the call to that instance.
 *
 ***************************************************************************/

int HALCameraFactory::getCameraHardwareNum()
{
	int ret = 0;
	char dev_node[16];
	int orientation = 0;

	if (mCameraConfig[0] != NULL)
	{
		orientation = mCameraConfig[0]->getCameraOrientation();
	}
	
	mRemovableCamerasNum = 0;
	
	// there are two attached cameras.
	if (mAttachedCamerasNum == 2)
	{
		for (int i = 0; i < MAX_NUM_OF_CAMERAS; i++)
		{
			strcpy(mHalCameraInfo[i].device_name, mCameraConfig[i]->cameraDevice());
			mHalCameraInfo[i].device_id		= mCameraConfig[i]->getDeviceID();
			mHalCameraInfo[i].facing		= mCameraConfig[i]->cameraFacing();
			mHalCameraInfo[i].orientation	= mCameraConfig[i]->getCameraOrientation();
			mHalCameraInfo[i].fast_picture_mode	= mCameraConfig[i]->supportFastPictureMode();
			mHalCameraInfo[i].is_uvc		= false;
		}
	}
	else if (mAttachedCamerasNum == 1)
	{
		// There is one attached cameras.
		strcpy(mHalCameraInfo[0].device_name, mCameraConfig[0]->cameraDevice());
		mHalCameraInfo[0].device_id		= mCameraConfig[0]->getDeviceID();
		mHalCameraInfo[0].facing		= mCameraConfig[0]->cameraFacing();
		mHalCameraInfo[0].orientation	= mCameraConfig[0]->getCameraOrientation();
		mHalCameraInfo[0].fast_picture_mode	= mCameraConfig[0]->supportFastPictureMode();
		mHalCameraInfo[0].is_uvc		= false;

		// There may be another USB camera. Then, the device node would be "/dev/video0" or "/dev/video1".
		if (!strcmp(mHalCameraInfo[0].device_name, "/dev/video1"))
		{
			sprintf(dev_node, "/dev/video0");
		}
		else
		{
			sprintf(dev_node, "/dev/video1");
		}
		ret = access(dev_node, F_OK);
		if(ret == 0)
		{
			mRemovableCamerasNum = 1;
			strcpy(mHalCameraInfo[1].device_name, dev_node);
			mHalCameraInfo[1].device_id		= 0;
			mHalCameraInfo[1].facing		= (mCameraConfig[0]->cameraFacing() == CAMERA_FACING_BACK) ? CAMERA_FACING_FRONT : CAMERA_FACING_BACK;
			mHalCameraInfo[1].orientation	= orientation;
			mHalCameraInfo[1].fast_picture_mode	= false;
			mHalCameraInfo[1].is_uvc		= true;
		}
	}
	else if (mAttachedCamerasNum == 0)
	{
		// There is not attached cameras.
		for (int i = 0; i < MAX_NUM_OF_CAMERAS; i++)
		{
			sprintf(dev_node, "/dev/video%d", i);
			ret = access(dev_node, F_OK);
			if(ret == 0)
			{
				strcpy(mHalCameraInfo[mRemovableCamerasNum].device_name, dev_node);
				mHalCameraInfo[mRemovableCamerasNum].device_id = i;
				mRemovableCamerasNum++;
			}
		}
		
		if (mRemovableCamerasNum == 1)
		{
			mHalCameraInfo[0].facing		= CAMERA_FACING_FRONT;
			mHalCameraInfo[0].orientation	= orientation;
			mHalCameraInfo[0].fast_picture_mode	= false;
			mHalCameraInfo[0].is_uvc		= true;
		}
		else if(mRemovableCamerasNum == 2)
		{
			mHalCameraInfo[0].facing		= CAMERA_FACING_BACK;
			mHalCameraInfo[0].orientation	= orientation;
			mHalCameraInfo[0].fast_picture_mode	= false;
			mHalCameraInfo[0].is_uvc		= true;
			mHalCameraInfo[1].facing		= CAMERA_FACING_FRONT;
			mHalCameraInfo[1].orientation	= orientation;
			mHalCameraInfo[1].fast_picture_mode	= false;
			mHalCameraInfo[1].is_uvc		= true;
		}
	}
	else
	{
		LOGE("error number of attached cameras: %d in camera.cfg", mAttachedCamerasNum);
		return 0;
	}

	LOGD("There is %d attached cameras and %d removable cameras", mAttachedCamerasNum, mRemovableCamerasNum);

	// total numbers include attached and removable cameras
	return mAttachedCamerasNum + mRemovableCamerasNum;
}

int HALCameraFactory::getCameraInfo(int camera_id, struct camera_info* info)
{
    //LOGV("%s: id = %d", __FUNCTION__, camera_id);

	int total_num_of_cameras = mAttachedCamerasNum + mRemovableCamerasNum;
	char calling_process[256];

    if (!isConstructedOK()) {
        LOGE("%s: HALCameraFactory has failed to initialize", __FUNCTION__);
        return -EINVAL;
    }

    if (camera_id < 0 || camera_id >= total_num_of_cameras) {
        LOGE("%s: Camera id %d is out of bounds (%d)",
             __FUNCTION__, camera_id, total_num_of_cameras);
        return -EINVAL;
    }

	info->orientation	= mHalCameraInfo[camera_id].orientation;
	info->facing		= mHalCameraInfo[camera_id].facing;
	info->static_camera_characteristics = NULL;

	// single camera
	if (total_num_of_cameras == 1)
	{
		getCallingProcessName(calling_process);
		if ((strcmp(calling_process, "com.tencent.mobileqq") == 0)
			|| (strcmp(calling_process, "com.tencent.mobileqq:video") == 0))
		{
			// cts, mobile qq need facing-back camera
			info->facing = CAMERA_FACING_BACK;
		}
		else if ((strcmp(calling_process, "com.google.android.talk") == 0)
			|| (strcmp(calling_process, "com.android.facelock") == 0))
		{
			// gtalk, facelock need facing-front camera
			info->facing = CAMERA_FACING_FRONT;
		}
	}

	char property[PROPERTY_VALUE_MAX];
	if (property_get("ro.sf.hwrotation", property, NULL) > 0) 
	{
        //displayOrientation
        switch (atoi(property)) 
		{
        case 270:
            if(info->facing == CAMERA_FACING_BACK)
			{
			    info->orientation = (info->orientation + 90) % 360;
			} 
			else if(info->facing == CAMERA_FACING_FRONT)
			{
			    info->orientation = (info->orientation + 270) % 360;
			} 
            break;
        }
    }
	
    return NO_ERROR;
}

int HALCameraFactory::cameraDeviceOpen(int camera_id, hw_device_t** device)
{
    LOGV("%s: id = %d", __FUNCTION__, camera_id);

    *device = NULL;

    if (!isConstructedOK()) {
        LOGE("%s: HALCameraFactory has failed to initialize", __FUNCTION__);
        return -EINVAL;
    }

    if (camera_id < 0 || camera_id >= (mAttachedCamerasNum + mRemovableCamerasNum)) {
        LOGE("%s: Camera id %d is out of bounds (%d)",
             __FUNCTION__, camera_id, mAttachedCamerasNum + mRemovableCamerasNum);
        return -EINVAL;
    }

	if (!mHardwareCameras[0]->isCameraIdle()
		|| !mHardwareCameras[1]->isCameraIdle())
	{
		LOGW("camera device is busy, wait a moment");
		usleep(500000);
	}

	mHardwareCameras[camera_id]->setCameraHardwareInfo(&mHalCameraInfo[camera_id]);

	if (mHardwareCameras[camera_id]->connectCamera(device) != NO_ERROR)
	{
		LOGE("%s: Unable to connect camera", __FUNCTION__);
		return -EUSERS;
	}
	
	if (mHardwareCameras[camera_id]->Initialize() != NO_ERROR) 
	{
		LOGE("%s: Unable to Initialize camera", __FUNCTION__);
		return -EINVAL;
	}
	
    return NO_ERROR;
}

/****************************************************************************
 * Camera HAL API callbacks.
 ***************************************************************************/

int HALCameraFactory::get_number_of_cameras(void)
{
    return gEmulatedCameraFactory.getCameraHardwareNum();
}

int HALCameraFactory::get_camera_info(int camera_id,
                                           struct camera_info* info)
{
    return gEmulatedCameraFactory.getCameraInfo(camera_id, info);
}

int HALCameraFactory::device_open(const hw_module_t* module,
                                       const char* name,
                                       hw_device_t** device)
{
    /*
     * Simply verify the parameters, and dispatch the call inside the
     * HALCameraFactory instance.
     */

    if (module != &HAL_MODULE_INFO_SYM.common) {
        LOGE("%s: Invalid module %p expected %p",
             __FUNCTION__, module, &HAL_MODULE_INFO_SYM.common);
        return -EINVAL;
    }
    if (name == NULL) {
        LOGE("%s: NULL name is not expected here", __FUNCTION__);
        return -EINVAL;
    }

    return gEmulatedCameraFactory.cameraDeviceOpen(atoi(name), device);
}

/********************************************************************************
 * Initializer for the static member structure.
 *******************************************************************************/

/* Entry point for camera HAL API. */
struct hw_module_methods_t HALCameraFactory::mCameraModuleMethods = {
    open: HALCameraFactory::device_open
};

}; /* namespace android */

camera_module_t HAL_MODULE_INFO_SYM = {
    common: {
         tag:           		HARDWARE_MODULE_TAG,
		 module_api_version:	CAMERA_DEVICE_API_VERSION_1_0,
		 hal_api_version:	 	HARDWARE_HAL_API_VERSION,
         id:            		CAMERA_HARDWARE_MODULE_ID,
         name:          		"V4L2Camera Module",
         author:        		"XSJ",
         methods:       		&android::HALCameraFactory::mCameraModuleMethods,
         dso:           		NULL,
         reserved:      		{0},
    },
    get_number_of_cameras:  	android::HALCameraFactory::get_number_of_cameras,
    get_camera_info:        	android::HALCameraFactory::get_camera_info,
};

