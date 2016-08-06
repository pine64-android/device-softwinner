#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

#define LOG_TAG "setmacaddr"
#include <utils/Log.h>

#include <cutils/properties.h>

#define PROPERTY_BT_BDADDR_PATH "ro.bt.bdaddr_path"

#define MAC_PATH "/data/wifimac.txt"
#define BTA_PATH "/data/btaddr.txt"
#define STATIC_MAC_PATH "/mnt/private/ULI/factory/mac.txt"
#define STATIC_BTA_PATH "/mnt/private/ULI/factory/btaddr.txt"

#define MAC_LEN 20

static int gen_randseed()
{
	int fd;
	int rc;
	unsigned int randseed;
	size_t len;
	struct timeval tval;

	len =  sizeof(randseed);
	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
	{
		ALOGD("%s: Open /dev/urandom fail\n", __FUNCTION__);
		return -1;
	}
	rc = read(fd, &randseed, len);
	close(fd);
	if(rc <0)
	{
		if (gettimeofday(&tval, (struct timezone *)0) > 0)
			randseed = (unsigned int) tval.tv_usec;
		else
			randseed = (unsigned int) time(NULL);

		ALOGD("open /dev/urandom fail, using system time for randseed\n");
	}
	return randseed;
}

/*
 * Get a Randon MAC addr, save to macaddr
 *
 */
static int get_randon_mac(char *macaddr)
{
	int randseed;
	int randvar1,randvar2;

	randseed = gen_randseed();
	if(randseed == -1)
		return -1;
	srand((unsigned int)randseed);

	randvar1 = rand();
	randvar2 = rand();
	ALOGD("%s:  randvar1 =0x%x, randvar2=0x%x",__FUNCTION__, randvar1,randvar2);

	//sprintf(macaddr, "00:e0:4c:%02x:%02x:%02x",
	//changed by tingle for 6 bytes random
	//Byte1,bit0,bit1 can't be 1.
	sprintf(macaddr, "%02x:%02x:%02x:%02x:%02x:%02x", \
			(unsigned char)((randvar2)&0xFC), \
			(unsigned char)((randvar2>>8)&0xFF), \
			(unsigned char)((randvar2>>16)&0xFF), \
			(unsigned char)((randvar1)&0xFF), \
			(unsigned char)((randvar1>>8)&0xFF), \
			(unsigned char)((randvar1>>16)&0xFF));

    ALOGD("generate a new MAC address: %s", macaddr);

    return 0;
}

/*
 * Generate randon MAC addr, save in file, backup in bakfile
 * if bakfile exist, use it
 * if bakfile not exist, generate a new randon addr
 */
static void generate_mac(char* file, char* bakfile)
{
    char macaddr[MAC_LEN];
	char cmd[128];

	ALOGD("%s: file= %s bakfile= %s",__FUNCTION__, file, bakfile);

    if (access(file, F_OK) == 0 && access(bakfile, F_OK) == 0) {
        ALOGD("both file and bakfile exist, it is ok.");
        return;
    } else if (access(bakfile, F_OK) == 0) {
        ALOGD("only bakfile exist, copy to file.");
        sprintf(cmd, "cp %s %s && chmod 644 %s", bakfile, file, file);
        ALOGD("cmd=%s", cmd);
        system(cmd);
        return;
    } else if (access(file, F_OK) == 0) {
        ALOGD("only file exist, copy to bakfile.");
        sprintf(cmd, "cp %s %s && chmod 644 %s", file, bakfile, bakfile);
        ALOGD("cmd=%s", cmd);
        system(cmd);
        return;
    }

    //or, should generate a new mac addr
    if(get_randon_mac(macaddr) != 0) {
        ALOGD("Generate a randon mac addr failed!!!");
        return;
    }
    //save mac addr to file
    sprintf(cmd, "echo %s > %s && chmod 644 %s", macaddr, file, file);
    ALOGD("cmd=%s", cmd);
    system(cmd);
    //backup to bakfile
    sprintf(cmd, "cp %s %s && chmod 644 %s", file, bakfile, bakfile);
    ALOGD("cmd=%s", cmd);
    system(cmd);
}

int main(int argc, char ** argv)
{
    char val[256];
    char cmd[128];

    if (access(STATIC_BTA_PATH, F_OK) != 0 && access(STATIC_MAC_PATH, F_OK) != 0) {
        sprintf(cmd, "mkdir -p /mnt/private/ULI/factory");
        ALOGD("cmd=%s", cmd);
        system(cmd);
    }


    if(property_get(PROPERTY_BT_BDADDR_PATH, val, NULL)) {
	    generate_mac(val, STATIC_BTA_PATH);
    } else {
	    generate_mac(BTA_PATH, STATIC_BTA_PATH);
    }

	generate_mac(MAC_PATH, STATIC_MAC_PATH);

	return 0;
}
