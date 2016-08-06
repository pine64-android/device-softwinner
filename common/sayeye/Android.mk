LOCAL_PATH:= $(call my-dir)

common_src_files := \
       SayeyeManager.cpp \
       CommandListener.cpp \
       SayeyeCommand.cpp \
       ResponseCode.cpp \
       SayeyeUtil.cpp \
       ScenseControl.cpp \
       Normal.cpp \
       Home.cpp \
       BootComplete.cpp \
       Video.cpp \
       Music.cpp \
       Rotate.cpp \
       Monitor.cpp \
       BenchMark.cpp \

common_c_includes := \
       external/stlport/stlport \
       bionic \
       frameworks/native/include \

common_shared_libraries := \
       libsysutils \
       libstlport \
       libcutils \
       libbinder \
       liblog \
       libhardware_legacy \
       liblogwrap \
       libutils

common_static_libraries :=

include $(CLEAR_VARS)

LOCAL_MODULE := libsayeye

ifeq ($(TARGET_BOARD_PLATFORM), kylin)
LOCAL_CFLAGS   += -DSUN9IW1P1
endif
ifeq ($(TARGET_BOARD_PLATFORM), astar)
LOCAL_CFLAGS   += -DSUN8IW5P1
endif
ifeq ($(TARGET_BOARD_PLATFORM), octopus)
LOCAL_CFLAGS   += -DSUN8IW6P1
endif

ifeq ($(TARGET_BOARD_PLATFORM), tulip)
LOCAL_CFLAGS   += -DSUN50IW1P1
endif

LOCAL_SRC_FILES := $(common_src_files)

LOCAL_C_INCLUDES := $(common_c_includes)

LOCAL_SHARED_LIBRARIES := $(common_shared_libraries)

LOCAL_STATIC_LIBRARIES := $(common_static_libraries)

LOCAL_MODULE_TAGS := eng tests

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE:= sayeye

ifeq ($(TARGET_BOARD_PLATFORM), kylin)
LOCAL_CFLAGS   += -DSUN9IW1P1
endif
ifeq ($(TARGET_BOARD_PLATFORM), astar)
LOCAL_CFLAGS   += -DSUN8IW5P1
endif
ifeq ($(TARGET_BOARD_PLATFORM), octopus)
LOCAL_CFLAGS   += -DSUN8IW6P1
endif
ifeq ($(TARGET_BOARD_PLATFORM), tulip)
LOCAL_CFLAGS   += -DSUN50IW1P1
endif

LOCAL_SRC_FILES := \
       sayeye.cpp \
       $(common_src_files)

LOCAL_C_INCLUDES := $(common_c_includes)

LOCAL_CFLAGS += -Werror=format

LOCAL_SHARED_LIBRARIES := $(common_shared_libraries)

LOCAL_STATIC_LIBRARIES := $(common_static_libraries)

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= sdc.c

LOCAL_MODULE:= sdc

LOCAL_C_INCLUDES :=

LOCAL_SHARED_LIBRARIES := libcutils

include $(BUILD_EXECUTABLE)
