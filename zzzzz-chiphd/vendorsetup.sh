#!/bin/bash
####此文件只实现升级其它脚本, 然后导入其它脚本的总入口
############################################################
if [ -d device/rockchip ]; then
CHIPHD_ANDROID_SCRIPT_PATH=device/rockchip/zzzzz-chiphd
elif [ -d device/intel ]; then
CHIPHD_ANDROID_SCRIPT_PATH=device/intel/zzzzz-chiphd
else
CHIPHD_ANDROID_SCRIPT_PATH=device/softwinner/zzzzz-chiphd
fi

##保存cd目录
temp_chiphd_oooooold_pwd=$OLDPWD
temp_chiphd_nnnnnnew_pwd=$PWD

##升级函数在此实现
source $CHIPHD_ANDROID_SCRIPT_PATH/com_misc.sh

##自动升级
update-chiphd-script-auto

##其它总入口
source $CHIPHD_ANDROID_SCRIPT_PATH/chiphdsetup.sh

##恢复cd目录
#if [ -d "$temp_chiphd_oooooold_pwd" ]; then
	cd $temp_chiphd_oooooold_pwd
#fi
cd $temp_chiphd_nnnnnnew_pwd
#############################################################
## end for this script file
#############################################################