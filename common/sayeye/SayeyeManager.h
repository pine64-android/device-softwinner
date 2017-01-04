/*
 * Copyright (C) 2014 ALLWINNERTECH
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

#ifndef __SAYEYE_MANAGER_H__
#define __SAYEYE_MANAGER_H__

#include <pthread.h>

#ifdef __cplusplus
#include <utils/List.h>
#include <sysutils/SocketListener.h>

class SayeyeManager {
private:
    static SayeyeManager *sInstance;

private:
    SocketListener *mBroadcaster;

    bool mDebug;
    int mSwitch;

public:
    virtual ~SayeyeManager();

    void setBroadcaster(SocketListener *sl) { mBroadcaster = sl; }
    SocketListener *getBroadcaster() { return mBroadcaster; }
    static SayeyeManager *Instance();
    /* Normal mode operation */
    int SetNormal();
    /* Home mode operation */
    int SetHome();
    /* BootComplete  mode operation */
    int SetBootComplete();

    /* Config Switch */
    int GetSwitch();
    int SetSwitch(int flags);

    /* video mode */
    int SetVideo(int tags);

    /* music mode */
    int SetMusic();

    /* rotate mode */
    int SetRotate();

    /* benchmark mode */
    int SetBenchMark(char *pkg_name, char *aty_name, char *pid, int tags);

    /* set tp runtime suspend */
    int SetTpSuspend(int tag);
    void SetDebug(bool enable);
    int start();
    int stop();

private:
    SayeyeManager();
};

extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif

#endif
