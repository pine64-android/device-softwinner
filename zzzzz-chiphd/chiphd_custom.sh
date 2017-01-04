#!/bin/bash

####检查是否lunch
function chiphd_check_lunch()
{
	if [ "$DEVICE" -a -d "$DEVICE"  ]; then
		echo "lunch : path $DEVICE"
	else
		echo 'not lunch'
	fi
}

#### touch c c++ file
function update_c_type_file_time_stamp()
{
	local tttDir=$1
	if [ -d "$tttDir" ]; then
		find $tttDir -name '*.[HhCc]*' | xargs touch -c
		echo "    update $tttDir"
	fi
}

#### recovery compile time stamp
function update_recovery_BoradConfig()
{
	if [ "$(chiphd_check_lunch)" == "not lunch" ]; then
		echo "not lunch"
		return 0
	else
		thisDevice=`echo $DEVICE`
	fi

	update_c_type_file_time_stamp bootable/recovery
}

#### wifi bt update compile time stamp
function update_wifi_bt_BoradConfig()
{
	if [ "$(chiphd_check_lunch)" == "not lunch" ]; then
		echo "not lunch"
		return 0
	else
		thisDevice=`echo $DEVICE`
	fi

	echo "make installclean now..." && make installclean

	#local IsBt="`grep "^\s*BOARD_HAVE_BLUETOOTH\s*:=\s*true" $thisDevice/BoardConfig.mk`"

	echo "now update wifi time stamp ..."
	update_c_type_file_time_stamp external/wpa_supplicant_8
	update_c_type_file_time_stamp hardware/libhardware_legacy

	echo "now update bluetooth time stamp ..."
#	find packages/apps/Bluetooth/ -type f | xargs touch
	update_c_type_file_time_stamp external/bluetooth
	update_c_type_file_time_stamp system/bluetooth
	update_c_type_file_time_stamp device/common/libbt
	update_c_type_file_time_stamp `echo device/softwinner/*-common/hardware/realtek`
}

#### 获取原生lunch
function xw_get_aosp_lunch_func()
{
	local thisAOSPPath=$(gettop)
	local thisAOSPSFile=build/envsetup.sh
	local start_line=$(sed -n '/^function lunch/='  $thisAOSPSFile)
	local end_line_set=""
	local end_line=""
	local end_flag=""

	if [ "$start_line" ]; then
		end_line_set=$(sed -n '/printconfig/='  $thisAOSPSFile)
		if [ "$end_line_set" ]; then
			for cur_line in $end_line_set
			do
				if [ $start_line -gt $cur_line ]; then
					continue
				else
					end_line=`expr $cur_line + 1`
					break
				fi
			done
		fi
	fi

	if [ "$end_line" ]; then
		end_flag=$(sed -n "$end_line"p $thisAOSPSFile)
	fi

	if [ "$end_flag" == "}" ]; then
		echo "function xw_o_aosp_lunch()" > $CHIPHD_ANDROID_SCRIPT_PATH/aosp_lunch.sh
		start_line=`expr $start_line + 1`
		sed -n "$start_line,$end_line"p $thisAOSPSFile >> $CHIPHD_ANDROID_SCRIPT_PATH/aosp_lunch.sh
	else
		echo "echo error_get_aosp_lunch" > $CHIPHD_ANDROID_SCRIPT_PATH/aosp_lunch.sh
	fi
}

#### 清除非受git控制的默认配置文件
function chiphd_gitclean_standard_device_cfg()
{
	local tDir=$1
	if [ ! "$tDir" ]; then
		tDir=.
	fi
	if [ -d $tDir/.git ]; then
		local OldPWD=$(pwd)
		local CleanFiles=
		cd $tDir  && CleanFiles=`git clean -dxn | awk  '$3 !~ /^configs_chiphd|^kernel|^modules/ {print $3}'`
		if [ "$CleanFiles" ]; then
			git clean -dxf  $CleanFiles #&& echo "clean standard device config"
		fi
		cd $OldPWD
	fi
}

#### checkout默认配置文件
function chiphd_checkout_standard_device_cfg()
{
	local tDir=$1
	if [ ! "$tDir" ]; then
		tDir=.
	fi
	if [ -d $tDir/.git ]; then
		local OldPWD=$(pwd)
		cd $tDir && git checkout -- `git ls-files | awk -F/ '$1 !~ /configs_chiphd/ {print $0 }'` #&& echo "checkout standard device config"
		cd $OldPWD
	fi
}

#### 恢复默认配置文件
function chiphd_recover_standard_device_cfg()
{
	local tDir=$1
	if [ ! "$tDir" ]; then
		tDir=.
	fi
	chiphd_gitclean_standard_device_cfg $tDir
	chiphd_checkout_standard_device_cfg $tDir
}

