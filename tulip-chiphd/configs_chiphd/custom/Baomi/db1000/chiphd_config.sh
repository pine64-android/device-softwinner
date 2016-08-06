#!/bin/bash

##定制动画

##定制android logo

##定制boot logo

##定制桌面布局

##定制壁纸

##预装apk

##摄像头配置文件

##定制build.prop属性
#含国家，语言，时区，亮度，时间格式，品牌相关，字体，音量，休眠时间，蓝牙名称，输入法等

function note-chiphd() {
cat <<EOF
touch：
1)update wifi & bluetooth time stamp
EOF
}

#function prj_chiphd_help1() {
function prj_chiphd_help() {
	MainDevicePath=$1
	ChiphdDevicePathTop=$2
	ATop=$MainDevicePath/../../..
	
	echo "now update wifi time stamp ..."
	touch -c  ${ATop}/external/wpa_supplicant_8/* && echo "touch -c ${ATop}/external/wpa_supplicant_8/*"
	touch -c  ${ATop}/hardware/libhardware_legacy/* && echo "touch -c ${ATop}/hardware/libhardware_legacy/*"

	echo "now update bluetooth time stamp ..."
	touch -c  ${ATop}/external/bluetooth/*  && echo "touch -c ${ATop}/external/bluetooth/*"
	touch -c  ${ATop}/system/bluetooth/*  && echo "touch -c ${ATop}/system/bluetooth/*"
	note-chiphd
}


