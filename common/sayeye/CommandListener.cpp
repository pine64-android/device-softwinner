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

#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define LOG_TAG "SayeyeCmdListener"
#include <cutils/log.h>

#include <sysutils/SocketClient.h>
#include <private/android_filesystem_config.h>

#include "ResponseCode.h"
#include "CommandListener.h"
#include "SayeyeManager.h"
#include "ScenseControl.h"

#define DUMP_ARGS 0

CommandListener::CommandListener() :
                 FrameworkListener("sayeye", true) {
    registerCmd(new NormalCmd());
    registerCmd(new HomeCmd());
    registerCmd(new BootCompleteCmd());
    registerCmd(new VideoCmd());
    registerCmd(new MusicCmd());
    registerCmd(new RotateCmd());
    registerCmd(new MonitorCmd());
    registerCmd(new BenchMarkCmd());
    registerCmd(new ConfigCmd());
}

#if DUMP_ARGS
void CommandListener::dumpArgs(int argc, char **argv, int argObscure) {
    char buffer[4096];
    char *p = buffer;

    memset(buffer, 0, sizeof(buffer));
    int i;
    for (i = 0; i < argc; i++) {
        unsigned int len = strlen(argv[i]) + 1; // Account for space
        if (i == argObscure) {
            len += 2; // Account for {}
        }
        if (((p - buffer) + len) < (sizeof(buffer)-1)) {
            if (i == argObscure) {
                *p++ = '{';
                *p++ = '}';
                *p++ = ' ';
                continue;
            }
            strcpy(p, argv[i]);
            p+= strlen(argv[i]);
            if (i != (argc -1)) {
                *p++ = ' ';
            }
        }
    }
    SLOGD("%s", buffer);
}
#else
void CommandListener::dumpArgs(int /*argc*/, char ** /*argv*/, int /*argObscure*/) { }
#endif

CommandListener::NormalCmd::NormalCmd() :
                 SayeyeCommand("normal") {
}

int CommandListener::NormalCmd::runCommand(SocketClient *cli,
                                           int argc, char **argv) {
    dumpArgs(argc, argv, -1);

    if (argc < 2) {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "Missing Argument", false);
        return 0;
    }

    SayeyeManager *sm = SayeyeManager::Instance();
    int rc = -1;
    if (!strcmp(argv[1], "enter")) {
        if (argc != 2) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: normal enter", false);
            return 0;
        }
        rc = sm->SetNormal();
    } else {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined normal mode command", false);
    }

    if (rc == 0) {
        //cli->sendMsg(ResponseCode::ScenseNormalResult, NULL, false);
        sm->getBroadcaster()->sendBroadcast(ResponseCode::ScenseNormalResult,"enter normal",false);
        cli->sendMsg(ResponseCode::CommandOkay, "normal operation succeeded", false);
    } else if (rc == 1) {
        cli->sendMsg(ResponseCode::CommandOkay, "current is normal mode", false);
    } else if (rc == 2) {
        cli->sendMsg(ResponseCode::CommandOkay, "normal mode disabled", false);
    } else {
        rc = ResponseCode::convertFromErrno();
        cli->sendMsg(rc, "normal operation failed", true);
    }
    return 0;
}

CommandListener::HomeCmd::HomeCmd() :
                 SayeyeCommand("home") {
}

int CommandListener::HomeCmd::runCommand(SocketClient *cli,
                                                      int argc, char **argv) {
    dumpArgs(argc, argv, -1);

    if (argc < 2) {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "Missing Argument", false);
        return 0;
    }

    SayeyeManager *sm = SayeyeManager::Instance();
    int rc = -1;
    if (!strcmp(argv[1], "enter")) {
        if (argc != 2) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: home enter", false);
            return 0;
        }
        rc = sm->SetHome();
    } else {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined home mode command", false);
    }
    if (rc == 0) {
        //cli->sendMsg(ResponseCode::ScenseHomeResult, NULL, false);
        sm->getBroadcaster()->sendBroadcast(ResponseCode::ScenseHomeResult,"enter home",false);
        cli->sendMsg(ResponseCode::CommandOkay, "home operation succeeded", false);
    } else if (rc == 1) {
        cli->sendMsg(ResponseCode::CommandOkay, "current is home mode", false);
    } else if (rc == 2) {
        cli->sendMsg(ResponseCode::CommandOkay, "home mode disabled", false);
    } else {
        rc = ResponseCode::convertFromErrno();
        cli->sendMsg(rc, "home operation failed", true);
    }
    return 0;
}

CommandListener::BootCompleteCmd::BootCompleteCmd() :
                 SayeyeCommand("bootcomplete") {
}

