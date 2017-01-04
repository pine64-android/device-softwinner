@echo off
adb push mtop32 /data/mtop
adb shell chmod 755 /data/mtop
adb shell /data/mtop -m

pause
