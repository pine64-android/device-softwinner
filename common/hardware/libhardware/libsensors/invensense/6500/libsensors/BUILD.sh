#!/bin/bash

# This is a sample of the command line make used to build
#   the libraries and binaries for the Pandaboard.
# Please customize this path to match the location of your
#   Android source tree. Other variables may also need to
#   be customized such as:
#     $CROSS, $PRODUCT, $KERNEL_ROOT

export ANDROID_BASE=/home/mcaramello/Software/Android/J/pandaboard

make -C software/build/android \
	VERBOSE=0 \
	TARGET=android \
	ANDROID_ROOT=${ANDROID_BASE}/android \
	KERNEL_ROOT=${ANDROID_BASE}/kernel/omap \
	CROSS=${ANDROID_BASE}/android/prebuilts/gcc/linux-x86/arm/arm-linux-androideabi-4.6/bin/arm-linux-androideabi- \
	PRODUCT=panda \
	MPL_LIB_NAME=mplmpu \
	echo_in_colors=echo \
	-f shared.mk

