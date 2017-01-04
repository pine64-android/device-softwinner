/*
 * Copyright (C) 2012 The Android Open Source Project
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

#ifndef _BDROID_BUILDCFG_H
#define _BDROID_BUILDCFG_H

#define BTM_DEF_LOCAL_NAME   "tulip-p1"

// SERVICE_CLASS:0x1A (Bit17 -Networking,Bit19 - Capturing,Bit20 -Object Transfer)
// MAJOR CLASS: COMPUTER
// MINOR CLASS: TABLET
#define BTA_DM_COD {0x1A, 0x01, 0x1C}

#define BTA_GATT_DEBUG TRUE


#define HCI_MAX_SIMUL_CMDS (1)
#define BTM_BLE_SCAN_SLOW_INT_1 (160)
#define BTM_BLE_SCAN_SLOW_WIN_1 (32)
#define BTM_MAX_VSE_CALLBACKS  (6)

#define BTM_BLE_CONN_INT_MIN_DEF     0x06
#define BTM_BLE_CONN_INT_MAX_DEF     0x0C
#define BTM_BLE_CONN_TIMEOUT_DEF     200

#define BTIF_HF_SERVICES (BTA_HSP_SERVICE_MASK)
#define BTIF_HF_SERVICE_NAMES  { BTIF_HSAG_SERVICE_NAME, NULL }

#define BTA_DISABLE_DELAY 1000 /* in milliseconds */
#define BTA_HOST_INTERLEAVE_SEARCH FALSE

/*heartbeat log define*/
#define BTPOLL_DBG FALSE
/*hci log define*/
#define BTHC_DBG FALSE
/*avdtp log define*/
#define AVDT_DEBUG FALSE
/*BT log verbose*/
#define BT_TRACE_VERBOSE FALSE
/*page timeout */
#define BTA_DM_PAGE_TIMEOUT 8192

#define BT_HCI_DEVICE_NODE_MAX_LEN 512
#endif
