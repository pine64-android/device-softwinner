/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>

#define LOG_TAG "Sayeye"

#include "cutils/klog.h"
#include "cutils/log.h"
#include "cutils/properties.h"

#include "SayeyeManager.h"
#include "CommandListener.h"
#include "SayeyeUtil.h"

int main() {
    SayeyeManager *sm;
    CommandListener *cl;

    SLOGI("Sayeye 1.1");

    /* For when cryptfs checks and mounts an encrypted filesystem */
    klog_set_level(6);

    /* Create our singleton managers */
    if (!(sm = SayeyeManager::Instance())) {
        SLOGE("Unable to create SayeyeManager");
        exit(1);
    };

    cl = new CommandListener();
    sm->setBroadcaster((SocketListener *) cl);

    if (sm->start()) {
        SLOGE("Unable to start SayeyeManager (%s)", strerror(errno));
        exit(1);
    }

    if (SayeyeUtil::process_config(sm)) {
        SLOGE("Error reading configuration (%s)... continuing anyways", strerror(errno));
    }

    /*
     * Now that we're up, we can respond to commands
     */
    if (cl->startListener()) {
        SLOGE("Unable to start CommandListener (%s)", strerror(errno));
        exit(1);
    }

    // Eventually we'll become the monitoring thread
    while(1) {
        sleep(1000);
    }

    SLOGI("Sayeye exiting");
    exit(0);
}

