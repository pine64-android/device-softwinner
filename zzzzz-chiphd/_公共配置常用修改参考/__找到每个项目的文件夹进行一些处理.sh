#!/bin/bash
###在configs_chiphd/custom运行
#找文件文件确定项目目录，可用sed替换为下级目录
#aaa=`find -name chiphd_config.sh | sed  's:chiphd_config.sh::g'` # 每个项目的根目录
aaa=`find -name chiphd_config.sh | sed  's:chiphd_config.sh:adevice/configs/:g'`
##需要修改的键
#bbb="gc0328 sp0718 sp2518 gc0311 sp0a19 siv120d siv121d"

###
#MainKey="camera_list_para"

##对找到的每个文件进行处理
line=0
for f in $aaa
do
	#echo "$f"
	cp ./media_profiles.xml $f   ##拷文件进行替换
done


#############################################################
## end for this script file
#############################################################

