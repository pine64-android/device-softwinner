#!/system/bin/sh

if [ ! -e /data/system.notfirstrun.precopy ]; then
    cp -rfp /system/precopy/* /data/app/
    chown system:system /data/app/*
    touch /data/system.notfirstrun.precopy
fi

