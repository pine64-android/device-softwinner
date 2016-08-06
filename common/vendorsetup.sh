#
# Copyright (C) 2012 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# This file is executed by build/envsetup.sh, and can use anything
# defined in envsetup.sh.
#
# In particular, you can add lunch options with the add_lunch_combo
# function: add_lunch_combo generic-eng

export JAVA_HOME=/usr/lib/jvm/java-7-openjdk-amd64
export PATH=$JAVA_HOME/bin:$PATH
export CLASSPATH=.:$JAVA_HOME/lib

function get_device_dir()
{
	DEVICE=$(gettop)/device/softwinner/$(get_build_var TARGET_DEVICE)
}

function cdevice()
{
    candroid
	get_device_dir
	cd $DEVICE
}

function cout()
{
	cd $OUT
}

function get_lichee_out_dir()
{
    LICHEE_DIR=$ANDROID_BUILD_TOP/../lichee

    TARGET_BOARD_PLATFORM=$(get_build_var TARGET_BOARD_PLATFORM)

    if [ "$TARGET_BOARD_PLATFORM" == "kylin" ]; then
        LINUXOUT_DIR=$LICHEE_DIR/out/sun9iw1p1/android/common
    fi
    if [ "$TARGET_BOARD_PLATFORM" == "astar" ]; then
        LINUXOUT_DIR=$LICHEE_DIR/out/sun8iw5p1/android/common
    fi
    if [ "$TARGET_BOARD_PLATFORM" == "octopus" ]; then
        LINUXOUT_DIR=$LICHEE_DIR/out/sun8iw6p1/android/common
    fi
    if [ "$TARGET_BOARD_PLATFORM" == "tulip" ]; then
        LINUXOUT_DIR=$LICHEE_DIR/out/sun50iw1p1/android/common
    fi

    LINUXOUT_MODULE_DIR=$LINUXOUT_DIR/lib/modules/*/*
}

function extract-bsp()
{
	CURDIR=$PWD

	get_lichee_out_dir
	get_device_dir

	cd $DEVICE

	#extract kernel
	if [ -f kernel ] ; then
		rm kernel
	fi
	cp $LINUXOUT_DIR/bImage kernel
	echo "$DEVICE/bImage copied!"

	#extract linux modules
	if [ -d modules ] ; then
		rm -rf modules
	fi
	mkdir -p modules/modules
	cp -rf $LINUXOUT_MODULE_DIR modules/modules
	echo "$DEVICE/modules copied!"
	chmod 0755 modules/modules/*

# create modules.mk
(cat << EOF) > ./modules/modules.mk 
# modules.mk generate by extract-files.sh, do not edit it.
PRODUCT_COPY_FILES += \\
	\$(call find-copy-subdir-files,*,\$(LOCAL_PATH)/modules,system/vendor/modules)
EOF

	cd $CURDIR
}

function pack()
{
    candroid
	T=$(gettop)
	get_device_dir
	export ANDROID_IMAGE_OUT=$OUT
	export PACKAGE=$T/../lichee/tools/pack

	sh $DEVICE/package.sh $*
}

function get_uboot()
{
    pack $@
    echo "-------------------------------------"
    if [ ! -e $OUT/boot-resource ];then
        mkdir $OUT/boot-resource
    fi
    rm -rf $OUT/boot-resource/*
    cp -vf $PACKAGE/out/boot-resource.fex $OUT
    cp -rf $PACKAGE/out/boot-resource/* $OUT/boot-resource/
    echo "\"$PACKAGE/out/boot-resource/* -> $OUT/boot-resource/\""
    cp -vf $PACKAGE/out/env.fex $OUT
    cp -vf $PACKAGE/out/boot0_nand.fex $OUT
    cp -vf $PACKAGE/out/boot0_sdcard.fex $OUT
    cp -vf $PACKAGE/out/u-boot.fex $OUT
}

function pack4dist()
{
	DATE=`date +%Y%m%d`
	keys_dir="./vendor/security"
	target_files="$OUT/obj/PACKAGING/target_files_intermediates/$TARGET_PRODUCT-target_files-$DATE.zip"
	signed_target_files="$OUT/signed_target_files.zip"
	target_images="$OUT/target_images.zip"

	get_uboot $@

	make target-files-package

	if [ -d $keys_dir ] ; then
		./build/tools/releasetools/sign_target_files_apks \
			-d $keys_dir $target_files $signed_target_files
		./build/tools/releasetools/img_from_target_files \
			$signed_target_files $target_images
	else
		./build/tools/releasetools/img_from_target_files \
			$target_files $target_images
	fi

	unzip -o $target_images -d $OUT
	rm $target_images
	pack $@
}

# add pengyuding for chiphd lunch

function candroid()
{
    cd $ANDROID_BUILD_TOP
}
function ccommon()
{
    cd $ANDROID_BUILD_TOP/device/softwinner/common
}
function clichee()
{
    cd $ANDROID_BUILD_TOP/../lichee
}
function cbrandy()
{
    cd $ANDROID_BUILD_TOP/../lichee/brandy
}
function clinux()
{
    cd $ANDROID_BUILD_TOP/../lichee/linux-3.10
}
function cconfig()
{
    cd $ANDROID_BUILD_TOP/../lichee/tools/pack/chips/sun50iw1p1/configs
}

function getcustom()
{
    NowCustomFlagFile=$DEVICE/configs_chiphd/NowCustom.sh
    if [ -f $NowCustomFlagFile ]; then
        #bash $thisXwDevicePath/configs_chiphd/do_chiphd.sh
        NowCustomCfg=`sed -n '1p' $NowCustomFlagFile`
        XWCUSTOM=${NowCustomCfg%/*}
        if [ -d $XWCUSTOM ]; then
            return 0
        fi
    else
        XWCUSTOM=""
        echo "not lunch-xw!!"
    fi
    return -1
}

function ccustom()
{
    getcustom
    [ $? -ne 0 ] && return
    cd $XWCUSTOM
}

function make-new()
{
    j="-j32"
    if [ $1 ];then
        j=$1
    fi
    candroid
    a=`date +%s`
    make installclean && make $j && pack
    sec=$((`date +%s`-a))
    min=$((sec/60))
    sec=$((sec%60))
    echo "用了"$min"分"$sec"秒"
}
function make-all()
{
    make-brandy
    clichee && . build.sh && candroid && extract-bsp && make-new $1 &
    candroid
}
function make-linux()
{
    clichee
    . build.sh &
}
function make-brandy()
{
    cbrandy
    . build.sh &
}
function make-lichee()
{
    make-brandy
    make-linux
}

function apply-lichee()
{
    getcustom
    [ $? -ne 0 ] && return

    clinux
    echo " "
    echo ">>> `pwd`"
    git reset HEAD .
    git co .
    git clean -df
    if [ -f $XWCUSTOM/lichee/linux.diff ]; then
        git apply $XWCUSTOM/lichee/linux.diff
    fi
    git st

    cbrandy
    echo " "
    echo ">>> `pwd`"
    git reset HEAD .
    git co .
    git clean -df
    if [ -f $XWCUSTOM/lichee/brandy.diff ]; then
           git apply $XWCUSTOM/lichee/brandy.diff
    fi
    git st
}

function patch-lichee()
{
    if [ $1 ] ; then
        echo ""
        echo ">>> Generating patch for $1 ..."
        cddir="c$1"
        $cddir
        git add -A
        git st
        git diff --cached > $XWCUSTOM/lichee/$1.diff

        size=`ls -l $XWCUSTOM/lichee/$1.diff | cut -d" " -f5`
        if [ $size -gt 0  ] ; then
            echo "Generate $1 success: $XWCUSTOM/lichee/$1.diff"
        else
            rm $XWCUSTOM/lichee/$1.diff
            echo "$1 has no change."
        fi
    else
        getcustom
        [ $? -ne 0 ] && return

        patch-lichee brandy
        patch-lichee linux

        candroid
    fi
}

function lunch-chiphd()
{
    lunch tulip_chiphd-eng 
}
function lunch-chiphd-user()
{
    lunch tulip_chiphd-user 
}
function lunch-t1()
{
    lunch tulip_t1-eng
}
function lunch-t1-user()
{
    lunch tulip_t1-user
}
