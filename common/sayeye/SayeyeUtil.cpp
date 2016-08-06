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
#include "ScenseConfig.h"

int SayeyeUtil::process_config(SayeyeManager *sm)
{
    char sayeye_filename[PROPERTY_VALUE_MAX + sizeof(SAYEYE_PREFIX)];
    char propbuf[PROPERTY_VALUE_MAX];
    int i;
    int ret = -1;
    int flags;

    property_get("ro.hardware", propbuf, "");
    snprintf(sayeye_filename, sizeof(sayeye_filename), SAYEYE_PREFIX"%s", propbuf);

    SLOGE("failed to open %s\n", sayeye_filename);

    sm = sm;
    ret = 0;

out_fail:
    return ret;
}

File::File(const char *file, int flags)
{
    int len = strlen(file);
    mFd = -1;
    if (len < FILE_PATH_MAX_LEN)
        strncpy(mFile, file, len);
    else
        SLOGE("failed to copy file name");
    mFlags = flags;
}

File::~File()
{
    if (mFd == -1)
        return;
    close(mFd);
}

int File::get_value(char *value, int len)
{
    if (value == NULL || len < 0)
        return -1;

    if (mFd == -1) {
        mFd = open(mFile, mFlags);
        if (mFd < 0) {
            SLOGE("failed to open %s, %s", mFile, strerror(errno));
            return -1;
        }
    }

    read(mFd, value, len);
    return 0;
}

int File::set_value(const char *value)
{
    if (value == NULL)
        return -1;

    if (mFd == -1) {
        mFd = open(mFile, mFlags);
        if (mFd < 0) {
            SLOGE("failed to open %s, %s", mFile, strerror(errno));
            return -1;
        }
    }

    write(mFd, value, strlen(value));
    return 0;
}

Cpu::Cpu()
{
    mCpu0Lock = new File(CPU0LOCK, O_WRONLY);
    mCpu0Gov = new File(CPU0GOV, O_WRONLY);
#ifdef SUN9IW1P1
    mCpu4Lock = new File(CPU4LOCK, O_WRONLY);
    mCpu4Gov = new File(CPU4GOV, O_WRONLY);
#endif
    mRoomAge = new File(ROOMAGE, O_WRONLY);
    mCpuFreq = new File(CPUFREQ,O_RDWR);
    mCpuOnline = new File(CPUONLINE, O_RDONLY);
    mCpuHot = new File(CPUHOT, O_WRONLY);
}

int Cpu::SetCpuHot(const char *cpu_hot)
{
    return mCpuHot->set_value(cpu_hot);
}

int Cpu::SetBootLock(const char *boot_lock)
{
#ifdef SUN9IW1P1
    mCpu4Lock->set_value(boot_lock);
#endif
    return mCpu0Lock->set_value(boot_lock);
}

int Cpu::SetRoomAge(const char *room_age)
{
    return mRoomAge->set_value(room_age);
}

int Cpu::SetCpuFreq(const char *cpu_freq)
{
    return mCpuFreq->set_value(cpu_freq);
}

int Cpu::GetCpuFreq(char *cpu_freq, int len)
{
    return mCpuFreq->get_value(cpu_freq, len);
}

int Cpu::GetCpuOnline(char *cpu_online, int len)
{
    return mCpuOnline->get_value(cpu_online, len);
}

int Cpu::SetCpuGov(const char *cpu_gov)
{
#ifdef SUN9IW1P1
    mCpu4Gov->set_value(cpu_gov);
#endif
    return mCpu0Gov->set_value(cpu_gov);
}

Cpu::~Cpu()
{
    delete mCpu0Lock;
    delete mCpu0Gov;
    delete mRoomAge;
    delete mCpuFreq;
    delete mCpuOnline;
    delete mCpuHot;
#ifdef SUN9IW1P1
    delete mCpu4Lock;
    delete mCpu4Gov;
#endif
}

Gpu::Gpu()
{
    mGpuFreq = new File(GPUFREQ, O_RDWR);
}

int Gpu::GetGpuFreq(char *gpu_freq, int len)
{
    return mGpuFreq->get_value(gpu_freq, len);
}

int Gpu::SetGpuFreq(const char *gpu_freq)
{
    return mGpuFreq->set_value(gpu_freq);
}

Gpu::~Gpu()
{
    delete mGpuFreq;
}

Dram::Dram()
{
    mDramFreq = new File(DRAMFREQ, O_RDONLY);
#if defined SUN8IW5P1 || defined SUN8IW6P1 || defined SUN9IW1P1
    mDramScen = new File(DRAMSCEN, O_WRONLY);
#endif
#if defined SUN50IW1P1
    mDramPause = new File(DRAMPAUSE, O_WRONLY);
#endif
}

int Dram::GetDramFreq(char *dram_freq, int len)
{
    return mDramFreq->get_value(dram_freq, len);
}

#if defined SUN8IW5P1 || defined SUN8IW6P1 || defined SUN9IW1P1
int Dram::SetDramScen(const char *dram_scen)
{
    return mDramScen->set_value(dram_scen);
}
#endif

#if defined SUN50IW1P1
int Dram::SetDramPause(const char *dram_pause)
{
    return mDramPause->set_value(dram_pause);
}
#endif
Dram::~Dram()
{
    delete mDramFreq;
}

Tasks::Tasks()
{
    mTasks = new File(TASKS, O_WRONLY);
}

int Tasks::SetTasks(const char *tasks)
{
    return mTasks->set_value(tasks);
}

Tasks::~Tasks()
{
    delete mTasks;
}


/* Touch Screen Runtime Suspend set */
#if defined SUN50IW1P1
Tp::Tp()
{
    mTp = new File(TP_SUSPEND, O_WRONLY);
}

int Tp::SetTpSuspend(const char *stat)
{
    return mTp->set_value(stat);
}

Tp::~Tp()
{
    delete mTp;
}
#endif
