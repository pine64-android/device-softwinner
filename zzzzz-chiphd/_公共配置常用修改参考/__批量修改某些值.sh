#!/bin/bash

#找文件
aaa=`find -name sys_config.fex`
##需要修改的键
bbb="gc0328 sp0718 sp2518 gc0311 sp0a19 siv120d siv121d"

###
MainKey="camera_list_para"

##对找到的每个文件进行处理
line=0
for f in $aaa
do
	#echo "$f"
	for kk in $bbb
	do
		##找是否赋值为0，
		##斜杠之内的为正则表达式，^为一行开头，\s代表空白字符类
		##\s*就为任意个空白字符
    line=$(sed -n "/^$kk\s*=\s*0/="  $f)
    if [ "$line" ]; then  ##找到就改为1
    	sed -i "s/^$kk\s*=\s*0/$kk                 = 1/" $f && echo "modify $f , $kk"
    else
    	ttt="`grep "$kk" $f`"
    	if [ "$ttt" ]; then
    		: #echo "$ttt"
    	else
    		##新增
    		line=$(sed -n "/\[$MainKey\]/="  $f)
    		if [ "$line" ]; then
    			 line=`expr $line + 2`
    			 ##插入一行
    			 sed -i "$line i $kk                 = 1" $f
    		else
    			echo "err : not find $MainKey"
    		fi
    	fi
    fi
    ##在$line插入一行
    #sed -i "$line i \$\(call inherit-product-if-exists, \$\(LOCAL_PATH\)\/preApk\/ChiphdPreApk.mk\)" $f
	done
done


#############################################################
## end for this script file
#############################################################

