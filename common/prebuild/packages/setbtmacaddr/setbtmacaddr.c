#include <stdio.h>
#include <stdlib.h>
#include <private/android_filesystem_config.h>
#include <time.h>
#include <fcntl.h>
#include <utils/Log.h>
#include <string.h>
#include <sys/stat.h>

#define STATIC_BTMAC_PATH "/mnt/private/ULI/factory/btmac.txt"
#define LOG_TAG "setbtmacaddr"

static void generate_btmac(char filepath[])
{
  int fd, i;
	char buf[30];
	uint8_t bt_addr[6];
	char cmd[128];

	int fd_data;
	int fd_fire;
	char mac_data[128];
	char mac_fire[128];
	

	ALOGD("Enter %s %s\n",__FUNCTION__,filepath);

	/*Check MAC_ADDR_FILE exist */

	//add for STATIC_MAC_PATH start
	if(access(STATIC_BTMAC_PATH, F_OK) == 0)
	{
		if(access(filepath, F_OK) == 0)
		{
			fd_data = open(filepath, O_RDWR, 0644);
			if( fd_data>= 0){
				memset(mac_data, 0, sizeof(mac_data));
				read(fd_data,mac_data,sizeof(mac_data));
				sprintf(mac_data, "%s",mac_data);
				
			}
			close(fd_data);

			fd_fire= open(STATIC_BTMAC_PATH, O_RDWR, 0644);		
			if( fd_fire>= 0){
				memset(mac_fire, 0, sizeof(mac_fire));			
				read(fd_fire,mac_fire,sizeof(mac_fire));
				sprintf(mac_fire, "%s",mac_fire);
			}			
			close(fd_fire);

			if(strcmp(mac_fire,mac_data)==0)
			{
			  return;
			}
		}
		ALOGD("%s: static bt mac file %s exists, use it",__FUNCTION__, STATIC_BTMAC_PATH);
		sprintf(cmd, "cat %s > %s && chmod 644 %s", STATIC_BTMAC_PATH, filepath, filepath);
		ALOGD("cmd: %s", cmd);
		system(cmd);
		ALOGD("%s: %s exists",__FUNCTION__, filepath);
		return;
	}
	//add for STATIC_BTMAC_PATH start

	srand(time(NULL) + getpid());

  memset(bt_addr, 0, sizeof(bt_addr));
  bt_addr[0] = 0x0;
  for(i=1; i<6; i++) {
  	bt_addr[i] = (uint8_t) (rand() >> 8) & 0xFF;
  }

  if(0x9e == bt_addr[3] && 0x8b == bt_addr[4] && (bt_addr[5] <= 0x3f)){
    //get random value
    bt_addr[3] = 0x00;
  }

	fd = open(filepath, O_CREAT|O_TRUNC|O_RDWR, 0644);
	if( fd >= 0)
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X\0", bt_addr[0], bt_addr[1], bt_addr[2], bt_addr[3], bt_addr[4], bt_addr[5]);
		write(fd, buf, sizeof(buf));
		close(fd);
        chown(filepath, AID_BLUETOOTH, AID_NET_BT_STACK);
        chmod(filepath, 0660);
	}
	ALOGD("%s: %s fd=%d, data=%s",__FUNCTION__, filepath, fd, buf);
	
	//add for STATIC_BTMAC_PATH start
	sprintf(cmd, " mkdir -p %s", "/mnt/private/ULI/factory");
	ALOGD("cmd: %s", cmd);
	system(cmd);
	
	fd = open(STATIC_BTMAC_PATH, O_CREAT|O_TRUNC|O_RDWR, 0644);
	if( fd >= 0)
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X\0", bt_addr[0], bt_addr[1], bt_addr[2], bt_addr[3], bt_addr[4], bt_addr[5]);
		write(fd, buf, sizeof(buf));
		close(fd);
		ALOGD("%s: %s fd=%d, data=%s",__FUNCTION__, STATIC_BTMAC_PATH, fd,buf);
	} else {
		ALOGD("%s not exist!", STATIC_BTMAC_PATH);
	}
	//add for STATIC_BTMAC_PATH start
}

int main(int argc, char ** argv)
{
	char filepath[80];
	memset(filepath, 0, sizeof(filepath));
	if(argc != 2)
	{
			ALOGD("Usage:setbtmacaddr <mac_addr_path>\n");
			return 0;
	}
	strncpy(filepath, argv[1], strlen(argv[1]));
	generate_btmac(filepath);

	return 0;
}
