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

#ifndef _RESPONSECODE_H
#define _RESPONSECODE_H

class ResponseCode {
public:
    // 100 series - Requestion action was initiated; expect another reply
    // before proceeding with a new command.
    static const int ActionInitiated  = 100;


    // 200 series - Requested action has been successfully completed
    static const int CommandOkay                = 200;

    // 400 series - The command was accepted but the requested action
    // did not take place.
    static const int OperationFailed            = 400;

    // 500 series - The command was not accepted and the requested
    // action did not take place.
    static const int CommandSyntaxError         = 500;
    static const int CommandParameterError      = 501;
    static const int CommandNoPermission        = 502;

    // 600 series - Unsolicited broadcasts
    static const int UnsolicitedInformational   = 600;
    static const int ScenseNormalResult         = 610;
    static const int ScenseHomeResult           = 611;
    static const int ScenseBootCompleteResult   = 612;
    static const int ScenseVideoResult          = 613;
    static const int ScenseMusicResult          = 614;
    static const int ScenseMonitorResult        = 615;
    static const int ScenseRotateResult         = 616;
    static const int ScenseBenchmarkResult      = 617;

    static int convertFromErrno();
};
#endif
