# Copyright (C) 2012 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


LOCAL_PATH := $(call my-dir)

#########################

include $(CLEAR_VARS)
LOCAL_MODULE := libsunxi_crypto
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES_arm := arm/libsunxi_crypto.so
LOCAL_SRC_FILES_arm64 := arm64/libsunxi_crypto.so
LOCAL_MULTILIB := both
include $(BUILD_PREBUILT)

################################
include $(CLEAR_VARS)

LOCAL_MODULE := keystore.exdroid
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MULTILIB := both
LOCAL_MODULE_PATH_32 := $(TARGET_OUT)/lib/hw
LOCAL_MODULE_PATH_64 := $(TARGET_OUT)/lib64/hw
LOCAL_SRC_FILES := module_sunxi.cpp
LOCAL_C_INCLUDES := \
       system/security/keystore \
       libnativehelper/include \
       $(LOCAL_PATH)/include/schw
LOCAL_CFLAGS = -fvisibility=hidden -Wall -Werror
LOCAL_SHARED_LIBRARIES := \
       liblog \
       libkeystore_binder \
       libdl \
       libc \
       libcutils \
       libsunxi_crypto
LOCAL_MODULE_TAGS := optional
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
include $(BUILD_SHARED_LIBRARY)