#### 获取相对路径($1相对于$2)
function chiphd-get-relative-path_2()
{
	local Dir1=$1
	local Dir2=$2
	if [ "$Dir1" -a "$Dir2" ]; then
		local ShortDir2=$(echo $Dir2 | awk -F/ '{print $(NF-1) "/" $(NF)}')
		echo $Dir1 | sed "s%^.*${ShortDir2}/%%"
	fi
}

#### git库打apply补丁
function patch-chiphd-custom()
{
	local thisPatch=$2 ##绝对路径
	local thisGitPath=$1
	local thisOperate=$3
	local retVal=0
#	echo "patch-chiphd-custom[ $thisPatch , $thisGitPath ]"
	if [ "$thisPatch" -a -f "$thisPatch" ]; then
		if [ "$thisGitPath" -a -d "$thisGitPath" -a -d "$thisGitPath/.git" ]; then
			local NowPWD=$(pwd)
			local OldPWD=$(cd - > /dev/null && pwd)

			cd $thisGitPath && echo -e "patch-chiphd-custom : \e[1;32m $thisGitPath\e[0m"
			#thisPatch=${NowPWD}/${thisPatch}
			if [ "$thisOperate" -a "$thisOperate" = "--check" ]; then
				#echo "git apply --check : $thisPatch" && git apply --check $thisPatch
				git apply --check $thisPatch
			else
				#echo "git apply --check : $thisPatch" && git apply --check $thisPatch && echo "---------- apply $thisPatch ----------" && git apply --ignore-whitespace $thisPatch
				git apply --check $thisPatch && echo "---------- apply $thisPatch ----------" && git apply --ignore-whitespace $thisPatch
			fi
			if [ "$?" = "0" ]; then
#				echo -e "\e[1;32m ok\e[0m : $thisPatch"
			:
			else
			#失败
				echo -e "\e[1;31m fail:\e[0m \e[1;33m $thisPatch\e[0m" && retVal=1
			fi
			cd $OldPWD
			cd $NowPWD
		fi
	fi
	return $retVal
}

#### 保存当前目录git库补丁
function patch-chiphd-create()
{
	local PatchSaveDirTop=$1
	if [ "$PatchSaveDirTop" -a -d "$PatchSaveDirTop" -a -d .git ]; then
		local Atop=
		if [ "$2" -a -d "$2" ]; then
			ATop=$2
		else
			ATop=$(gettop)
		fi
		## 目标路径
		local TargePath=$(chiphd-get-relative-path_2 `pwd` $ATop)
		TargePath=${PatchSaveDirTop}/${TargePath}
		if [ ! -d $TargePath ]; then
			mkdir -p $TargePath
		fi
		if [ -d $TargePath ]; then
			git add .
			git add -u .
			git diff --cached --binary > $TargePath/custum.patch && echo -e "save : \e[1;32m $TargePath/custum.patch \e[0m"
		fi
	fi
}

#### 清除git库补丁的修改
function patch-chiphd-clear()
{
	local thisGitPath=$1
	local retVal=0
#echo "patch-chiphd-clear[ $thisGitPath ]"
	if [ "$thisGitPath" -a -d "$thisGitPath" -a -d "$thisGitPath/.git" ]; then
		local NowPWD=$(pwd)
		local OldPWD=$(cd - > /dev/null && pwd)

		cd $thisGitPath && echo -e "\e[1;33m patch-chiphd-clear\e[0m : $thisGitPath"
		#local cleanFiles=`git clean -dn`
		git reset HEAD .
		git clean -df
		git checkout -- .

		cd $OldPWD
		cd $NowPWD
	fi

	return $retVal
}

