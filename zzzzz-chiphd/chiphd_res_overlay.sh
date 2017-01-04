
##替换资源(目前只验证jpg图片类的快速替换)
function do_chiphd_res_overlay_replace()
{
	local thisAAPT=
	local thisTop=`gettop`
	if [ "$thisTop" -a -d "$thisTop" ]; then
		thisAAPT=$thisTop/out/host/linux-x86/bin/aapt
  else
  	thisAAPT=$thisTop/device/softwinner/zzzzz-chiphd/apktool/aapt
  fi
  
  if [ -f $thisAAPT ]; then
  	: #echo "use aapt : $thisAAPT"
  else
  	return 1
  fi
  
  ##为恢复目录状态而设
  local OldPath0=$(echo $OLDPWD)
  local OldPath1=$(echo $PWD)
  ##查找替换apk的顶级目录
  local MyTargeTopPath=`echo $OUT/obj/APPS`
  ##替换apk资源的顶级目录
  local MyResOverlayTopPath=`echo $DEVICE/res-chiphd-overlay`
  if [ "$MyTargeTopPath" -a -d $MyTargeTopPath -a "$MyResOverlayTopPath" -a -d $MyResOverlayTopPath ]; then
  	local SubF=`ls -A $MyResOverlayTopPath`
  	if [ "$SubF" ]; then
  		##遍历每个APK要替换的资源文件夹
  		for f in $SubF
  		do
  			#echo "overlay replace $f"
  			if [ -d $MyResOverlayTopPath/$f ]; then
  				##获取目标apk路径
  				MyTargePath=`find $MyTargeTopPath -name "${f}*intermediates" -type d`
  				if [ "$MyTargePath" -a -d "$MyTargePath" ]; then
  					##获取目标apk
  					MyTarge=`find $MyTargePath -name "*.apk"`
  					if [ "$MyTarge" -a -f "$MyTarge" ]; then
  						##实现替换
  						cd $MyResOverlayTopPath/$f
  						MyNewFiles=`find res -name "*.*" -type f`
  						if [ "$MyNewFiles" ]; then
  							$thisAAPT remove -v $MyTarge $MyNewFiles
  							$thisAAPT add -v $MyTarge $MyNewFiles
  							#echo "$thisAAPT remove -v $MyTarge $MyNewFiles"
  							#echo "$thisAAPT add -v $MyTarge $MyNewFiles"
  						fi
  					else
  						echo "no result about $f"
  					fi
  				fi
  			fi
  		done
  	fi
  fi

	##回目录
  if [ "$OldPath0" -a -d "$OldPath0" ]; then
   cd $OldPath0
  fi
  if [ "$OldPath1" -a -d "$OldPath1" ]; then
   cd $OldPath1
  fi
}