int CommandListener::BootCompleteCmd::runCommand(SocketClient *cli,
                                                      int argc, char **argv) {
    dumpArgs(argc, argv, -1);

    if (argc < 2) {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "Missing Argument", false);
        return 0;
    }

    SayeyeManager *sm = SayeyeManager::Instance();
    int rc = -1;
    if (!strcmp(argv[1], "enter")) {
        if (argc != 2) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: bootcomplete enter", false);
            return 0;
        }
        rc = sm->SetBootComplete();
    } else {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined home mode command", false);
    }

    if (rc == 0) {
        //cli->sendMsg(ResponseCode::ScenseBootCompleteResult, NULL, false);
        sm->getBroadcaster()->sendBroadcast(ResponseCode::ScenseBootCompleteResult,"enter bootcomplete",false);
        cli->sendMsg(ResponseCode::CommandOkay, "bootcomplete operation succeeded", false);
    } else if(rc == 1) {
        cli->sendMsg(ResponseCode::CommandOkay, "current is bootcomplete mode", false);
    } else {
        rc = ResponseCode::convertFromErrno();
        cli->sendMsg(rc, "bootcomplete operation failed", true);
    }
    return 0;
}

CommandListener::VideoCmd::VideoCmd() :
                 SayeyeCommand("video") {
}

int CommandListener::VideoCmd::runCommand(SocketClient *cli,
                                                      int argc, char **argv) {
    dumpArgs(argc, argv, -1);

    if (argc < 2) {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "Missing Argument", false);
        return 0;
    }

    SayeyeManager *sm = SayeyeManager::Instance();
    int rc = -1;
    if (!strcmp(argv[1], "enter")) {
        if (argc != 3) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: video enter [4k] | [1080p]", false);
            return -1;
        }

        if (!strcmp(argv[2], "4k")) {
            rc = sm->SetVideo(4096);
        } else if (!strcmp(argv[2], "1080p")) {
            rc = sm->SetVideo(1080);
        } else {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined video mode command", false);
        }
    } else {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined video mode command", false);
    }

    if (rc == 0) {
        //cli->sendMsg(ResponseCode::ScenseVideoResult, NULL, false);
        sm->getBroadcaster()->sendBroadcast(ResponseCode::ScenseVideoResult,"enter video",false);
        cli->sendMsg(ResponseCode::CommandOkay, "video operation succeeded", false);
    } else if(rc == 1) {
        cli->sendMsg(ResponseCode::CommandOkay, "current is video mode", false);
    } else if (rc == 2) {
        cli->sendMsg(ResponseCode::CommandOkay, "video mode disabled", false);
    } else {
        rc = ResponseCode::convertFromErrno();
        cli->sendMsg(rc, "video operation failed", true);
    }
    return 0;
}

CommandListener::MusicCmd::MusicCmd() :
                 SayeyeCommand("music") {
}

int CommandListener::MusicCmd::runCommand(SocketClient *cli,
                                                      int argc, char **argv) {
    dumpArgs(argc, argv, -1);

    if (argc < 2) {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "Missing Argument", false);
        return 0;
    }

    SayeyeManager *sm = SayeyeManager::Instance();
    int rc = -1;
    if (!strcmp(argv[1], "enter")) {
        if (argc != 2) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: music enter", false);
            return -1;
        }
        rc = sm->SetMusic();
    } else {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined music mode command", false);
    }

    if (rc == 0) {
        //cli->sendMsg(ResponseCode::ScenseMusicResult, NULL, false);
        sm->getBroadcaster()->sendBroadcast(ResponseCode::ScenseMusicResult,"enter music",false);
        cli->sendMsg(ResponseCode::CommandOkay, "music operation succeeded", false);
    } else if(rc == 1) {
        cli->sendMsg(ResponseCode::CommandOkay, "current is music mode", false);
    } else if (rc == 2) {
        cli->sendMsg(ResponseCode::CommandOkay, "music mode disabled", false);
    } else {
        rc = ResponseCode::convertFromErrno();
        cli->sendMsg(rc, "music operation failed", true);
    }
    return 0;
}

