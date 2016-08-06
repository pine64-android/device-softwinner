#!/bin/bash
#
# sdk_switcher.sh
# (c) Copyright 2014
# Allwinner Technology Co., Ltd. <www.allwinnertech.com>
# xiechr <xiechr@allwinnertech.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
cur_dir=`pwd`
manifest_tmp_dir=`pwd`/.manifest_tmp
manifest_url="git://172.16.1.11/manifest.git"
scripts_branch="scripts"

#setting for shengzhen user
user=`echo $@ | awk '{for (i=1;i<NF;i++) if ($i=="-u" && $(i+1)=="sz") print $(i+1)}'`
if [ "$user" = "sz" ] ; then
    manifest_url="git://172.20.1.24/manifest.git"
fi

rm -rf $manifest_tmp_dir
mkdir $manifest_tmp_dir
echo "git clone $manifest_url $manifest_tmp_dir"
git clone $manifest_url $manifest_tmp_dir
cd $manifest_tmp_dir
echo "git checkout -b $scripts_branch remotes/origin/$scripts_branch"
git checkout -b $scripts_branch remotes/origin/$scripts_branch
if [ ! -f $manifest_tmp_dir/scripts/sdk_cmd.sh ] ; then
	echo "can not get switch scripts from manifest server"
	rm -rf $manifest_tmp_dir
	exit 1
fi
cd $cur_dir
$manifest_tmp_dir/scripts/sdk_cmd.sh $@
rm -rf $manifest_tmp_dir
