#!/system/bin/sh
FILE_TAR="/system/priv-data/data_backup.tar"

echo "On satart if [ ! -e /bootloader/data.notfirstrun ]; then"
if [ ! -e /bootloader/data.notfirstrun ]; then
    echo "Ceadte data.notfirstrun start"

    if [ -f $FILE_TAR ]; then
        echo "$FILE_TAR is exist,bengin to resume data"
        tar -xf $FILE_TAR -C ../../
    else
        echo "$FILE_TAR  is not exist,do nothing and return"
    fi
    touch /bootloader/data.notfirstrun

    echo "Ceadte data.notfirstrun finish"
fi
echo "tar -xf $FILE_TAR -C ./ end "

