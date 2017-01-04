#!/bin/bash
############################################################
## SDK info
############################################################
####获取android版本

android_top=$(gettop)
eagle_tvdsettings=$android_top/vendor/tvd/packages/TvdSettings
mars_tvdsettings=$android_top/device/softwinner/fiber-common/prebuild/packages/TvdSettings
suger_tvdsettings=$android_top/device/softwinner/common/packages/TvdSettings

function chiphd_get_android_ver() {
	AndroidTop=$(gettop)
	if [ ! "$AndroidTop" ]; then AndroidTop=. ; fi
	A_VERSION=`grep -r "PLATFORM_VERSION := [4-9]" ${AndroidTop}/build/core/version_defaults.mk | sed 's/^.*=//' | sed "s/\s*//g"`
	if [ $A_VERSION ]; then
		echo ${A_VERSION:0:3} #取前2位数字
	else
		unset A_VERSION
		#show_vit "cannot find android PLATFORM_VERSION"
	fi
}

####是否intel的sdk
function is_rockchip_SDK() {
	AndroidTop=$(gettop)
	if [ ! "$AndroidTop" ]; then AndroidTop=. ; fi
	if [ -d "${AndroidTop}/device/rockchip" ]; then
		echo true
	else
		echo false
	fi
}

####是否intel的sdk
function is_intel_SDK() {
	AndroidTop=$(gettop)
	if [ ! "$AndroidTop" ]; then AndroidTop=. ; fi
	if [ -d "${AndroidTop}/device/intel" ]; then
		echo true
	else
		echo false
	fi
}

function is_box_SDK
{
	AndroidTop=$(gettop)
	if [ ! "$AndroidTop" ]; then AndroidTop=. ; fi
	if [ -d $eagle_tvdsettings -o -d $mars_tvdsettings -o -d $suger_tvdsettings ];then
		echo true
	else
		echo false
	fi
}
####获取sdk原厂版本
function chiphd_get_allwinner_ver() {
	if [ "$1" -a -d "$1" ]; then
		grep "^\s*[^#]ro.product.firmware=" $1/*.mk | sed 's/^.*=\s*//' | sed 's/\s*\\//' | sed 's/\r//'
	else
		grep "^\s*[^#]ro.product.firmware=" $DEVICE/*.mk | sed 's/^.*=\s*//' | sed 's/\s*\\//' | sed 's/\r//'
	fi
}

####获取intel平台名,必须android目录下调用
function chiphd_get_intel_plamform_name() {
	local tmpDevice=`find device/intel -name device.mk | awk -F/ 'NR==1 { print $3 }'`
	if [ -d device/intel/${tmpDevice} ]; then
		echo ${tmpDevice}
	else
		echo "unknow"
	fi
}

####获取芯片信息
function chiphd_get_chip_type() {
	XINWU_VENDOR_TOP=$(gettop)/device/softwinner
	WinnerChipNameStrSet="crane nuclear fiber wing polaris octopus"
#	WinnerChipStrSet="a10 a13 a31 a20"

if [ -d "$XINWU_VENDOR_TOP" ]; then
	
	kk=a10
	for ii in $WinnerChipNameStrSet
	do
		if [ -d $XINWU_VENDOR_TOP/${ii}-common ]; then
			kk=${ii}
			break;
		fi
	done
	
	#echo $kk
	thisResult=a31
	thisChipCfgDir=tools/pack/chips/sun6i/configs/android
	
	case $kk in
		"crane")
			thisChipCfgDir=tools/pack/chips/sun4i/configs/crane
			thisResult=a10
		;;
		"nuclear")
			thisChipCfgDir=tools/pack/chips/sun5i/configs/android
			thisResult=a13
		;;
		"fiber")
			thisChipCfgDir=tools/pack/chips/sun6i/configs/android
			thisResult=a31
		;;
		"wing")
			thisChipCfgDir=tools/pack/chips/sun7i/configs/android
			thisResult=a20
		;;
		"polaris")
			thisChipCfgDir=tools/pack/chips/sun8iw3p1/configs/android
			thisResult=a23
		;;
		"octopus")
			thisChipCfgDir=tools/pack/chips/sun8iw6p1/configs
			thisResult=a83
		;;
		*)
			thisChipCfgDir=unknow
			thisResult=unknow
		;;
	esac ####end case

	if [ "$thisResult" == "a31" ]; then
		if [ -d $XINWU_VENDOR_TOP/fiber-a31s-evb ]; then
			thisResult=a31s
		else
			#下面判断依赖用户命名的sdk目录名
			thisAVer=`grep -r "PLATFORM_VERSION := 4" $(gettop)/build/core/version_defaults.mk | sed 's/^.*= //' | sed 's/..$//'`
			if [ "$thisAVer" -a "`echo $thisAVer | grep 4.[2-9] 2>/dev/null`" ]; then
				thisTop=$(gettop) && thisTop=${thisTop%/*} &&	thisTop=${thisTop##*/} 
				if [ "`echo $thisTop | grep [aA]31[sS] 2>/dev/null`" ]; then
					thisResult=a31s
				fi
			fi
		fi
	fi

	if [ "$thisResult" == "a20" ]; then
		if [ -d $XINWU_VENDOR_TOP/sugar-evb ]; then
			thisResult=sugar-a20
		fi
		if [ -d $XINWU_VENDOR_TOP/wing-k70 ]; then
			winnerVer=$(chiphd_get_allwinner_ver $XINWU_VENDOR_TOP/wing-k70)
			if [ "`echo "$winnerVer" | grep v2`" ]; then
				thisResult=a20-v2
			fi
		fi
	fi

	if [ "$thisResult" == "a23" ]; then
		if [ -d $XINWU_VENDOR_TOP/astar-y3 ]; then
			thisResult=a33
		fi
	fi
else
	XINWU_VENDOR_TOP=$(gettop)/device/intel
	if [ -d "${XINWU_VENDOR_TOP}/baytrail" -o -d "${XINWU_VENDOR_TOP}/cherrytrail" ]; then
		thisResult=intel
	else
		thisResult=unknow
	fi
fi

	if [ "CfgDir" = "$1" ]; then
		echo $thisChipCfgDir
	else
		echo $thisResult
	fi
}
#############################################################
## end for this script file
#############################################################
