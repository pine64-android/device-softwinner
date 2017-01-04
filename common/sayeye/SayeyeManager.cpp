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

#include "SayeyeManager.h"
#include "ScenseControl.h"
#include "Normal.h"
#include "Home.h"
#include "BootComplete.h"
#include "Music.h"
#include "Video.h"
#include "BenchMark.h"
#include "Monitor.h"
#include "Rotate.h"

SayeyeManager *SayeyeManager::sInstance = NULL;
SayeyeManager *SayeyeManager::Instance() {
    if (!sInstance)
        sInstance = new SayeyeManager();
    return sInstance;
}

SayeyeManager::SayeyeManager() {
    mDebug = false;
    mSwitch = ScenseControl::NORMAL |
              ScenseControl::BOOTCOMPLETE |
              ScenseControl::HOME |
              ScenseControl::VIDEO |
              ScenseControl::MUSIC |
              ScenseControl::MONITOR |
              ScenseControl::ROTATE |
              ScenseControl::BENCHMARK;
}

SayeyeManager::~SayeyeManager() {
}

void SayeyeManager::SetDebug(bool enable) {
    mDebug = enable;
}

/* Normal mode operation */
int SayeyeManager::SetNormal()
{
    if ((mSwitch & ScenseControl::NORMAL) == 0) {
        return 2;
    }
    return Normal::SetNormal();
}

/* Home mode operation */
int SayeyeManager::SetHome()
{
    if ((mSwitch & ScenseControl::HOME) == 0) {
        return 2;
    }
    return Home::SetHome();
}

/* BootComplete mode operation */
int SayeyeManager::SetBootComplete()
{
    return BootComplete::SetBootComplete();
}

int SayeyeManager::SetSwitch(int flags)
{
    if (mDebug)
        SLOGD("flags switch = %x", flags);
    mSwitch = flags;
    return 0;
}

int SayeyeManager::GetSwitch()
{
    return mSwitch;
}

/* video mode */
int SayeyeManager::SetVideo(int tags)
{
    if ((mSwitch & ScenseControl::VIDEO) == 0) {
        return 2;
    }
    return Video::SetVideo(tags);
}

/* music mode */
int SayeyeManager::SetMusic()
{
    if ((mSwitch & ScenseControl::MUSIC) == 0) {
        return 2;
    }
    return Music::SetMusic();
}

/* rotate mode */
int SayeyeManager::SetRotate()
{
    if ((mSwitch & ScenseControl::ROTATE) == 0) {
        return 2;
    }
    return Rotate::SetRotate();
}

/* benchmark mode */
int SayeyeManager::SetBenchMark(char *pkg_name, char *aty_name, char *pid, int tags)
{
    return BenchMark::SetBenchMark(pkg_name, aty_name, pid, tags);
}

int SayeyeManager::SetTpSuspend(int tag)
{
    return Monitor::SetTpSuspend(tag);
}

int SayeyeManager::start() {
    return 0;
}

int SayeyeManager::stop() {
    return 0;
}
