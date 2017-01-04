/*
 * Copyright 2014 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fcntl.h>
#include <string.h>
#include "edify/expr.h"
#include "BurnBoot.h"

Value* BurnBootFn(const char *name, State * state, int argc, Expr * argv[]) {
    if (argc != 0) {
        return ErrorAbort(state, "%s() expects no args, got %d", name, argc);
    }
    burnbootInner((UpdaterInfo*)(state->cookie));
    return StringValue(strdup(""));
}

Value* AssertBootVersionFn(const char *name, State * state, int argc, Expr * argv[]) {
    if (argc != 1) {
        return ErrorAbort(state, "%s() expects 1 arg, got %d", name, argc);
    }
    char *boot;
    if (ReadArgs(state, argv, 1, &boot) < 0) return NULL;
    double ota_boot_version = atof(boot);
    // only check boot version when the boot version is configured when packing the image
    if (ota_boot_version != 0) {
        int fd=open("sys/nand_driver0/nand_debug",O_RDONLY);
        if (fd<0&&ota_boot_version==1.0)
            printf("device is boot_v%s and ota is boot_v%s ", "1.0" , boot);
        else if (fd>0&&ota_boot_version==2.0)
            printf("device is boot_v%s and ota is boot_v%s ", "2.0" , boot);
        else
        {
            char *boot_v = fd < 0 ? "1.0" : "2.0";
            if (fd>=0)
                close(fd);
            return ErrorAbort(state, "%s() device is boot_v%s , but ota is boot_v%s ", name, boot_v , boot);
        }
        if (fd>=0)
            close(fd);
    }
    free(boot);
    return StringValue(strdup("t"));
}

void Register_librecovery_updater_common() {
    RegisterFunction("burnboot", BurnBootFn);
    RegisterFunction("assert_boot_version", AssertBootVersionFn);
}
