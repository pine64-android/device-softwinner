#!/bin/bash
############################################################
## for chiphd begin
############################################################
####全局变量
THIS_PWD_BAK=$(pwd)
THIS_OLD_PWD_BAK=$(pwd)

function this_record_pwd() {
	THIS_PWD_BAK=$(pwd)
	cd - > /dev/null
	THIS_OLD_PWD_BAK=$(pwd)
	cd - > /dev/null
}

function this_resume_pwd() {
	cd $THIS_OLD_PWD_BAK
	cd $THIS_PWD_BAK
}

####删除存在的文件
function del_exist_file() {
	if [ "$1" ]; then
		if [ -f "$1" ]; then
			rm "$1"
		fi
	fi
}

#### windows路径的cd命令
function wcd() {
	if [ $# -lt 1 ]; then
		show_vit "need path"
		return 1
	fi

	if [ 1 -lt $# ]; then
		show_vit 'Is there space? use ""'
		return 0
	fi

	local my_path
	my_path="$(echo "$1" | sed 's/\\/\//g')"
	#echo $my_path
	if [ -d "$my_path" ]; then
		cd "$my_path"
	else
		echo "no path : $my_path"
	fi
}

#####获取ip
function get_my_ip_addr() {
	local myip=$(ifconfig eth0 2>/dev/null | grep "inet" | cut -f 2 -d ":" | cut -f 1 -d " ")
	if [ ! "$myip" ]; then
		myip=$(ifconfig eth1 2>/dev/null | grep "inet" | cut -f 2 -d ":" | cut -f 1 -d " ")
		if [ ! "$myip" ]; then
			myip=$(ifconfig wlan0 2>/dev/null | grep "inet" | cut -f 2 -d ":" | cut -f 1 -d " ")
			if [ ! "$myip" ]; then
				myip="127.0.0.1"
			fi
		fi
	fi
	echo $myip
}

#####简单检查是否是chiphd的服务器
function IS_CHIPHD_SERVER_BY_IP() {
	local chiphd_server_ip_set="192.168.1.20 192.168.1.22 192.168.1.23 192.168.1.101"
	this_ip=$(get_my_ip_addr)

	if [ "`echo $chiphd_server_ip_set | grep $this_ip`" ]; then
		echo "true"
	else
		echo "false"
	fi
}

#自动升级脚本
function update-chiphd-script-auto()
{
	##检查是否chiphd server
	if [ "`IS_CHIPHD_SERVER_BY_IP`" == "true" ]; then
		if [ -d $CHIPHD_ANDROID_SCRIPT_PATH ]; then
			local TempPwd=`pwd`
			cd $CHIPHD_ANDROID_SCRIPT_PATH && echo -e -n "    now update script : " && git pull
			cd $TempPwd
		else
			echo -e "\e[1;33m  No dir -- $CHIPHD_ANDROID_SCRIPT_PATH\e[0m"
		fi
	fi
}

# 覆盖多个同名文件（比如各解析度的wallpaper）
function findcp()
{
	SFile=$1  #拷贝源文件
	TDir=$2   #目标目录
	if [ "t$SFile" != "t" -a -f $SFile ]; then
		if [ "t$TDir" = "t" ]; then
			TDir=.
		fi
		if [ -d $TDir ]; then
			SFileName=${SFile##*/}
			TFiles=`find $TDir -name $SFileName`
			if [ "$TFiles" ]; then
				for ii in $TFiles
				do
					echo "cp $SFile $ii" && cp $SFile $ii
				done
			fi
		fi
	fi
}
#############################################################
## end for this script file
#############################################################
