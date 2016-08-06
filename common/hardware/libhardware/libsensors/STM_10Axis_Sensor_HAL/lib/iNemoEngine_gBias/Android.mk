# Copyright (C) 2013 STMicroelectronics
# Matteo Dameno, Denis Ciocca - Motion MEMS Product Div.
# Copyright (C) 2008 The Android Open Source Project
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

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_PREBUILT_LIBS :=  ./iNemoEngine_gBias.$(TARGET_ARCH).a
LOCAL_STATIC_LIBRARIES := iNemoEngine_gBias.$(TARGET_ARCH)
LOCAL_MODULE_TAGS := optional
include $(BUILD_MULTI_PREBUILT)

include $(CLEAR_VARS)
LOCAL_CFLAGS := -DLOG_TAG=\"Sensors\"
LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_SRC_FILES := iNemoEngine_gbias_API.c
LOCAL_MODULE :=  iNemoEngine_gBias_API
LOCAL_MODULE_TAGS := optional
LOCAL_WHOLE_STATIC_LIBRARIES :=  iNemoEngine_gBias.$(TARGET_ARCH)
include $(BUILD_STATIC_LIBRARY)

