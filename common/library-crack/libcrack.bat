@echo off
set local_path=%cd%

adb remount
adb push %local_path%\lib32\libad_audio.so                /system/lib
adb push %local_path%\lib32\libaxx.so                     /system/lib
adb push %local_path%\lib32\libdxx.so                     /system/lib
adb push %local_path%\lib32\librv.so                      /system/lib
adb push %local_path%\lib32\librx.so                      /system/lib
adb push %local_path%\lib64\libad_audio.so                /system/lib64
adb push %local_path%\lib64\libaxx.so                     /system/lib64
adb push %local_path%\lib64\libdxx.so                     /system/lib64
adb push %local_path%\lib64\librv.so                      /system/lib64
adb push %local_path%\lib64\librx.so                      /system/lib64
adb shell sync 
pause