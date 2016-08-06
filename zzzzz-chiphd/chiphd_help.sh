#!/bin/bash
############################################################
# show help
function help-chiphd() {
show_vit "chiphd script[$CHIPHD_THIS_SCRIPT_VER]:"
cat <<EOF
functions from device/softwinner/zzzzz-chiphd/vendorsetup.sh
source build/envsetup.sh --nosync for not auto sync default-remote-branch
- update-chiphd-script update chiphd scripts, recommend source again
- chiphd-bsp:          update public linux kernel.
- chiphd-tagOut:       output current repo-git log and status.
- quicklyClean:        rm \$OUT/system \$OUT/root, with -a to rm more files.
- shgrep:              Greps on all local .sh .mk files.
- allgrep:             Greps on all local .c .cc .cpp .h .java .xml .sh .mk files.
- showfile:            e.g. showfile FileName 20 30   -- show the 20th to 30th lines of the "FileName" file.
- patch_chiphd_am:     auto git am patch
- patch_chiphd_ap:     auto git apply patch
- tar_repo_committed   /* tar files committed by git-log command */
     e.g. tar_repo_committed --committer=blue        /*tar all files are committed by blue on current branch*/
          tar_repo_committed --committer=blue  --since="30 days ago"    /*tar files : blue, since 30 days ago,
                                                                         but merge-files may be lost.*/
- tar_repo_now_modify  /* tar modified(not committed now) files */
------------------------
adb-tips, just show for copy to cmd windows:
adb reboot                                              -- reboot
adb install [apk]                                       -- install apk
adb pull /system/vendor/modules/                        -- pull .ko file
adb push  /system/vendor/modules                        -- push .ko file
adb push  /system/etc/permissions                       -- push permission file
adb push  /system/etc                                   -- push some cfg file
adb shell cat /proc/kmsg                                -- print kernel debug info
          cat /proc/meminfo                             -- print meminfo
          cat /proc/cpuinfo                             -- print cpuinfo
          cat /proc/version                             -- print version
adb shell input text "xw26614116888"                    -- input text
adb shell input keyevent 7                              -- input key (7:KEYCODE_0, 29:KEYCODE_A)
adb shell getevent                                      -- get event
adb shell sendevent [device] [type] [code] [value]      -- send event
adb shell am start -n [packageName/className] -a [action] -d [data] -m [MIME-TYPE] -c [category] -e [ext-data]
     e.g. am start -n com.android.settings/com.android.settings.Settings     ---- open setting.apk
          am start -a android.intent.action.VIEW -d http://www.sohu.com      ---- open http://www.sohu.com

EOF
}
#############################################################
## end for this script file
#############################################################
