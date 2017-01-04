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
	T=$(gettop)
	get_device_dir
	export ANDROID_IMAGE_OUT=$OUT
	export PACKAGE=$T/../lichee/tools/pack

	verity_data_init

	sh $DEVICE/package.sh $*
}

function fex_copy()
{
    if [ -e $1 ]; then
        cp -vf $1 $2
    else
        echo $1" not exist"
    fi
}

function get_uboot()
{
    pack $@
    echo "-------------------------------------"
    TARGET_BOARD_PLATFORM=$(get_build_var TARGET_BOARD_PLATFORM)
    UBOOT_FEX=u-boot.fex
    if [ "$TARGET_BOARD_PLATFORM" == "tulip" ]; then
        UBOOT_FEX=boot_package.fex
    fi

    fex_copy $PACKAGE/out/boot-resource.fex $OUT/boot-resource.fex
    fex_copy $PACKAGE/out/env.fex $OUT/env.fex
    fex_copy $PACKAGE/out/boot0_nand.fex $OUT/boot0_nand.fex
    fex_copy $PACKAGE/out/boot0_sdcard.fex $OUT/boot0_sdcard.fex
    fex_copy $PACKAGE/out/$UBOOT_FEX $OUT/u-boot.fex

    fex_copy $PACKAGE/out/toc1.fex $OUT/toc1.fex
    fex_copy $PACKAGE/out/toc0.fex $OUT/toc0.fex
}

function update_uboot()
{
    verity=`echo $@ | awk -F' ' '{ \
        for (i = 1; i <= NF; i++) { \
            if ($i == "-v") { \
                print $i; \
                break; \
            } \
        } \
    }'`
    if [ "x$verity" = "x-v" ]; then
        echo "copy toc into $1"
        fex_copy $PACKAGE/out/toc1.fex ./toc1.fex
        fex_copy $PACKAGE/out/toc0.fex ./toc0.fex
        zip -m $1 ./toc1.fex
        zip -m $1 ./toc0.fex
    fi
}

function pack4dist()
{
	# Found out the number of cores we can use
	cpu_cores=`cat /proc/cpuinfo | grep "processor" | wc -l`
	if [ ${cpu_cores} -le 8 ] ; then
	    JOBS=${cpu_cores}
	else
	    JOBS=`expr ${cpu_cores} / 2`
	fi

	DATE=`date +%Y%m%d`
	keys_dir="./vendor/security"
	target_files="$OUT/obj/PACKAGING/target_files_intermediates/$TARGET_PRODUCT-target_files-$DATE.zip"
	signed_target_files="$OUT/$TARGET_PRODUCT-signed_target_files-$DATE.zip"
	target_images="$OUT/target_images.zip"

	verity_data_init

	get_uboot $@

	make -j $JOBS target-files-package

	if [ -d $keys_dir ] ; then
		./build/tools/releasetools/sign_target_files_apks \
			-d $keys_dir $target_files $signed_target_files
		./build/tools/releasetools/img_from_target_files \
			$signed_target_files $target_images
		final_target_files=$signed_target_files
	else
		./build/tools/releasetools/img_from_target_files \
			$target_files $target_images
		final_target_files=$target_files
	fi

	unzip -o $target_images -d $OUT
	rm $target_images

	verity_data_init

	pack $@

	update_uboot $final_target_files $@
	echo -e "ota package: \033[31m$final_target_files\033[0m"
}


function verity_key_init()
{
	echo "-----verity_key_init-----"
	device/softwinner/common/verity/gen_dm_verity_key.sh
}

function verity_data_init()
{
	echo "-----verity_data_init-----"
	device/softwinner/common/verity/gen_dm_verity_data.sh ${OUT}/system.img ${OUT}/verity_block
	cp -vf ${OUT}/verity_block ${OUT}/verity_block.img
}
