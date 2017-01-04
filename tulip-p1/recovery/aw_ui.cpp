/*
 * Copyright (C) 2015 The Android Open Source Project
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

#include "device.h"
#include "roots.h"
#include "screen_ui.h"
#include <unistd.h>

#define AW_UP            114
#define AW_DOWN          115
#define KEY_POWER        116

#define FIRST_BOOT_FLAG "/bootloader/data.notfirstrun"

static const char* MENU_ITEMS[] = {
    "Reboot system now",
    "Reboot to bootloader",
    "Apply update from ADB",
    "Apply update from extsd",
    "Apply update from usbhost",
    "Wipe data/factory reset",
    "Wipe cache partition",
    "Mount /system",
    "View recovery logs",
    "Power off",
    NULL
};

class AwDevice : public Device {

  public:
    AwDevice(RecoveryUI* ui) :
        Device(ui) {
    }

    const char* const* GetMenuItems() {
        return MENU_ITEMS;
    }

    BuiltinAction InvokeMenuItem(int menu_position) {
      switch (menu_position) {
        case 0: return REBOOT;
        case 1: return REBOOT_BOOTLOADER;
        case 2: return APPLY_ADB_SIDELOAD;
        case 3: return APPLY_TF;
        case 4: return APPLY_USB;
        case 5: return WIPE_DATA;
        case 6: return WIPE_CACHE;
        case 7: return MOUNT_SYSTEM;
        case 8: return VIEW_RECOVERY_LOGS;
        case 9: return SHUTDOWN;
        default: return NO_ACTION;
      }
    }

    int HandleMenuKey(int key, int visible) {
      if (!visible) {
        return kNoAction;
      }

      switch (key) {
        case KEY_DOWN:
        case AW_UP:
          return kHighlightDown;

        case KEY_UP:
        case AW_DOWN:
          return kHighlightUp;

        case KEY_ENTER:
        case KEY_POWER:
          return kInvokeItem;

        default:
          return kNoAction;
      }
    }

    bool PostWipeData() {
        ensure_path_mounted(FIRST_BOOT_FLAG);
        unlink(FIRST_BOOT_FLAG);
        return true;
    }
};

Device* make_device() {
  return new AwDevice(new ScreenRecoveryUI);
}
