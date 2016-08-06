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

#ifndef __SAYEYE_UTIL_H__
#define __SAYEYE_UTIL_H__

#include "SayeyeManager.h"

#define SAYEYE_PREFIX "/sayeye."
#define FILE_PATH_MAX_LEN 255

class SayeyeUtil {
public:
    static int process_config(SayeyeManager *sm);
};

/* System Spec File for get or set value */
class File {
public:
    int get_value(char *value, int len);
    int set_value(const char *value);
    File(const char *file, int flags);
    ~File();
private:
    char mFile[255];
    int mFd;
    int mFlags;
};

/* Cpu Spec */
class Cpu {
public:
    int SetBootLock(const char *boot_lock);
    int SetRoomAge(const char *room_age);
    int SetCpuFreq(const char *cpu_freq);
    int GetCpuFreq(char *cpu_freq, int len);
    int GetCpuOnline(char *cpu_online, int len);
    int SetCpuGov(const char *cpu_gov);
    int SetCpuHot(const char *cpu_hot);
    Cpu();
    ~Cpu();
private:
    File *mCpu0Lock;
    File *mCpu0Gov;
    File *mRoomAge;
    File *mCpuFreq;
    File *mCpuOnline;
    File *mCpuHot;
#ifdef SUN9IW1P1
    File *mCpu4Gov;
    File *mCpu4Lock;
#endif
};

/* Gpu Spec */
class Gpu {
public:
    int GetGpuFreq(char *gpu_freq, int len);
    int SetGpuFreq(const char *gpu_freq);
    Gpu();
    ~Gpu();
private:
    File *mGpuFreq;

};

/* Dram Spec */
class Dram {
public:
    int GetDramFreq(char *dram_freq, int len);
#if defined SUN8IW5P1 || defined SUN8IW6P1 || defined SUN9IW1P1
    int SetDramScen(const char *dram_scen);
#endif
#if defined SUN50IW1P1
    int SetDramPause(const char *dram_pause);
#endif
    Dram();
    ~Dram();
private:
    File *mDramFreq;
#if defined SUN8IW5P1 || defined SUN8IW6P1 || defined SUN9IW1P1
    File *mDramScen;
#endif
#if defined SUN50IW1P1
    File *mDramPause;
#endif
};

/* Stroage Spec */
class Io {
public:
private:

};

/*  Task pro set */
class Tasks {
public:
    int SetTasks(const char *tasks);
    Tasks();
    ~Tasks();
private:
    File *mTasks;
};

/* Touch Screen Runtime Suspend set */
#if defined SUN50IW1P1
class Tp {
public:
    int SetTpSuspend(const char *stat);
    Tp();
    ~Tp();
private:
    File *mTp;
};
#endif /* Tp define end */
#endif
