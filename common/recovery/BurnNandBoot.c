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

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "Utils.h"
#include "BurnNandBoot.h"

#define NAND_BLKBURNBOOT0 		_IO('v',127)
#define NAND_BLKBURNUBOOT 		_IO('v',128)
#define DEVNODE_PATH_NAND   "/dev/block/by-name/bootloader"

void clearPageCache(){
    FILE *fp = fopen("/proc/sys/vm/drop_caches", "w+");
    char *num = "1";
    fwrite(num, sizeof(char), 1, fp);
    fclose(fp);
}

int burnNandBoot0(BufferExtractCookie *cookie) {

	if (checkBoot0Sum(cookie)) {
		bb_debug("wrong boot0 binary file!\n");
		return -1;
	}

	int fd = open(DEVNODE_PATH_NAND, O_RDWR);
	if (fd == -1) {
		bb_debug("open device node failed ! errno is %d : %s\n", errno, strerror(errno));
		return -1;
	}

    clearPageCache();

	int ret = ioctl(fd, NAND_BLKBURNBOOT0, (unsigned long)cookie);

	if (ret) {
		bb_debug("burnNandBoot0 failed ! errno is %d : %s\n", errno, strerror(errno));
	} else {
		bb_debug("burnNandBoot0 succeed!\n");
	}

	close(fd);
	return ret;
}

int burnNandUboot(BufferExtractCookie *cookie) {

	if (checkUbootSum(cookie) && checkBoot1Sum(cookie)) {
		bb_debug("wrong uboot binary file!\n");
		return -1;
	}

	int fd = open(DEVNODE_PATH_NAND, O_RDWR);
	if (fd == -1) {
		bb_debug("open device node failed ! errno is %d : %s\n", errno, strerror(errno));
		return -1;
	}

    clearPageCache();

	int ret = ioctl(fd,NAND_BLKBURNUBOOT, (unsigned long)cookie);

	if (ret) {
		bb_debug("burnNandUboot failed ! errno is %d : %s\n", errno, strerror(errno));
	} else {
		bb_debug("burnNandUboot succeed!\n");
	}

	close(fd);
	return ret;
}
