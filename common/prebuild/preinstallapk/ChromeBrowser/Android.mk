###############################################################################
# DragonFire
LOCAL_PATH := $(call my-dir)

##########################
#include $(CLEAR_VARS)
#LOCAL_MODULE := Sample
#LOCAL_MODULE_TAGS := optional
#LOCAL_CERTIFICATE := PRESIGNED
#LOCAL_MODULE_PATH := $(TARGET_OUT)/preinstall
#LOCAL_MODULE_PATH := $(TARGET_OUT)/precopy
#LOCAL_MODULE_CLASS := APPS
#LOCAL_SRC_FILES := Sample.apk
#LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
#include $(BUILD_PREBUILT)

########################################
include $(CLEAR_VARS)
LOCAL_MODULE := ChromeBrowser
LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := PRESIGNED
LOCAL_MODULE_PATH := $(TARGET_OUT)/preinstall
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := Chrome.apk
include $(BUILD_PREBUILT)

