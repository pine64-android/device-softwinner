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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#define LOG_TAG "Sayeye"

#include <cutils/log.h>
#include <sysutils/NetlinkEvent.h>

#include "ScenseControl.h"
#include "ScenseConfig.h"

ScenseControl *ScenseControl::sInstance = NULL;

ScenseControl *ScenseControl::Instance() {
    if (!sInstance)
        sInstance = new ScenseControl();
    return sInstance;
}

ScenseControl::ScenseControl() {
    mScense = NORMAL;
    mTags = 0;
    mType = 0;
    mPid = (char *)malloc(10);
    if (mPid == NULL)
        SLOGE("Pid memory alloc failed");
    memset(mPid, 0, 10);

    mPackageName = (char *)malloc(PACKAGE_NAME_LEN);
    if (mPackageName == NULL)
        SLOGE("PackageName memory alloc failed");
    memset(mPackageName, 0, PACKAGE_NAME_LEN);

    mActivityName = (char *)malloc(ACTIVITY_NAME_LEN);
    if (mActivityName == NULL)
        SLOGE("ActivityName memory alloc failed");
    memset(mActivityName, 0, ACTIVITY_NAME_LEN);

    mChanged = false;
    mCpu = new Cpu;
    mGpu = new Gpu;
    mDram = new Dram;
    mIo = new Io;
    mTasks = new Tasks;
#if defined SUN50IW1P1
    mTp = new Tp;
#endif
}

ScenseControl::~ScenseControl() {
    delete mCpu;
    delete mGpu;
    delete mDram;
    delete mIo;
    delete mTasks;
}

const char *ScenseControl::ToString(const int scense)
{
    if (scense == BOOTCOMPLETE)
        return "BootComplete";
    else if (scense == NORMAL)
        return "Normal";
    else if (scense == HOME)
        return "Home";
    else if (scense == VIDEO)
        return "Video";
    else if (scense == MUSIC)
        return "Music";
    else if (scense == BENCHMARK)
        return "Benchmark";
    else if (scense == ROTATE)
        return "Rotate";
    else if (scense == MONITOR)
        return "Monitor";
    else
        return "Unknown-Error";
}
int ScenseControl::SetScense(int scense)
{
    if (mScense == scense) {
        //mChanged = false;
        mChanged = true;
        SLOGD("current scense is (%s)", ToString(scense));
        return 1;
    }
    SLOGD("Scense state changed form (%s) to (%s)", ToString(mScense), ToString(scense));
    mScense = scense;
    mChanged = true;
    return 0;
}

int ScenseControl::GetScense()
{
    return mScense;
}
int ScenseControl::SetTags(int tags)
{
    mTags = tags;
    return 0;
}

int ScenseControl::GetTags()
{
    return mTags;
}

int ScenseControl::SetPid(const char *pid)
{
    if (pid == NULL) {
        SLOGE("pid set error,pid is null");
        return -1;
    }
    strcpy(mPid, pid);
    return 0;
}

const char *ScenseControl::GetPid()
{
    return mPid;
}

int ScenseControl::SetPackageName(const char *package_name)
{
    if (package_name == NULL) {
        SLOGE("package name set error,package_name is null");
        return -1;
    }
    strcpy(mPackageName, package_name);
    return 0;
}

const char *ScenseControl::GetPackageName()
{
    return mPackageName;
}


int ScenseControl::SetActivityName(const char *activity_name)
{
    if (activity_name == NULL) {
        SLOGE("activity name set error,activity_name is null");
        return -1;
    }
    strcpy(mActivityName, activity_name);
    return 0;
}

const char *ScenseControl::GetActivityName()
{
    return mActivityName;
}

int ScenseControl::ScenseChange() {
    if (!mChanged)
        return 1;

    switch (mScense) {
        case NORMAL:
            mCpu->SetRoomAge(ROOMAGE_NORMAL);
#if defined SUN8IW5P1 || defined SUN8IW6P1 || defined SUN9IW1P1
            mDram->SetDramScen(DRAM_NORMAL);
#endif

#if defined SUN9IW1P1 || defined SUN8IW6P1
            mGpu->SetGpuFreq(GPU_NORMAL);
#endif

#if defined SUN50IW1P1
            mTp->SetTpSuspend("1");
            mDram->SetDramPause("0");
#endif
            break;
        case HOME:
            mCpu->SetRoomAge(ROOMAGE_NORMAL);
#if defined SUN8IW5P1 || defined SUN8IW6P1 || defined SUN9IW1P1
            mDram->SetDramScen(DRAM_HOME);
#endif

#if defined SUN9IW1P1 || defined SUN8IW6P1
            mGpu->SetGpuFreq(GPU_NORMAL);
#endif
            break;
        case BOOTCOMPLETE:
            mCpu->SetBootLock("1");
            mCpu->SetCpuGov(CPU_GOVERNOR);
            mCpu->SetCpuHot("1");
#if defined SUN50IW1P1
            mDram->SetDramPause("0");
#endif
            break;
        case VIDEO:
            if (1) {
                mCpu->SetRoomAge(ROOMAGE_NORMAL);
#if defined SUN8IW5P1 || defined SUN8IW6P1 || defined SUN9IW1P1
                mDram->SetDramScen(DRAM_LOCALVIDEO);
#endif

#if defined SUN9IW1P1 || defined SUN8IW6P1
                mGpu->SetGpuFreq(GPU_NORMAL);
#endif
            } else {
                mCpu->SetRoomAge(ROOMAGE_NORMAL);
#if defined SUN8IW5P1 || defined SUN8IW6P1 || defined SUN9IW1P1
                mDram->SetDramScen(DRAM_4KLOCALVIDEO);
#endif

#if defined SUN9IW1P1 || defined SUN8IW6P1
                mGpu->SetGpuFreq(GPU_NORMAL);
#endif
            }
            break;
        case MUSIC:
            mCpu->SetRoomAge(ROOMAGE_NORMAL);
#if defined SUN8IW5P1 || defined SUN8IW6P1 || defined SUN9IW1P1
            mDram->SetDramScen(DRAM_BGMUSIC);
#endif

#if defined SUN9IW1P1 || defined SUN8IW6P1
            mGpu->SetGpuFreq(GPU_NORMAL);
#endif

#if defined SUN50IW1P1
                mTp->SetTpSuspend("0");
#endif
            break;
        case MONITOR:
            switch (mTags) {
            case 1:
                break;
            default :
                break;
            }
            break;
        case ROTATE:
            mCpu->SetRoomAge(ROOMAGE_NORMAL);
#if defined SUN8IW5P1 || defined SUN8IW6P1 || defined SUN9IW1P1
            mDram->SetDramScen(DRAM_NORMAL);
#endif

#if defined SUN9IW1P1 || defined SUN8IW6P1
            mGpu->SetGpuFreq(GPU_NORMAL);
#endif
            break;
        case BENCHMARK:
            mTasks->SetTasks(mPid);
            mCpu->SetRoomAge(roomage_little[mTags]);
#if defined SUN8IW5P1 || defined SUN8IW6P1 || defined SUN9IW1P1
            mDram->SetDramScen(DRAM_NORMAL);
#endif
#if defined SUN50IW1P1
            mDram->SetDramPause("1");
#endif

            if (mTags == APP_TYPE_GPU || mTags == APP_TYPE_CPU_GPU) {
#if defined SUN9IW1P1 || defined SUN8IW6P1
                mGpu->SetGpuFreq(GPU_PERF);
#endif
            }
            break;
        defalut:
            break;
    }
    SLOGD("Scense (%s) state set succeed\n", ToString(mScense));
    return 0;
}
