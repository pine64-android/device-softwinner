/*
 * Copyright (C) 2009 The Android Open Source Project
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

#include <linux/input.h>

#include "common.h"
#include "device.h"
#include "screen_ui.h"

#define KEY_VOLUMEDOWN          139 // linux 114
#define KEY_VOLUMEUP            114 // linux 115
#define KEY_POWER               116
// to fix the key defined above
// we can see linux key from linux-3.4/include/linux/input.h
#define KEY_MENU                115 // linux 139

static const char* HEADERS[] = { "Volume up/down to move highlight;",
                                 "enter button to select.",
                                 "",
                                 NULL };

static const char* ITEMS[] =  {"reboot system now",
                               "apply update from ADB",
                               "wipe data/factory reset",
                               "wipe cache partition",
                               "apply update from external storage",
                               "apply update from extsd storage",
                               "apply update from usbhost storage",
                               "reboot to bootloader",
                               "power down",
                               "view recovery logs",
                               NULL };

class DefaultUI : public ScreenRecoveryUI {
  public:
    virtual KeyAction CheckKey(int key) {
        if (key == KEY_HOME) {
            return TOGGLE;
        }
        return ENQUEUE;
    }
};

class DefaultDevice : public Device {
  public:
    DefaultDevice() :
        ui(new DefaultUI) {
    }

    RecoveryUI* GetUI() { return ui; }

    int HandleMenuKey(int key, int visible) {
        if (visible) {
            switch (key) {
              case KEY_DOWN:
              case KEY_VOLUMEDOWN:
                return kHighlightDown;

              case KEY_UP:
              case KEY_VOLUMEUP:
                return kHighlightUp;

              case KEY_ENTER:
              case KEY_POWER:
                return kInvokeItem;
            }
        }

        return kNoAction;
    }

    BuiltinAction InvokeMenuItem(int menu_position) {
        switch (menu_position) {
          case 0: return REBOOT;
          case 1: return APPLY_ADB_SIDELOAD;
          case 2: return WIPE_DATA;
          case 3: return WIPE_CACHE;
          case 4: return APPLY_EXT;
          case 5: return APPLY_TF;
          case 6: return APPLY_USB;
          case 7: return REBOOT_BOOTLOADER;
          case 8: return SHUTDOWN;
          case 9: return READ_RECOVERY_LASTLOG;
          default: return NO_ACTION;
        }
    }

    const char* const* GetMenuHeaders() { return HEADERS; }
    const char* const* GetMenuItems() { return ITEMS; }

    const char* GetExternalStoragePath() { return "/data/media/0"; }
    const char* GetExtsdStoragePath() { return "/extsd"; }
    const char* GetUsbhostStoragePath() { return "/usbhost"; }
  private:
    RecoveryUI* ui;
};

Device* make_device() {
    return new DefaultDevice();
}
