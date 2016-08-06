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

#ifndef _COMMAND_LISTENER_H__
#define _COMMAND_LISTENER_H__

#include <sysutils/FrameworkListener.h>
#include "SayeyeCommand.h"

/*
 * command protocol define:
 * argv[0] = [scense]
 * argv[1] = [tags]
 * argv[2] = [pid]
 * argv[3] = [package name]
 * argv[4] = [activity name]
 */

class CommandListener : public FrameworkListener {
public:
    CommandListener();
    virtual ~CommandListener() {}

private:
    static void dumpArgs(int argc, char **argv, int argObscure);

    class NormalCmd : public SayeyeCommand {
    public:
        NormalCmd();
        virtual ~NormalCmd() {}
        int runCommand(SocketClient *c, int argc, char ** argv);
    };

    class HomeCmd : public SayeyeCommand {
    public:
        HomeCmd();
        virtual ~HomeCmd() {}
        int runCommand(SocketClient *c, int argc, char ** argv);
    };

    class BootCompleteCmd : public SayeyeCommand {
    public:
        BootCompleteCmd();
        virtual ~BootCompleteCmd() {}
        int runCommand(SocketClient *c, int argc, char ** argv);
    };

    class VideoCmd : public SayeyeCommand {
    public:
        VideoCmd();
        virtual ~VideoCmd() {}
        int runCommand(SocketClient *c, int argc, char ** argv);
    };

    class MusicCmd : public SayeyeCommand {
    public:
        MusicCmd();
        virtual ~MusicCmd() {}
        int runCommand(SocketClient *c, int argc, char ** argv);
    };

    class MonitorCmd : public SayeyeCommand {
    public:
        MonitorCmd();
        virtual ~MonitorCmd() {}
        int runCommand(SocketClient *c, int argc, char ** argv);
    };

    class RotateCmd : public SayeyeCommand {
    public:
        RotateCmd();
        virtual ~RotateCmd() {}
        int runCommand(SocketClient *c, int argc, char ** argv);
    };

    class BenchMarkCmd : public SayeyeCommand {
    public:
        BenchMarkCmd();
        virtual ~BenchMarkCmd() {}
        int runCommand(SocketClient *c, int argc, char ** argv);
    };

    class ConfigCmd : public SayeyeCommand {
    public:
        ConfigCmd();
        virtual ~ConfigCmd() {}
        int runCommand(SocketClient *c, int argc, char ** argv);
    };
};

#endif
