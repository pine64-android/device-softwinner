#ifndef __SUN50IW1P1_H__
#define __SUN50IW1P1_H__
/* cpu spec files defined */
#define CPU0LOCK    "/sys/devices/system/cpu/cpu0/cpufreq/boot_lock"
#define ROOMAGE     "/sys/devices/soc.0/cpu_budget_cool.17/roomage"
#define CPUFREQ     "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq"
#define CPUONLINE   "/sys/devices/system/cpu/online"
#define CPUHOT      "/sys/kernel/autohotplug/enable"
#define CPU0GOV     "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"
/* gpu spec files defined */
#define GPUFREQ     "/sys/devices/1c40000.gpu/dvfs/android"
/* ddr spec files defined */
#define DRAMFREQ    "/sys/class/devfreq/dramfreq/cur_freq"
#define DRAMPAUSE    "/sys/class/devfreq/dramfreq/adaptive/pause"
/* task spec files defined */
#define TASKS       "/dev/cpuctl/tasks"
/* touch screen runtime suspend */
#define TP_SUSPEND  "/sys/devices/soc.0/1c2ac00.twi/i2c-0/0-0040/runtime_suspend"

/*  value define */
#define ROOMAGE_PERF       "816000 4 0 0 1152000 4 0 0 0"
#define ROOMAGE_NORMAL     "0 0 0 0 1152000 4 0 0 0"
#define ROOMAGE_VIDEO      "0 0 0 0 1152000 4 0 0 0"
/* dram scene value defined */
#define DRAM_NORMAL         "0"
#define DRAM_HOME           "1"
#define DRAM_LOCALVIDEO     "2"
#define DRAM_BGMUSIC        "3"
#define DRAM_4KLOCALVIDEO   "4"
/* gpu scene value defined */
#define GPU_NORMAL          "4\n"
#define GPU_HOME            "4\n"
#define GPU_LOCALVIDEO      "4\n"
#define GPU_BGMUSIC         "4\n"
#define GPU_4KLOCALVIDEO    "4\n"
#define GPU_PERF            "8\n"

const static char *roomage_little[] = {
    "0 4 0 0 1152000 4 0 0",
    "1008000 4 0 0 1152000 4 0 0",
    "0 4 0 0 1152000 4 0 0",
    "0 4 0 0 1152000 4 0 0",
    "1008000 4 0 0 1152000 4 0 0",
};
#endif
