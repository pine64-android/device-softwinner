#!/bin/bash 

if [ $# -eq 2 ]; then
	IN=$1
	OUT=$2
else
	echo "Input params error!"
	exit 1
fi

echo "Generate verity block data for file $IMG"
TARGET_PATH=device/softwinner/common/verity

IMG=${TARGET_PATH}/system.img.new

simg2img ${IN} ${IMG}

BLK_SIZE=4096
METADATA_BLK_OFFSET=8

SIZE=`du -b ${IMG} | awk '{print $1}'`

echo ${SIZE}
echo ${BLK_SIZE}

#########################################
## system image must 4096 block align
#########################################

BLKS=`expr ${SIZE} / ${BLK_SIZE}`
if [ "`expr ${SIZE} % ${BLK_SIZE} `" != "0" ]; then
	echo " Can't support system image size, Must 4096 bytes align !!!"
	exit 1
else
	echo "Prepare for Dm-verity data ..."
fi

##########################################
##
## Hash tree Layer 1/2/3 block count
##
###########################################

if [ `expr ${BLKS} % 128` = "0" ] ; then
HASH_L1_CNT=`(expr ${BLKS} / 128) `
else
CNT=`(expr ${BLKS} / 128) `
HASH_L1_CNT=`expr ${CNT} + 1`
fi

if [ `expr ${HASH_L1_CNT} % 128` = "0" ] ; then
HASH_L2_CNT=`(expr ${HASH_L1_CNT} / 128) `
else
CNT=`(expr ${HASH_L1_CNT} / 128) `
HASH_L2_CNT=`expr ${CNT} + 1`
fi

HASH_L3_CNT=1

HASH_CNT=`expr ${HASH_L1_CNT} + ${HASH_L2_CNT}`
HASH_CNT=`expr ${HASH_CNT} + ${HASH_L3_CNT}`

##############################################
##
## Generate the raw hash table and hash tree
##
#############################################

${TARGET_PATH}/veritysetup format $IMG $TARGET_PATH/hash $TARGET_PATH/raw_table

############################################
##
## Generate the verity format table 
##
############################################

echo "Convert raw_table to table "
cp $TARGET_PATH/table $TARGET_PATH/table_tmp
ROOT_HASH=`sed -n '/Root Hash/p' $TARGET_PATH/raw_table | awk '{print $3}'`
awk 'NR==1{$4=a}1' a=${BLK_SIZE} $TARGET_PATH/table > $TARGET_PATH/table_tmp 
awk 'NR==1{$5=a}1' a=${BLK_SIZE} $TARGET_PATH/table >  $TARGET_PATH/table_tmp 
awk 'NR==1{$6=a}1' a=${BLKS}  $TARGET_PATH/table >  $TARGET_PATH/table_tmp 
awk 'NR==1{$7=a}1' a=${METADATA_BLK_OFFSET}  $TARGET_PATH/table >  $TARGET_PATH/table_tmp 
awk 'NR==1{$9=a}1' a=${ROOT_HASH}  $TARGET_PATH/table > $TARGET_PATH/table_tmp 

cat  $TARGET_PATH/table_tmp

############################################
##
## Sign the dm-verity table
##
############################################
echo "Sign the table"
openssl dgst -sha256 -binary -sign  $TARGET_PATH/rsa_key/rsa_key.pair  $TARGET_PATH/table_tmp > $TARGET_PATH/sign

 echo "--------------Dm-verity rsa debug info----------------" >  $TARGET_PATH/rsa_info
 echo "	Table file:" >> $TARGET_PATH/rsa_info
 cat  $TARGET_PATH/table_tmp>> $TARGET_PATH/rsa_info
 echo " Rsa decrypt result " >> $TARGET_PATH/rsa_info
 openssl rsautl -verify -in  $TARGET_PATH/sign -inkey  $TARGET_PATH/rsa_key/rsa_key.pair -raw -hexdump >> $TARGET_PATH/rsa_info
 echo " Sha256 result ">> $TARGET_PATH/rsa_info
 sha256sum  $TARGET_PATH/table_tmp >> $TARGET_PATH/rsa_info

#############################################
##
## Generate verity_block by AW
##
############################################
echo "Generage metadata"
${TARGET_PATH}/dm_merge -m  $TARGET_PATH/sign  $TARGET_PATH/table_tmp  $TARGET_PATH/metadata

echo "Merge verity_block"
dd if=$TARGET_PATH/metadata of=$TARGET_PATH/hash_dev bs=${BLK_SIZE} count=${METADATA_BLK_OFFSET} >/dev/null 2>&1
dd if=$TARGET_PATH/hash of=$TARGET_PATH/hash_dev  bs=${BLK_SIZE} seek=${METADATA_BLK_OFFSET} count=${HASH_CNT}>/dev/null 2>&1
dd of=${OUT} if=$TARGET_PATH/hash_dev bs=${BLK_SIZE} count=`expr ${HASH_CNT} + ${METADATA_BLK_OFFSET}`>/dev/null 2>&1

rm -rf  ${IMG}

echo "Dm-Verity hash tree and metadata ready !"

############################################
## Save Debug files 
###########################################
DEBUG_INFO="./verity_debug"
if [ ! -d  $TARGET_PATH/${DEBUG_INFO} ]; then
	mkdir  $TARGET_PATH/${DEBUG_INFO}
fi

# hash tree
mv -f  $TARGET_PATH/hash  $TARGET_PATH/$DEBUG_INFO
# hash table
mv -f  $TARGET_PATH/raw_table  $TARGET_PATH/$DEBUG_INFO
mv -f  $TARGET_PATH/table_tmp  $TARGET_PATH/$DEBUG_INFO
# hash table signature
mv -f  $TARGET_PATH/sign  $TARGET_PATH/$DEBUG_INFO 
mv -f  $TARGET_PATH/rsa_info  $TARGET_PATH/$DEBUG_INFO
# verity block
mv -f  $TARGET_PATH/metadata  $TARGET_PATH/$DEBUG_INFO
mv -f  $TARGET_PATH/hash_dev  $TARGET_PATH/$DEBUG_INFO

rm -rf $TARGET_PATH/${DEBUG_INFO}

exit 0

