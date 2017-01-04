#!/bin/bash

aaa=`find -name polaris_chiphd.mk`

##对找到的每个文件进行处理
line=0
for f in $aaa
do
    line=$(sed -n '/modules.mk/='  $f)
    line=`expr $line + 1`
    #echo $line
    ##在$line插入一行
    sed -i "$line i \$\(call inherit-product-if-exists, \$\(LOCAL_PATH\)\/preApk\/ChiphdPreApk.mk\)" $f
done


#############################################################
## end for this script file
#############################################################

