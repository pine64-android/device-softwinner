#!/bin/bash
############################################################
##  创建android repo git tag 文件
############################################################
#### repo tag file to system.img
MY_DAY=$(date '+%Y%02m%02d')
MY_TIME=$(date '+%02k%02M')
TIME_NAME=${MY_DAY}-${MY_TIME}
TIME_TXT=$(date '+%Y-%02m-%02d %02k:%02M')
REPO_ANDROID_TAGPath=$OUT/system
#build tag
if [ "$1" = "--tag" ]; then
	if [ -d $REPO_ANDROID_TAGPath ]; then
		echo "now get android-repo-tag..."
		echo -e "android pack at $TIME_TXT\n" > $REPO_ANDROID_TAGPath/release_tag.txt
		repo forall -c 'echo "--------------------------------------------------------------------------------" && \
			pwd && git log -1 && echo ****status**** && git status' >> $REPO_ANDROID_TAGPath/release_tag.txt
		if [ -d $DEVICE/.git ]; then
			cd $DEVICE
			(echo "--------------------------------------------------------------------------------" && \
			pwd && git log -1 && echo ****status**** && git status) >> $REPO_ANDROID_TAGPath/release_tag.txt
			cd - > /dev/null
		fi
	fi
fi

#############################################################
## for chiphd end
#############################################################