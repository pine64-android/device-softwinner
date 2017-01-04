/*
 * Copyright (C) 2014 The Android Open Source Project
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

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "BurnBoot.h"
#include "BurnNandBoot.h"
#include "BurnSdBoot.h"

static bool extractFexToTemp(ZipArchive* za, const char* path) {
    bool success = false;
    //Extract fex file to TMP_FEX_FILE_PATH
    const ZipEntry* entry = mzFindZipEntry(za, path);
    if (entry == NULL) {
        printf("no %s in package\n", path);
        goto finish_free;
    }

    FILE* f = fopen(TMP_FEX_FILE_PATH, "wb");
    if (f == NULL) {
        printf("can't open %s for write: %s\n",
                TMP_FEX_FILE_PATH, strerror(errno));
        goto finish_free;
    }
    success = mzExtractZipEntryToFile(za, entry, fileno(f));
    fclose(f);
  finish_free:
    return success;
}

static int executeBurnboot(UpdaterInfo* info, DeviceBurn burnFunc, const char* fex) {
    extractFexToTemp(info->package_zip, fex);

    BufferExtractCookie* cookie = malloc(sizeof(BufferExtractCookie));
    if (!getBufferExtractCookieOfFile(TMP_FEX_FILE_PATH, cookie)) {
        burnFunc(cookie);
        unlink(TMP_FEX_FILE_PATH);
    } else {
        bb_debug("open temp boot binary file failed!\n");
    }
    // free(bootexpr);
    // free(devexpr);
    free(cookie->buffer);
    free(cookie);
    return 0;
}

/************************ public interface ***********************/

int burnbootInner(UpdaterInfo* info) {
    const char* boot0_fex;
    const char* uboot_fex;
    DeviceBurn burnboot0_func;
    DeviceBurn burnuboot_func;
    int flash_type = getFlashType();
    if (flash_type == FLASH_TYPE_UNKNOW)
        return -1;
    int secure = check_soc_is_secure();
    if (secure) {
        boot0_fex = TOC0_FEX;
        uboot_fex = TOC1_FEX;
    } else {
        if (flash_type == FLASH_TYPE_NAND) {
            boot0_fex = BOOT0_NAND_FEX;
        } else {
            boot0_fex = BOOT0_EMMC_FEX;
        }
        uboot_fex = UBOOT_FEX;
    }
    if (flash_type == FLASH_TYPE_NAND) {
        burnboot0_func = burnNandBoot0;
        burnuboot_func = burnNandUboot;
    } else {
        burnboot0_func = burnSdBoot0;
        burnuboot_func = burnSdUboot;
    }
    executeBurnboot(info, burnboot0_func, boot0_fex);
    executeBurnboot(info, burnuboot_func, uboot_fex);
    return 0;
}