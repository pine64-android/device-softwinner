#!/bin/bash

####获取android版本
function chiphd_get_android_ver() {
	if [ "$1" -a -d $1 ]; then
		AndroidTop=$1
	else
		AndroidTop=$(gettop)
	fi
	A_VERSION=`grep -r "PLATFORM_VERSION := " ${AndroidTop}/build/core/version_defaults.mk | sed 's/^.*=//' | sed "s/\s*//g"`
	if [ "$A_VERSION" ]; then
		echo ${A_VERSION:0:3} #取前2位数字
	else
		unset A_VERSION
		#show_vit "cannot find android PLATFORM_VERSION"
	fi
}

####下载公共配置
function download_softwinner_chiphd_device()
{
	local thisAndroidTop=$1
	local SoftwinnerDir=${thisAndroidTop}/device/softwinner
	
	## 检查是哪个sdk
	local SDKName=`cd ${SoftwinnerDir} && echo *-common`
	SDKName=`echo $SDKName | sed "s/ .*//"`
	if [ -d $SoftwinnerDir/${SDKName} ]; then
		SDKName=${SDKName%-common}
	else
		echo "$SDKName error"
		return 0
	fi
	if [ "$SDKName" ]; then
		if [ "`echo $SDKName | grep astar  | grep kylin | grep octopus`"  ]; then
			SDKName=astar
		fi
	fi
	local ChipName=
	case $SDKName in
		crane)
			ChipName=a10
		;;
		nuclear)
			ChipName=a13
		;;
		fiber)
			ChipName=a31s
		;;
		wing)
			ChipName=a20
		;;
		octopus)
			ChipName=a83
		;;
		polaris)
			ChipName=a23
			if [ -d $SoftwinnerDir/astar-y3 ]; then
				ChipName=a33
				SDKName=astar
			fi
		;;
		astar)
			ChipName=a33
		;;
		eagle)
			ChipName=h8
		;;
		*)
			ChipName=unknow
		;;
	esac ####end case

	#android version
	local AVersion=$(chiphd_get_android_ver $thisAndroidTop) && AVersion=${AVersion:0:3}
	
	## server config
	local CfgServerIp=192.168.1.20
	local CfgTopPath=/git_repo/chiphd_devices
	
	echo $ChipName-$AVersion
	if [ ! -d ${SoftwinnerDir}/${SDKName}-chiphd ]; then
		git clone ssh://git@${CfgServerIp}${CfgTopPath}/${ChipName}/${AVersion}/${SDKName}-chiphd.git ${SoftwinnerDir}/${SDKName}-chiphd -o exdroid
	else
		echo "warning : exist ${SoftwinnerDir}/${SDKName}-chiphd"
	fi
}

function download_softwinner_chiphd_device_by_manifest_xml()
{
	local thisAndroidTop=$1
	local SoftwinnerDir=${thisAndroidTop}/device/softwinner

	## 检查是哪个sdk
	local ICName=""
	local ChipName=""
	ChipName=`grep 'fetch="/git_repo/' ${thisAndroidTop}/.repo/manifest.xml | sed 's%.*fetch="/git_repo/%%' | sed 's%/.*%%'`
	if [ "$ChipName" ]; then
		#(由于历史原因)改为小写a开头
		ChipName=`echo $ChipName | sed "s/^A/a/"`
		#(由于历史原因)改为小写h开头
		ChipName=`echo $ChipName | sed "s/^H/h/"`
		case $ChipName in
			a10) ICName=crane
			;;
			a13) ICName=nuclear
			;;
			a31s) ICName=fiber
			;;
			a20) ICName=wing
			;;
			a83) ICName=octopus
			;;
			a23) ICName=polaris
			;;
			a33) ICName=astar
			;;
			a64) ICName=tulip
			;;
			h8) ICName=eagle
			;;
			*) ICName=unknow
			;;
			esac ####end case
	else
		echo "error : get chipname from .repo/manifest.xml  fail"
		return 1
	fi

	#android version
	local AVersion=$(chiphd_get_android_ver $thisAndroidTop) && AVersion=${AVersion:0:3}

	## server config
	local CfgServerIp=192.168.1.20
	local CfgTopPath=/git_repo/chiphd_devices

	echo $ChipName-$AVersion
	if [ ! -d ${SoftwinnerDir}/${ICName}-chiphd ]; then
		git clone ssh://git@${CfgServerIp}${CfgTopPath}/${ChipName}/${AVersion}/${ICName}-chiphd.git ${SoftwinnerDir}/${ICName}-chiphd -o exdroid
	else
		echo "warning : exist ${SoftwinnerDir}/${ICName}-chiphd"
	fi
}