#### 应用/取消客户特性补丁
function allpatch-chiphd-custom()
{
	if [ "$(chiphd_check_lunch)" == "not lunch" ]; then
		echo "not lunch"
		return 0
	else
		thisDevice=`echo $DEVICE`
	fi

	local patchTop=$1
	local NowOperate=$2
	if [ "$patchTop" -a -d "$patchTop" -a "$NowOperate" ]; then
		NowPWD=`pwd`
		allPatch=`find $patchTop -name "*.patch"`
		last_path="android"
		if [ "$allPatch" ]; then
			##遍历各补丁并操作
			for iPatch in $allPatch
			do
				APatch=${iPatch} ##绝对路径
				##相对路径
				tGitDir=$(echo ${iPatch} | sed "s%${patchTop}/%%" )
				tGitDir=${tGitDir%/*}
				#patch-chiphd${NowOperate} $tGitDir $APatch $NowOperate
				if [ $NowOperate = "--clear"  ]; then
					if [ "$last_path" = "$tGitDir" ]; then
						continue
					else
						patch-chiphd-clear $tGitDir $APatch
					fi
				else
					patch-chiphd-custom $tGitDir $APatch $NowOperate
				fi
				last_path=$tGitDir
			done
		fi
	fi
}

#检查是否存在函数
function chiphdCustomScriptCallBackCheck()
{
	CustomScriptFile=$1
	CustomScriptFunc=$2

	if [ -f "$CustomScriptFile" ]; then
		if [ "`grep  "^function $CustomScriptFunc"  $CustomScriptFile `" ]; then
			echo True
		else
			echo False
		fi
	else
		echo False
	fi
}

#检查是否存在函数及调用
function chiphdCustomScriptCallBack()
{
	CustomScriptFile=$1
	CustomScriptFunc=$2
	CustomScriptArg1=$3
	CustomScriptArg2=$4

	if [ "Test_`chiphdCustomScriptCallBackCheck $CustomScriptFile $CustomScriptFunc`" = "Test_True"  ]; then
		$CustomScriptFunc $CustomScriptArg1 $CustomScriptArg2
	fi
}

####选择项目配置
function lunch-xw()
{
	if [ "$(chiphd_check_lunch)" == "not lunch" ]; then
		echo "not lunch"
		return 0
	else
		thisDevice=`echo $DEVICE`
	fi

	local ConfigsFName=chiphd_config.sh
	local ConfigsPath=${thisDevice}/configs_chiphd
	local ProductSetTop=${ConfigsPath}/custom
##遍历所有客户方案配置
#	local ProductSet=`find $ConfigsPath -name $ConfigsFName`

	local ProductSetShort=`find $ProductSetTop -name $ConfigsFName | awk -F/ '{print $(NF-2) "/" $(NF-1)}' | sort`
	local ProductSelExitName=select/exit
	local ProductShortSelSet="$ProductSetShort $ProductSelExitName"
	
	
	local ProductSel=
	
	select MySEL in $ProductShortSelSet; do
		case $MySEL in
			"$ProductSelExitName")
				echo -e "   selected \e[1;31m$MySEL\e[0m"
				break;
			;;
			*)
				if [ "$MySEL" ]; then
					#echo "$ProductSetTop/$MySEL"
					if [ -d "$ProductSetTop/$MySEL" ]; then
						echo -e "   selected \e[1;31m$MySEL\e[0m"
						ProductSel=$MySEL
						break;
					else
						echo -e "   error selected \e[1;31m$MySEL\e[0m"
					fi
				else
					echo -e "  \e[1;31m error selected \e[0m"
				fi
			;;
		esac ####end case
	done ####end select
	
	local ProductSelPath="$ProductSetTop/$MySEL"
	if [ "$ProductSel" -a -d "$ProductSelPath" -a ! "$ProductSelPath" = "$ProductSetTop/" ]; then
		## 旧项目
		local OldProductSelPath=
		if [ -f ${ConfigsPath}/NowCustom.sh ]; then
			OldProductSelPath=$(sed -n '1p' ${ConfigsPath}/NowCustom.sh)
			OldProductSelPath=${OldProductSelPath%/*}
		fi
		## 新项目
		echo "${ProductSelPath}/$ConfigsFName" > ${ConfigsPath}/NowCustom.sh

		## 导入项目自定义脚本
		local MyCustomScript=${ProductSelPath}/${ConfigsFName}
		unset prj_chiphd_help1
		if [ "Test_`chiphdCustomScriptCallBackCheck $MyCustomScript prj_chiphd_help1`" = "Test_True"  ]; then
			echo "==== source $MyCustomScript ===="
			source $MyCustomScript
		fi

		#### 更新时间戳并拷贝到配置根目录
		local ProjectSelDir=$ProductSelPath/adevice
		if [ -d $ProjectSelDir ]; then
			Old_SW_BOARD_RECOVERY_ROTATION=0
			New_SW_BOARD_RECOVERY_ROTATION=0

			Old_WIFI_VENDOR=realtek
			New_WIFI_VENDOR=realtek
			##上一项目wifi-bt配置信息
			thisBoardConfigFile=$thisDevice/BoardConfig.mk
			if [ "`grep "^\s*BOARD_HAVE_BLUETOOTH\s*:=\s*true" $thisBoardConfigFile`" ]; then
				OldHaveBT=true
			else
				OldHaveBT=false
			fi
			##上一项目recovery ui配置信息
			if [ "`grep "^\s*SW_BOARD_RECOVERY_ROTATION\s*:=\s*" $thisBoardConfigFile`" ]; then
				Old_SW_BOARD_RECOVERY_ROTATION=`awk '$1=="SW_BOARD_RECOVERY_ROTATION" {print $3}' $thisBoardConfigFile`
			fi

			## android还原特性补丁的修改
			if [ "$OldProductSelPath" -a -d "$OldProductSelPath" ]; then
				if [ -d "${OldProductSelPath}/androidPatch" ]; then
					allpatch-chiphd-custom ${OldProductSelPath}/androidPatch --clear
				fi
			fi
			## android应用特性补丁的修改
			if [ "$ProductSelPath" -a -d "$ProductSelPath" ]; then
				if [ -d "${ProductSelPath}/androidPatch" ]; then
					allpatch-chiphd-custom ${ProductSelPath}/androidPatch --apply
				fi
			fi
			chiphd_recover_standard_device_cfg $thisDevice    ##恢复
			find $ProjectSelDir -name "*" -exec touch {} \;   ##更新时间戳
			cp -r $ProjectSelDir/* $thisDevice                ##拷贝到标准配置

			####wifi-bt编译宏处理
			if [ "`grep "^\s*BOARD_HAVE_BLUETOOTH\s*:=\s*true" $thisBoardConfigFile`" ]; then
				NewHaveBT=true
			else
				NewHaveBT=false
			fi
			##新项目recovery ui配置信息
			if [ "`grep "^\s*SW_BOARD_RECOVERY_ROTATION\s*:=\s*" $thisBoardConfigFile`" ]; then
				New_SW_BOARD_RECOVERY_ROTATION=`awk '$1=="SW_BOARD_RECOVERY_ROTATION" {print $3}' $thisBoardConfigFile`
			fi
			####更新recovery时间戳
			if [ "$New_SW_BOARD_RECOVERY_ROTATION" != "$Old_SW_BOARD_RECOVERY_ROTATION" ]; then
				update_recovery_BoradConfig
			fi
			####更新wifi,bluetooth时间戳
			if [ "$NewHaveBT" != "$OldHaveBT" ]; then
				update_wifi_bt_BoradConfig
			fi
			#### 预安装APK
			if [ -d $thisDevice/preApk ]; then
			    DoChiphdPreApk $thisDevice/preApk
			fi
			#### 自定义函数调用
			chiphdCustomScriptCallBack $MyCustomScript prj_chiphd_help1 ${thisDevice} ${ProductSelPath}
		fi
	fi
}

#### 保存当前目录git库补丁
function patch--chiphd()
{
	if [ "$(chiphd_check_lunch)" == "not lunch" ]; then
		echo "not lunch"
		return 0
	else
		thisDevice=`echo $DEVICE`
	fi

	local ConfigsFName=chiphd_config.sh
	local ConfigsPath=${thisDevice}/configs_chiphd
	local ProductFlagFile=${ConfigsPath}/NowCustom.sh

	if [ -d "$ConfigsPath" ]; then
		if [ -f "$ProductFlagFile" ]; then
			local NowProduct=$(sed -n '1p' $ProductFlagFile)
			if [ "$NowProduct" -a -f "$NowProduct" ]; then
				NowProduct=${NowProduct%/*}
				if [ ! -d "$NowProduct/androidPatch" ]; then
					mkdir $NowProduct/androidPatch
				fi
				patch-chiphd-create "${NowProduct}/androidPatch"
			fi
		fi
	fi
}

###############重载原生lunch
xw_get_aosp_lunch_func
if [ -f $CHIPHD_ANDROID_SCRIPT_PATH/aosp_lunch.sh ]; then
	source $CHIPHD_ANDROID_SCRIPT_PATH/aosp_lunch.sh
fi

function lunch()
{
	xw_o_aosp_lunch $@

	if [ "test_$(get_build_var TARGET_DEVICE)" != "test_generic" ]; then
		DEVICE=$(gettop)/device/softwinner/$(get_build_var TARGET_DEVICE)
	fi
	if [ "$(chiphd_check_lunch)" == "not lunch" ]; then
		echo "not lunch"
		return 0
	else
		thisDevice=`echo $DEVICE`
	fi

	local ConfigsFName=chiphd_config.sh
	local ConfigsPath=${thisDevice}/configs_chiphd
	local ProductFlagFile=${ConfigsPath}/NowCustom.sh

	if [ -d "$ConfigsPath" ]; then
		if [ -f "$ProductFlagFile" ]; then
			local NowProduct=$(sed -n '1p' $ProductFlagFile |   awk -F/ '{print $(NF-2) "/" $(NF-1)}')
			show_gtip $NowProduct
		else
			lunch-xw
		fi
	fi
}

#############################################################
## OTA完整包的生成
#############################################################
function once_complete_image()
{
	make installclean&&make -j16&&get_uboot&&make target-files-package&&pack
}


#############################################################
## 快速切换产品目录固件生成命令
#############################################################
function quick-pack()
{
	make installclean&&make -j16&&pack
}

#############################################################
## end for this script file
#############################################################

