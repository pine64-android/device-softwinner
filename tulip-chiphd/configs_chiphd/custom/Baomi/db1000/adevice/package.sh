#!/bin/bash

####set board
thisXwDevicePath=$0
thisXwDevicePath=${thisXwDevicePath%/*}
thisXwBoard=chiphd
XwLicheeConfig=
thisChip=sun50iw1p1

if [ -f $thisXwDevicePath/configs_chiphd/do_chiphd.sh ]; then
	NowCustomFlagFile=$thisXwDevicePath/configs_chiphd/NowCustom.sh
	if [ ! -f $NowCustomFlagFile ]; then
		bash $thisXwDevicePath/configs_chiphd/do_chiphd.sh
	fi
	NowCustomCfg=`sed -n '1p' $NowCustomFlagFile`
	thisXwBoard=$(echo $NowCustomCfg  | awk -F/ '{print $(NF-2)"-"$(NF-1)}')
	XwLicheeConfig=${NowCustomCfg%/*}/lichee
	#cat $DEVICE/configs_chiphd/custom/NowCustom.sh | awk -F/ '{print $(NF-2) "/" $(NF-1)}'`
fi

#thisCfgDir=`echo $ANDROID_BUILD_TOP/../lichee/tools/pack/chips/*/configs/android`
## for a10
#if [ ! -d "$thisCfgDir" ]; then thisCfgDir=`echo $ANDROID_BUILD_TOP/../lichee/tools/pack/chips/*/configs/crane`; fi
## for a3x-4.4
#if [ ! -d "$thisCfgDir" ]; then thisCfgDir=`echo $ANDROID_BUILD_TOP/../lichee/tools/pack/chips/sun8iw5p1/configs`; fi

# if defined thisChip
thisCfgDir=`echo $ANDROID_BUILD_TOP/../lichee/tools/pack/chips/$thisChip/configs`

## copy .fex file
if [ "$XwLicheeConfig" -a -d "$XwLicheeConfig" ]; then
	#echo $thisCfgDir/$thisXwBoard
	if [ ! -d $thisCfgDir/$thisXwBoard ]; then mkdir $thisCfgDir/$thisXwBoard; fi
	cp -r --force ${XwLicheeConfig}/configs/*.* $thisCfgDir/$thisXwBoard/
	echo "done : cp -r --force ${XwLicheeConfig}/configs/*.* $thisCfgDir/$thisXwBoard/"
else
	echo "XwLicheeConfig = ${XwLicheeConfig}"
fi

#### repo tag file to system.img when $1 = --tag
XwImgAndroidTagFunc="$thisXwDevicePath/../zzzzz-chiphd/xw_img_create_android_tag.sh"
if [ -f $XwImgAndroidTagFunc ]; then
	$XwImgAndroidTagFunc $1
fi

#### prepare for rename-img-file
XwImgRenameFunc="$thisXwDevicePath/../zzzzz-chiphd/xw_img_rename.sh"
if [ -f $XwImgRenameFunc ]; then
	$XwImgRenameFunc --new && echo "auto rename img file later ..."
fi



#### rebuild system.img
if [ "$CHIPHD_NEED_REBUILD_SYS_IMG" = "true" ]; then
	echo "------------- rebuild system.img"
	make systemimage-nodeps
fi

######################### allwinner script

cd $PACKAGE

chip=sun50iw1p1
platform=android
board=${thisXwBoard}
debug=uart0
sigmode=none
securemode=none

usage()
{
	printf "Usage: pack [-cCHIP] [-pPLATFORM] [-bBOARD] [-d] [-s] [-v] [-h]
	-c CHIP (default: $chip)
	-p PLATFORM (default: $platform)
	-b BOARD (default: $board)
	-d pack firmware with debug info output to card0
	-s pack firmware with signature
	-v pack firmware with secureboot
	-h print this help message
"
}

while getopts "c:p:b:dsvh" arg
do
	case $arg in
		c)
			chip=$OPTARG
			;;
		p)
			platform=$OPTARG
			;;
		b)
			board=$OPTARG
			;;
		d)
			debug=card0
			;;
		s)
			sigmode=sig
			;;
		v)
			securemode=secure
			;;
		h)
			usage
			exit 0
			;;
		?)
			exit 1
			;;
	esac
done

./pack -c $chip -p $platform -b $board -d $debug -s $sigmode -v $securemode


#### rename
if [ -f $XwImgRenameFunc ]; then
	$XwImgRenameFunc --rename $thisCfgDir/$thisXwBoard/sys_config.fex $1
fi