CommandListener::MonitorCmd::MonitorCmd() :
                 SayeyeCommand("monitor") {
}
int CommandListener::MonitorCmd::runCommand(SocketClient *cli,
                                                      int argc, char **argv) {
    dumpArgs(argc, argv, -1);

    if (argc < 2) {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "Missing Argument", false);
        cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: monitor [cpu] | [gpu] | [io] | [dram]", false);
        return 0;
    }

    SayeyeManager *sm = SayeyeManager::Instance();
    int rc = -1;
    if (!strcmp(argv[1], "cpu")) {
        /*
         * TODO
         * get cpu freq/online-core count/temp/occu ...
         */
        if (argc != 2) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: monitor cpu", false);
            return -1;
        }
        rc = sm->SetMusic();
    } else if (!strcmp(argv[1], "gpu")) {
        /*
         * TODO
         * get gpu freq/online-core count/temp/occu ...
         */
        if (argc != 2) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: monitor gpu", false);
            return -1;
        }
        rc = sm->SetMusic();
    } else if (!strcmp(argv[1], "dram")) {
        /*
         * TODO
         * get dram freq/online-core count/temp/occu ...
         */
        if (argc != 2) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: monitor dram", false);
            return -1;
        }
        rc = sm->SetMusic();
    } else if (!strcmp(argv[1], "io")) {
        /*
         * TODO
         * get io freq/online-core count/temp/occu ...
         */
        if (argc != 2) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: monitor io", false);
            return -1;
        }
        rc = sm->SetMusic();
    } else if (!strcmp(argv[1], "tp")) {
        if (argc != 3) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: monitor tp 0 | 1", false);
            return -1;
        }
        rc = sm->SetTpSuspend(atoi(argv[2]));

    } else {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined monitor mode command", false);
    }

    if (rc == 0) {
        sm->getBroadcaster()->sendBroadcast(ResponseCode::ScenseMonitorResult,"enter monitor",false);
        cli->sendMsg(ResponseCode::CommandOkay, "monitor operation succeeded", false);
    } else if(rc == 1) {
        cli->sendMsg(ResponseCode::CommandOkay, "current is monitor mode", false);
    } else if (rc == 2) {
        cli->sendMsg(ResponseCode::CommandOkay, "monitor mode disabled", false);
    } else {
        rc = ResponseCode::convertFromErrno();
        cli->sendMsg(rc, "monitor operation failed", true);
    }
    return 0;
}

CommandListener::RotateCmd::RotateCmd() :
                 SayeyeCommand("rotate") {
}
int CommandListener::RotateCmd::runCommand(SocketClient *cli,
                                                      int argc, char **argv) {
    dumpArgs(argc, argv, -1);

    if (argc < 2) {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "Missing Argument", false);
        cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: rotate enter", false);
        return 0;
    }

    SayeyeManager *sm = SayeyeManager::Instance();
    int rc = -1;
    if (!strcmp(argv[1], "enter")) {
        if (argc != 2) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: rotate enter", false);
            return -1;
        }
        rc = sm->SetRotate();
    } else {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined rotate mode command", false);
    }

    if (rc == 0) {
        //cli->sendMsg(ResponseCode::ScenseRotateResult, NULL, false);
        sm->getBroadcaster()->sendBroadcast(ResponseCode::ScenseRotateResult,"enter rotate",false);
        cli->sendMsg(ResponseCode::CommandOkay, "rotate operation succeeded", false);
    } else if(rc == 1) {
        cli->sendMsg(ResponseCode::CommandOkay, "current is rotate mode", false);
    } else if (rc == 2) {
        cli->sendMsg(ResponseCode::CommandOkay, "rotate mode disabled", false);
    } else {
        rc = ResponseCode::convertFromErrno();
        cli->sendMsg(rc, "rotate operation failed", true);
    }
    return 0;
}

CommandListener::BenchMarkCmd::BenchMarkCmd() :
                 SayeyeCommand("benchmark") {
}
int CommandListener::BenchMarkCmd::runCommand(SocketClient *cli,
                                                      int argc, char **argv) {
    dumpArgs(argc, argv, -1);

    if (argc < 2) {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "Missing Argument", false);
        cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: benchmark enter [package name] [activity name] [pid] [tags]", false);
        return 0;
    }

    SayeyeManager *sm = SayeyeManager::Instance();
    int rc = -1;
    if (!strcmp(argv[1], "enter")) {
        if (argc != 6) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: benchmark enter [package name] [activity name] [pid] [tags]", false);
            return -1;
        }
        rc = sm->SetBenchMark(argv[2], argv[3], argv[4], atoi(argv[5]));
    } else {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined benchmark mode command", false);
    }

    if (rc == 0) {
        //cli->sendMsg(ResponseCode::ScenseBenchmarkResult, NULL, false);
        sm->getBroadcaster()->sendBroadcast(ResponseCode::ScenseBenchmarkResult,"enter benchmark",false);
        cli->sendMsg(ResponseCode::CommandOkay, "benchmark operation succeeded", false);
    } else if(rc == 1) {
        /*
         * TODO
         * benchmark mode need re-enter
         */
        cli->sendMsg(ResponseCode::CommandOkay, "current is benchmark mode", false);
    } else if (rc == 2) {
        cli->sendMsg(ResponseCode::CommandOkay, "benchmark mode disabled", false);
    } else {
        rc = ResponseCode::convertFromErrno();
        cli->sendMsg(rc, "benchmark operation failed", true);
    }
    return 0;
}

