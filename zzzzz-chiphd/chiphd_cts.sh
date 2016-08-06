#!/bin/bash
############################################################
## cts sign img file
############################################################
##制作密钥
function make__SecurityKeys()
{
	local ANDROID_DIR=$(gettop)
	if [ "$DEVICE" -a -d "$DEVICE" -a "$ANDROID_DIR" -a -d "$ANDROID_DIR" ]; then
		cd ${ANDROID_DIR}/build/tools/
		source mkkey.sh platform
		source mkkey.sh media
		source mkkey.sh releasekey
		source mkkey.sh shared
		DeviceSecurityDir=$DEVICE/security
		if [ ! -d "$DeviceSecurityDir" ]; then
			mkdir $DeviceSecurityDir
		fi
		if [ "`ls $DeviceSecurityDir/*.pem`" -a "`ls $DeviceSecurityDir/*.pk8`" ]; then
			echo "key-files existed"
		else
			cp *.pem $DeviceSecurityDir && echo "cp *.pem to $DeviceSecurityDir"
			cp *.pk8 $DeviceSecurityDir && echo "cp *.pk8 to $DeviceSecurityDir"
		fi
		rm *.pem *.pk8
		cd -
	fi
}


##调用私有签名函数
##私有签名函数需定义在$DEVICE/vendorsetup.sh,格式为function Sing_xxxxx(), 成功必须返回0
##xxxxx为add_lunch_combo xxxxx-user定义
## $1="--check" for check function, default for sign
function SignMyAndroidFiles()
{
	local ANDROID_DIR=$(gettop)
	local DeviceDir=$DIVECE
	
	if [ ! "$DEVICE" ]; then
		echo "not lunch ?"
	fi
	
	local LunchName=$(grep "add_lunch_combo .*-user" $DEVICE/vendorsetup.sh | sed 's/add_lunch_combo \([^-]*\)-user/\1/')
	local SignLunchName=$(grep "function\s*Sign_.*()" $DEVICE/vendorsetup.sh | grep "$LunchName")

	if [ "$SignLunchName" ]; then
		##获取私有签名函数并调用
		Local SignFunction=$(grep "function\s*Sign_.*()" $DEVICE/vendorsetup.sh | sed 's/function\s*\([^(]*\)(.*/\1/')
		echo "this SignFunction is $SignFunction"
		if [ "$1" = "--check" -a "$SignFunction" ]; then
			return 0
		fi
		if [ "$OUT" -a -d "$OUT" -a -d out/dist -a "$ANDROID_DIR" -a -d "$ANDROID_DIR" ]; then
			$SignFunction && return 0
		fi
	else
		echo "do you finish Sign-function Sign_$LunchName in $DEVICE/vendorsetup.sh ?"
	fi

	return 1
}

##打包签名文件
function sign_img_pack()
{
#	IsMakeKeys="$1"  ####not need invoke make__SecurityKeys in this function, invoke it in shell
	IsGetUBoot="-b" ##"S1"  # when modify sys_config, need get_uboot, now get_uboot every time

	local ThisPWd=${PWD}
  MyAndroidDir=$(gettop)
  
	##检查是否在android目录调用
	if [ "$MyAndroidDir" -a -d "$MyAndroidDir" ]; then
		if [ "$ThisPWd" == "$MyAndroidDir" ]; then
			echo "check android dir ok"
		else
			if [ "`echo "$MyAndroidDir" | grep "$ThisPWd"`" -a -d $ThisPWd/device/softwinner -a -d $ThisPWd/frameworks ]; then
				echo "check android dir ok"
			else
				echo "should invoke in $MyAndroidDir"
				cd $MyAndroidDir    ## can cd -
				return 1
			fi
		fi
	else
		echo "check android dir fail"
		return 1
	fi

	if [ "$OUT" -a -d "$OUT" ]; then
		echo "would use $OUT"
	else
		echo "Do you lunch ?"
		return 1
	fi

	##检查是否实现签名函数
	SignMyAndroidFiles --check || return 1 ##(echo "SignMyAndroidFiles" && return 1)

#### make keys
#	if [ "$IsMakeKeys" == "--newkey" ]; then
#		make__SecurityKeys
#	fi

#### get uboot
	if [ -f $OUT/system.img ]; then
		if [ "$IsGetUBoot" == "-b" -o ! -d "$OUT/bootloader" ]; then
        	echo "do : get_uboot" && get_uboot 
		else
			echo "-----------    not get_uboot     -----------------"
		fi
	else
		echo "do you make android ?"
		return 1
	fi


####make dist && sign files
	if [ -d $MyAndroidDir/out/dist ]; then
		rm -rf ${MyAndroidDir}/out/dist
	fi
	make -j16 dist
	if [ "$?" = "0" ]; then
		SignMyAndroidFiles
	else
		echo "make -j16 dist error"
		return 1
	fi
	if [ "$?" = "0" ]; then
		echo "----------------sign files ooooooooooook----------"
	else
		echo "SignMyFiles error"
		return 1
	fi

####get .img files
	#./build/tools/releasetools/ota_from_target_files out/dist/signed-target-files.zip out/dist/sined-ota.zip
	./build/tools/releasetools/img_from_target_files out/dist/signed-target-files.zip  out/dist/signed-img.zip
	mkdir out/dist/signed-img && unzip out/dist/signed-img.zip -d out/dist/signed-img
	cp  out/dist/signed-img/*.img $OUT/ && echo "done : make__signedImg"

####最终打包固件
	pack
} 
#############################################################
## end for this script file
#############################################################
