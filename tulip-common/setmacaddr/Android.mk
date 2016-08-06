LOCAL_PATH:= $(call my-dir)
# RTK mac
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE    := false
LOCAL_SHARED_LIBRARIES  := libcutils
LOCAL_LDLIBS        += -Idl

LOCAL_SRC_FILES     := setmacaddr.c

LOCAL_MODULE := setmacaddr

LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)
