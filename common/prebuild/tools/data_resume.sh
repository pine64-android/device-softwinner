#!/sbin/busybox sh

BUSYBOX="/sbin/busybox"

$BUSYBOX mkdir /bootloader
$BUSYBOX mount -t vfat /dev/block/by-name/bootloader /bootloader
if [ -e /bootloader/data.notfirstrun ]; then
    $BUSYBOX rm /bootloader/data.notfirstrun
fi
$BUSYBOX umount /bootloader
$BUSYBOX rmdir /bootloader
$BUSYBOX echo "resume data finish"
