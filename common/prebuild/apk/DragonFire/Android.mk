###############################################################################
# DragonFire
LOCAL_PATH := $(call my-dir)

##########################
#include $(CLEAR_VARS)
#LOCAL_MODULE := Sample
#LOCAL_MODULE_TAGS := optional
#LOCAL_CERTIFICATE := PRESIGNED
#LOCAL_MODULE_CLASS := APPS
#LOCAL_SRC_FILES := Sample.apk
#LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
#include $(BUILD_PREBUILT)

########################################
include $(CLEAR_VARS)
LOCAL_MODULE := DragonFire
LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform
LOCAL_DEX_PREOPT := false
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := DragonFire.apk
include $(BUILD_PREBUILT)

########################################
include $(CLEAR_VARS)
LOCAL_MODULE := DragonPhone
LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform
LOCAL_DEX_PREOPT := false
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := DragonPhone.apk
include $(BUILD_PREBUILT)

########################################
include $(CLEAR_VARS)
LOCAL_MODULE := DragonAging
LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform
LOCAL_DEX_PREOPT := false
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
LOCAL_SRC_FILES := DragonAging.apk
include $(BUILD_PREBUILT)

