#!/bin/bash

# This is a sample of the command line make used to build
#   the libraries and binaries for the Pandaboard.
# Please customize this path to match the location of your
#   Android source tree. Other variables may also need to
#   be customized such as:
#     $CROSS, $PRODUCT, $KERNEL_ROOT

export ANDROID_BASE=/home/liuhanlei/workspace/a80/android
export KERNEL_BASE=/home/liuhanlei/workspace/a80/lichee
export CROSS_BASE=/home/liuhanlei/workspace/a80/android/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.7/bin/arm-linux-androideabi-
export PRODUCT_BASE=kylin-mb976a9

#export ANDROID_BASE=/home/bbb/work2/android
#export KERNEL_BASE=/home/bbb/work2/android/kernel
#export CROSS_BASE=/home/bbb/work2/android/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
#export PRODUCT_BASE=pandaboard

#export ANDROID_BASE=/home/bbb/work/panda_4.0.3
#export KERNEL_BASE=/home/bbb/work/panda_4.0.3/kernel
#export CROSS_BASE=/home/bbb/work/panda_4.0.3/prebuilt/linux-x86/toolchain/arm-linux-androideabi-4.4.x/bin/arm-linux-androideabi-
#export PRODUCT_BASE=panda

make -C build/android \
	VERBOSE=1 \
	TARGET=android \
	ANDROID_ROOT=${ANDROID_BASE} \
	KERNEL_ROOT=${KERNEL_BASE} \
	CROSS=${CROSS_BASE} \
	PRODUCT=${PRODUCT_BASE} \
	MPL_LIB_NAME=mplmpu \
	echo_in_colors=echo \
	-f shared.mk \
	clean
	
make -C build/android \
	VERBOSE=1 \
	TARGET=android \
	ANDROID_ROOT=${ANDROID_BASE} \
	KERNEL_ROOT=${KERNEL_BASE} \
	CROSS=${CROSS_BASE} \
	PRODUCT=${PRODUCT_BASE} \
	MPL_LIB_NAME=mplmpu \
	echo_in_colors=echo \
	-f shared.mk
