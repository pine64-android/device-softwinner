#!/bin/bash 

if [ -d "$1" ]; then
IMG=$1
else
	echo "Input image $1 wrong!"
	exit 1
fi
echo "Generate file verify block for file-system $IMG"

TARGET_PATH=$OUT/verity
RUN_PATH=$DEVICE/verity
FILE_SIG_TOOL="$RUN_PATH/file_sign"

${FILE_SIG_TOOL} ${IMG} system sha256 ${TARGET_PATH}/verity_block $TARGET_PATH/rsa_key/rsa_key.pair -d ${TARGET_PATH} >${TARGET_PATH}/file_sig.log

exit 0