CommandListener::ConfigCmd::ConfigCmd() :
                 SayeyeCommand("config") {
}
int CommandListener::ConfigCmd::runCommand(SocketClient *cli,
                                                      int argc, char **argv) {
    dumpArgs(argc, argv, -1);

    /*
    if ((cli->getUid() != 0) && (cli->getUid() != AID_SYSTEM)) {
        cli->sendMsg(ResponseCode::CommandNoPermission, "No permission to run fstrim commands", false);
        return 0;
    }
    */

    if (argc < 2) {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "Missing Argument", false);
        return 0;
    }

    SayeyeManager *sm = SayeyeManager::Instance();
    int rc = -1;
    int flags = sm->GetSwitch();
    if (!strcmp(argv[1], "home")) {
        if (argc != 3) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: config home [enable] | [disable]", false);
            return -1;
        }
        if (!strcmp(argv[2], "enable")) {
            rc = sm->SetSwitch(flags | ScenseControl::HOME);
        } else if (!strcmp(argv[2], "disable")) {
            rc = sm->SetSwitch(flags & ~ScenseControl::HOME);
        } else {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined command", false);
        }
    } else if (!strcmp(argv[1], "normal")) {
        if (argc != 3) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: config normal [enable] | [disable]", false);
            return -1;
        }
        if (!strcmp(argv[2], "enable")) {
            rc = sm->SetSwitch(flags | ScenseControl::NORMAL);
        } else if (!strcmp(argv[2], "disable")) {
            rc = sm->SetSwitch(flags | ~ScenseControl::NORMAL);
        } else {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined command", false);
        }
    } else if (!strcmp(argv[1], "video")) {
        if (argc != 3) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: config video [enable] | [disable]", false);
            return -1;
        }
        if (!strcmp(argv[2], "enable")) {
            rc = sm->SetSwitch(flags | ScenseControl::VIDEO);
        } else if (!strcmp(argv[2], "disable")) {
            rc = sm->SetSwitch(flags | ~ScenseControl::VIDEO);
        } else {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined command", false);
        }
    } else if (!strcmp(argv[1], "music")) {
        if (argc != 3) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: config music [enable] | [disable]", false);
            return -1;
        }
        if (!strcmp(argv[2], "enable")) {
            rc = sm->SetSwitch(flags | ScenseControl::MUSIC);
        } else if (!strcmp(argv[2], "disable")) {
            rc = sm->SetSwitch(flags & ~ScenseControl::MUSIC);
        } else {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined command", false);
        }
    } else if (!strcmp(argv[1], "rotate")) {
        if (argc != 3) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: config rotate [enable] | [disable]", false);
            return -1;
        }
        if (!strcmp(argv[2], "enable")) {
            rc = sm->SetSwitch(flags | ScenseControl::ROTATE);
        } else if (!strcmp(argv[2], "disable")) {
            rc = sm->SetSwitch(flags & ~ScenseControl::ROTATE);
        } else {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined command", false);
        }
    } else if (!strcmp(argv[1], "benchmark")) {
        if (argc != 3) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: config benchmark [enable] | [disable]", false);
            return -1;
        }
        if (!strcmp(argv[2], "enable")) {
            rc = sm->SetSwitch(flags | ScenseControl::BENCHMARK);
        } else if (!strcmp(argv[2], "disable")) {
            rc = sm->SetSwitch(flags & ~ScenseControl::BENCHMARK);
        } else {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined command", false);
        }
    } else if (!strcmp(argv[1], "disable")) {
        if (argc != 2) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: config disable", false);
            return -1;
        }
        flags = ScenseControl::NORMAL |
                ScenseControl::BOOTCOMPLETE |
                ScenseControl::HOME |
                ScenseControl::VIDEO |
                ScenseControl::MUSIC |
                ScenseControl::MONITOR |
                ScenseControl::ROTATE |
                ScenseControl::BENCHMARK;
        rc = sm->SetSwitch(~flags);
    } else if (!strcmp(argv[1], "enable")) {
        if (argc != 2) {
            cli->sendMsg(ResponseCode::CommandSyntaxError, "Usage: config enable", false);
            return -1;
        }
        flags = ScenseControl::NORMAL |
                ScenseControl::BOOTCOMPLETE |
                ScenseControl::HOME |
                ScenseControl::VIDEO |
                ScenseControl::MUSIC |
                ScenseControl::MONITOR |
                ScenseControl::ROTATE |
                ScenseControl::BENCHMARK;
        rc = sm->SetSwitch(flags);
    } else {
        cli->sendMsg(ResponseCode::CommandSyntaxError, "un-defined command", false);
    }

    if (rc == 0) {
        cli->sendMsg(ResponseCode::CommandOkay, "config operation succeeded", false);
    } else {
        rc = ResponseCode::convertFromErrno();
        cli->sendMsg(rc, "config operation failed", true);
        return -1;
    }
    return 0;
}
