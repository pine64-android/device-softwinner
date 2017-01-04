#!/bin/bash

####检查是否lunch
function chiphd_check_lunch()
{
	if [ "$DEVICE" ]; then
		echo "lunch : path $DEVICE"
	else
		echo 'not lunch'
	fi
}

#### touch all file
function update_all_type_file_time_stamp()
{
	local tttDir=$1
	if [ -d "$tttDir" ]; then
		find $tttDir -name "*" | xargs touch -c
		find $tttDir -name "*.*" | xargs touch -c
		echo "    TimeStamp $tttDir"
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
	#update_c_type_file_time_stamp `echo device/softwinner/*-common/hardware/realtek`
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
function chiphd_gitclean_project()
{
	local tDir=$1
	if [ ! "$tDir" ]; then
		tDir=.
	fi
	if [ -d $tDir/.git ]; then
		local OldPWD=$(pwd)
		local CleanFiles=
		cd $tDir  && CleanFiles=`git clean -dn`
		if [ "$CleanFiles" ]; then
			git clean -df  $CleanFiles #&& echo "clean standard device config"
		fi
		cd $OldPWD
	fi
}

#### checkout默认配置文件
function chiphd_checkout_project()
{
	local tDir=$1
	if [ ! "$tDir" ]; then
		tDir=.
	fi
	if [ -d $tDir/.git ]; then
		local OldPWD=$(pwd)
		cd $tDir && git checkout -- . #&& echo "checkout standard device config"
		cd $OldPWD
	fi
}

#### checkout默认配置文件
function chiphd_recover_project()
{
	local tDir=$1
	if [ ! "$tDir" ]; then
		tDir=.
	fi
	if [ -d $tDir/.git ]; then
		local OldPWD=$(pwd)
		cd $tDir && echo "---- recover $tDir"

		git reset HEAD . ###recovery for cached files

		thisFiles=`git clean -dn`
		if [ "$thisFiles" ]; then
			git clean -df
		fi

#		thisFiles=`git diff --cached --name-only`
#		if [ "$thisFiles" ]; then
#			git checkout HEAD $thisFiles
#		fi

		thisFiles=`git diff --name-only`
		if [ "$thisFiles" ]; then
			git checkout HEAD $thisFiles
		fi
		cd $OldPWD
	fi
}

#### 获取所以git库路径,在android目录下调用
function chiphd_get_repo_git_path_from_xml()
{
	local default_xml=.repo/manifest.xml
	if [ -f $default_xml ]; then
		grep '<project' $default_xml | sed 's%.*path="%%' | sed 's%".*%%'
	fi
}

#### 获取所以git库路径,在android目录下调用, $1为绝对路径
function chiphd_save_repo_git_path_cache()
{
	local NowPWD=$(pwd)
	local OldPWD=$(cd - > /dev/null && pwd)
	local CachePath=$1
	local CacheFlagFile=
	if [ "$CachePath" -a -d "$CachePath"  ]; then
		CacheFlagFile=${CachePath}/ThisCommitID
	else
		return 1
	fi
	
	if [ -l .repo/manifest.xml ]; then
		local R_manifestFile=`readlink .repo/manifest.xml`
		if [ -f .repo/${R_manifestFile} ]; then
			local S_manifestFile=${R_manifestFile##*/}

			cd .repo/manifests
			local NowCommitID=`git log -1 --pretty=format:"%H" $S_manifestFile`
			local OldCommitID=0

			if [ -f ${CacheFlagFile}  ]; then
				OldCommitID=`sed -n '1p' ${CacheFlagFile}`
			fi
			
			if [ "$OldCommitID" = "NowCommitID" ]; then
				return 0
			else
				cd $NowPWD
				grep '<project' .repo/manifest.xml | sed 's%.*path="%%' | sed 's%".*%%' > ${CacheFlagFile}.path.log
				echo $NowCommitID > ${CacheFlagFile}
			fi

		fi
	fi
	cd $OldPWD
	cd $NowPWD
}

#### 恢复默认配置文件
function chiphd_recover_standard_device_cfg()
{
	local tDir=$1
	if [ "$tDir" -a -d $tDir ]; then
		#echo $tDir
		:
	else
		return 0
	fi
	local tOldPwd=$OLDPWD
	local tNowPwd=$PWD
	cd $(gettop)
	#echo "now get all project from repo..."
	local AllRepoProj=`chiphd_get_repo_git_path_from_xml`
	if [ "$AllRepoProj" ]; then
		for ProjPath in $AllRepoProj
		do
			if [ -d "${tDir}/$ProjPath" ]; then
				chiphd_recover_project $ProjPath
			fi
		done
	fi
	cd $tOldPwd
	cd $tNowPwd
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

#检查是否存在函数及调用
function chiphdCustomScriptCallBack()
{
	CustomScriptFile=$1
	CustomScriptFunc=$2
	CustomScriptArg1=$3
	CustomScriptArg2=$4

	if [ -f $CustomScriptFile ]; then
		if [ "`grep  "^function $CustomScriptFunc"  $CustomScriptFile `" ]; then
			$CustomScriptFunc $CustomScriptArg1 $CustomScriptArg2
		fi
	fi
}

#获取intel 内部的device目录
function chiphdGetIntelInnerDevicePath()
{
	local ATop=$(gettop)
	local AVer=$(chiphd_get_android_ver)
	local RetVal=
	if [ "$AVer"_test = "4.2"_test  ]; then
		thisProductFile="`echo $TARGET_PRODUCT`".mk
		thisProductFile=${thisProductFile##*/}
		thisProductFile=`find device/softwinner/ -name $thisProductFile`
		if [ "$thisProductFile" -a -f "$thisProductFile" ]; then
			RetVal=${ATop}/${thisProductFile%/*}
		fi
	fi

	if [ "$AVer"_test = "4.4"_test  ]; then
		thisProductFile="`echo $TARGET_PRODUCT`".mk
		thisProductFile=${thisProductFile##*/}
		thisProductFile=`find device/softwinner/ -name $thisProductFile`
		if [ "$thisProductFile" -a -f "$thisProductFile" ]; then
			RetVal=${ATop}/${thisProductFile%/*}
		fi
	fi

	if [ "$RetVal" -a -d "$RetVal" ]; then
		echo $RetVal
	else
		echo "unknow"
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

	local thisSDKTop=$(gettop)
	local ConfigsPath=${thisSDKTop}/../chiphdDevices
	if [ -d "$ConfigsPath" ]; then
		ConfigsPath=$(cd $ConfigsPath && pwd)
	else
		echo "no path : $ConfigsPath"
		return 1
	fi
	local ConfigsFName=proj_help.sh
	local ProductSetTop=${ConfigsPath}/custom
##遍历所有客户方案配置
#	local ProductSet=`find $ConfigsPath -name $ConfigsFName`

	local ProductSetShort=`find $ProductSetTop -name $ConfigsFName | awk -F/ '{print $(NF-3) "/" $(NF-2) "/" $(NF-1)}' | sort`
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
	echo "" #&& return 0
	local ProductSelPath="$ProductSetTop/$MySEL"
	if [ "$ProductSel" -a -d "$ProductSelPath" -a ! "$ProductSelPath" = "$ProductSetTop/" ]; then
		## 旧项目
		local OldProductSelPath=
		local OldProductSelDir=
		local OldBaseSelDir=
		if [ -f ${ConfigsPath}/NowCustom.sh ]; then
			OldProductSelPath=$(sed -n '1p' ${ConfigsPath}/NowCustom.sh)
			OldProductSelPath=${OldProductSelPath%/*}
			OldProductSelDir=${OldProductSelPath}/android

			OldBaseSelDir=${OldProductSelPath%/*} && OldBaseSelDir=${OldBaseSelDir%/*}/android
		fi
		## 新项目
		echo "${ProductSelPath}/$ConfigsFName" > ${ConfigsPath}/NowCustom.sh

		## 导入项目自定义脚本
		local MyCustomScript=${ProductSelPath}/${ConfigsFName}
		unset prj_chiphd_help1
		source $MyCustomScript

		#### 更新时间戳并拷贝到配置根目录
		local ProjectSelDir=$ProductSelPath/android
		local thisBaseSelDir=${ProductSelPath%/*} && thisBaseSelDir=${thisBaseSelDir%/*}/android
		
		#echo "OldBaseSelDir = $OldBaseSelDir"
		#echo "thisBaseSelDir = $thisBaseSelDir"
		
		if [ -d $ProjectSelDir -a $thisBaseSelDir ]; then
			local tOldPwd=$OLDPWD
			local tNowPwd=$PWD

			local thisProjDelFileSh=$thisSDKTop/chiphd_delete.sh
			if [ -f "$thisProjDelFileSh" ]; then rm $thisProjDelFileSh; fi

			## 清除旧项目的修改
			if [ "$OldBaseSelDir" -a "$OldBaseSelDir" != "$thisBaseSelDir"  ]; then
				show_vip "clean by $OldBaseSelDir" && chiphd_recover_standard_device_cfg $OldBaseSelDir
			fi
			show_vip "clean by $OldProductSelDir" && chiphd_recover_standard_device_cfg $OldProductSelDir
			## 确保新项目的修改纯净
			show_vip "clean by $thisBaseSelDir" && chiphd_recover_standard_device_cfg $thisBaseSelDir
			show_vip "clean by $ProjectSelDir" && chiphd_recover_standard_device_cfg $ProjectSelDir

			## 新项目代码拷贝
			update_all_type_file_time_stamp $thisBaseSelDir
			show_vip "copy source code : $thisBaseSelDir/* " && cp -r $thisBaseSelDir/*  $thisSDKTop/ && echo "copy done"
			update_all_type_file_time_stamp $ProjectSelDir
			show_vip "copy source code : $ProjectSelDir/*  " && cp -r $ProjectSelDir/*  $thisSDKTop/ && echo "copy done"

			if [ -f "$thisProjDelFileSh" ]; then
				show_vip "now do delete files" && source $thisProjDelFileSh
			fi
			#### 预安装APK
			local thisIntelInnerDevicePath=$(chiphdGetIntelInnerDevicePath)
			echo thisIntelInnerDevicePath=${thisIntelInnerDevicePath}
			local thisPreApkPath=${thisIntelInnerDevicePath}/preApk
			if [ -d "$thisPreApkPath" ]; then
				if [ "`find $thisPreApkPath -name "*.apk"`" ]; then
			    DoChiphdPreApk $thisPreApkPath
				echo
			  fi
			fi
			#### 自定义函数调用
			chiphdCustomScriptCallBack $MyCustomScript prj_chiphd_help1 ${thisDevice} ${ProductSelPath}

			cd $tOldPwd
			cd $tNowPwd
		else
			echo "no config : $ProjectSelDir"
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
#	DEVICE=$(gettop)/device/intel/baytrail
	if [ "$(chiphd_check_lunch)" == "not lunch" ]; then
		echo "not lunch"
		return 0
	else
		thisDevice=`echo $DEVICE`
	fi

	local ConfigsFName=proj_help.sh
	local ConfigsPath=$(gettop)/../chiphdDevices
	local ProductFlagFile=${ConfigsPath}/NowCustom.sh

	if [ -d "$ConfigsPath" ]; then
		if [ -f "$ProductFlagFile" ]; then
			local NowProduct=$(sed -n '1p' $ProductFlagFile |   awk -F/ '{print $(NF-3) "/" $(NF-2) "/" $(NF-1)}')
			show_gtip $NowProduct
		else
			lunch-xw
		fi
	fi
}

#############################################################
## end for this script file
#############################################################

