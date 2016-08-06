#!/sbin/busybox sh
BUSYBOX="/sbin/busybox"
FILE_TAR="/system/priv-data/data_backup.tar"

mount -t vfat /dev/block/by-name/bootloader /bootloader

echo "On satart if [ ! -e /bootloader/data.notfirstrun ]; then"
if [ ! -e /bootloader/data.notfirstrun ]; then
    echo "Ceadte data.notfirstrun start"


    if [ -f $FILE_TAR ]; then
        $BUSYBOX echo "$FILE_TAR is exist,bengin to resume data"
        $BUSYBOX tar -xf $FILE_TAR -C ../../
    else
        $BUSYBOX echo "$FILE_TAR  is not exist,do nothing and return"
    fi
    $BUSYBOX touch /bootloader/data.notfirstrun

    echo "Ceadte data.notfirstrun finish"
fi
echo "tar -xf $FILE_TAR -C ./ end "

