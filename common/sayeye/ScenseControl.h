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

#ifndef __SCENSE_CONTROL_H__
#define __SCENSE_CONTROL_H__

#include <pthread.h>

#include <utils/List.h>
#include <sysutils/SocketListener.h>

#include "SayeyeUtil.h"

/*NOTICE:maybe a bug */
#define PACKAGE_NAME_LEN 64
#define ACTIVITY_NAME_LEN 64

class ScenseControl {
private:
    static ScenseControl *sInstance;
private:
    /* restore current scense */
    int mScense;
    /* restore top applaction pid */
    char *mPid;
    /* restore top applaction package name */
    char *mPackageName;
    /* restore top applaction's current activity name */
    char *mActivityName;
    /* current scense tags */
    int mTags;
    /*
     * restore current applaction type
     * fill by appcheck lib
     * */
    int mType;

    bool mChanged;
    /* files operate */
    Cpu *mCpu;
    Gpu *mGpu;
    Dram *mDram;
    Io *mIo;
    Tasks *mTasks;
#if defined SUN50IW1P1
    Tp *mTp;
#endif

public:
    static const int NORMAL         = 0x00000001;
    static const int HOME           = 0x00000002;
    static const int BOOTCOMPLETE   = 0x00000004;
    static const int VIDEO          = 0x00000008;
    static const int MUSIC          = 0x00000010;
    static const int MONITOR        = 0x00000020;
    static const int ROTATE         = 0x00000040;
    static const int BENCHMARK      = 0x00000080;

    int ScenseChange();

    int SetScense(int scense);
    int GetScense();

    int SetTags(int tags);
    int GetTags();

    int SetPid(const char *pid);
    const char *GetPid();

    int SetPackageName(const char *package_name);
    const char *GetPackageName();

    int SetActivityName(const char *activity_name);
    const char *GetActivityName();

    const char *ToString(const int scense);

    static ScenseControl *Instance();
private:
    ScenseControl();
    ~ScenseControl();
};

#endif
