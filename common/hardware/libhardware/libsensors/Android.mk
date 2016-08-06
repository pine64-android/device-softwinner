# Copyright (C) 2010 The Android Open Source Project
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
$(warning $(SW_BOARD_USES_SENSORS_TYPE))
LOCAL_PATH := $(call my-dir)

ifneq ($(filter invensense,$(SW_BOARD_USES_SENSORS_TYPE)),)
    include $(call all-named-subdir-makefiles,invensense)
else
ifneq ($(filter lsm9ds0,$(SW_BOARD_USES_SENSORS_TYPE)),)
    include $(call all-named-subdir-makefiles,STM_10Axis_Sensor_HAL)
else
#ifneq ($(filter aw_sensors,$(SW_BOARD_USES_SENSORS_TYPE)),)
    include $(call all-named-subdir-makefiles,aw_sensors)
#else
#    $(warning $(SW_BOARD_USES_SENSORS_TYPE))
#endif
endif
endif

include $(CLEAR_VARS)


#include $(all-subdir-makefiles)