function download_intel_chiphd_device()
{
	local thisAndroidTop=$1
	local IntelDir=${thisAndroidTop}/device/intel
	#android version
	local AVersion=$(chiphd_get_android_ver $thisAndroidTop) && AVersion=${AVersion:0:3}
	## server config
	local CfgServerIp=192.168.1.20
	local CfgTopPath=/git_repo/intel/chiphd
	local ICName=
	if [ -f "${thisAndroidTop}/.repo/manifest.xml" ]; then
		RemoteRepoPath=`grep '/git_repo/' ${thisAndroidTop}/.repo/manifest.xml | sed 's:^.*/git_repo/intel/::' | sed 's/".*//'`
		if [ "$RemoteRepoPath" ]; then
			case $RemoteRepoPath in
				baytrail/android-4.2)
					ICName=Z3735DE
				;;
				z3735DE/4.4/android)
					ICName=Z3735DE
				;;
				z3735FG/4.4/4/android)
					ICName=Z3735FG
				;;
				z3735FG/*)
					ICName=Z3735FG
				;;
				cht/*)
					ICName=Z8300
				;;
				*)
					ICName=unknow
				;;
			esac ####end case
		fi
	fi

	if [ "$ICName" = "unknow" ]; then
		echo "unknow intel ic"
	else
		if [ ! -d ${thisAndroidTop}/../chiphdDevices ]; then
			git clone ssh://git@${CfgServerIp}${CfgTopPath}/${AVersion}/${ICName}/chiphdDevices.git ${thisAndroidTop}/../chiphdDevices -o exdroid
		else
			echo "warning : exist ${thisAndroidTop}/../chiphdDevices"
		fi
	fi
}


function download_rockchip_chiphd_device()
{
	local thisAndroidTop=$1
	local thisPlatform=rockchip
	local IntelDir=${thisAndroidTop}/device/${thisPlatform}
	#android version
	local AVersion=$(chiphd_get_android_ver $thisAndroidTop) && AVersion=${AVersion:0:3}
	## server config
	local CfgServerIp=192.168.1.20
	local CfgTopPath=/git_repo/${thisPlatform}/chiphd
	local ICName=
	if [ -f "${thisAndroidTop}/.repo/manifest.xml" ]; then
		RemoteRepoPath=`grep '/git_repo/' ${thisAndroidTop}/.repo/manifest.xml | sed "s:^.*/git_repo/${thisPlatform}/::" | sed 's/".*//'`
		if [ "$RemoteRepoPath" ]; then
			case $RemoteRepoPath in
				SOFIA3GR/*)
					ICName=SOFIA3GR
				;;
				*)
					ICName=unknow
				;;
			esac ####end case
		fi
	fi

	if [ "$ICName" = "unknow" ]; then
		echo "unknow intel ic"
	else
		if [ ! -d ${thisAndroidTop}/../chiphdDevices ]; then
			git clone ssh://git@${CfgServerIp}${CfgTopPath}/${AVersion}/${ICName}/chiphdDevices.git ${thisAndroidTop}/../chiphdDevices -o exdroid
		else
			echo "warning : exist ${thisAndroidTop}/../chiphdDevices"
		fi
	fi
}

function download_chiphd_device()
{
	local CurDir=`pwd`
	local thisFile=${0#\.\/}
	local thisFileDir
	if [ "$thisFile" == "${thisFile%/*}" ]; then
		thisFileDir=$CurDir
	else
		thisFileDir=$CurDir/${thisFile%/*}
	fi
	local IC_VendorDir=${thisFileDir%/*}
	local thisAndroidTop=${IC_VendorDir%/*} && thisAndroidTop=${thisAndroidTop%/*}

#	echo "thisFileDir=$thisFileDir"
#	echo "IC_VendorDir=$IC_VendorDir"
#	echo "thisAndroidTop=$thisAndroidTop"
	if [ -d "${thisAndroidTop}/device/intel" ]; then
		download_intel_chiphd_device ${thisAndroidTop}
	elif [ -d "${thisAndroidTop}/device/rockchip" ]; then
		download_rockchip_chiphd_device ${thisAndroidTop}
	else
		download_softwinner_chiphd_device_by_manifest_xml ${thisAndroidTop}
	fi
}

download_chiphd_device

#############################################################
## end for this script file
#############################################################

