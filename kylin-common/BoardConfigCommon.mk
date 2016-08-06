TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_VARIANT := cortex-a15

TARGET_NO_BOOTLOADER := true

TARGET_BOARD_PLATFORM := kylin
TARGET_BOOTLOADER_BOARD_NAME := exdroid
TARGET_BOOTLOADER_NAME := exdroid

BOARD_EGL_CFG := device/softwinner/kylin-common/egl/egl.cfg
BOARD_KERNEL_BASE := 0x20000000

BOARD_CHARGER_ENABLE_SUSPEND := true

BOARD_SEPOLICY_DIRS := \
    device/softwinner/kylin-common/sepolicy

BOARD_SEPOLICY_UNION := \
	genfs_contexts \
	wpa.te \
	bluetooth.te \
	debuggerd.te \
	device.te \
	file_contexts \
	init.te \
	kernel.te \
	netd.te \
	pvrsrvctl.te \
        rild.te \
	surfaceflinger.te \
	service_contexts \
	system_server.te \
	unconfined.te \
	mediaserver.te \
	preinstall.te \
	recovery.te \
	sensors.te \
	shell.te \
	system_app.te \
	vold.te  \
	untrusted_app.te \
	platform_app.te

USE_OPENGL_RENDERER := true
