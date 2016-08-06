# inherit common.mk
$(call inherit-product, device/softwinner/common/common.mk)

DEVICE_PACKAGE_OVERLAYS := \
    device/softwinner/astar-common/overlay \
    $(DEVICE_PACKAGE_OVERLAYS)

PRODUCT_PACKAGES += \
	hwcomposer.astar \
	camera.astar \
	lights.astar

PRODUCT_PACKAGES += \
	libion

# audio
PRODUCT_PACKAGES += \
	audio.primary.astar \
	audio.a2dp.default \
	audio.usb.default \
	audio.r_submix.default

PRODUCT_PACKAGES +=\
	charger_res_images \
	charger
	
PRODUCT_COPY_FILES += \
	hardware/aw/audio/astar/audio_policy.conf:system/etc/audio_policy.conf \
	hardware/aw/audio/astar/phone_volume.conf:system/etc/phone_volume.conf

PRODUCT_COPY_FILES += \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:system/etc/media_codecs_google_telephony.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video_le.xml:system/etc/media_codecs_google_video_le.xml

PRODUCT_COPY_FILES += \
	device/softwinner/astar-common/media_codecs.xml:system/etc/media_codecs.xml \
	device/softwinner/astar-common/init.sun8i.usb.rc:root/init.sun8i.usb.rc \
	device/softwinner/astar-common/configs/cfg-videoplayer.xml:system/etc/cfg-videoplayer.xml

PRODUCT_COPY_FILES += \
    device/softwinner/common/config/android.hardware.sensor.temperature.ambient.xml:system/etc/permissions/android.hardware.sensor.temperature.ambient.xml
	
# video libs
PRODUCT_PACKAGES += \
  libMemAdapter          \
	libcdx_base              \
	libcdx_stream            \
	libnormal_audio          \
	libcdx_parser            \
	libVE                    \
	libvdecoder              \
	libvencoder              \
	libadecoder              \
	libsdecoder              \
	libplayer                \
	libad_audio              \
	libaw_plugin             \
	libthumbnailplayer       \
	libaw_wvm                \
	libstagefrighthw         \
	libOmxCore               \
	libOmxVdec               \
	libOmxVenc               \
	libI420colorconvert      \
	libawmetadataretriever   \
	libawplayer      
	
# egl
PRODUCT_COPY_FILES += \
    device/softwinner/astar-common/egl/egl.cfg:system/lib/egl/egl.cfg \
    device/softwinner/astar-common/egl/gralloc.default.so:system/lib/hw/gralloc.default.so \
    device/softwinner/astar-common/egl/libGLES_mali.so:system/lib/egl/libGLES_mali.so \

PRODUCT_PROPERTY_OVERRIDES += \
	wifi.interface=wlan0 \
	wifi.supplicant_scan_interval=15 \
	keyguard.no_require_sim=true

PRODUCT_PROPERTY_OVERRIDES += \
	ro.kernel.android.checkjni=0

# 131072=0x20000 196608=0x30000
PRODUCT_PROPERTY_OVERRIDES += \
	ro.opengles.version=131072
	
# For mali GPU only
PRODUCT_PROPERTY_OVERRIDES += \
	debug.hwui.render_dirty_regions=false

PRODUCT_PROPERTY_OVERRIDES += \
	ro.sys.cputype=QuadCore-A33

# Enabling type-precise GC results in larger optimized DEX files.  The
# additional storage requirements for ".odex" files can cause /system
# to overflow on some devices, so this is configured separately for
# each product.
PRODUCT_TAGS += dalvik.gc.type-precise

PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.firmware=v3.3rc5

# if DISPLAY_BUILD_NUMBER := true then
# BUILD_DISPLAY_ID := $(BUILD_ID).$(BUILD_NUMBER)
# required by gms.
DISPLAY_BUILD_NUMBER := true
BUILD_NUMBER := $(shell date +%Y%m%d)

# widevine
BOARD_WIDEVINE_OEMCRYPTO_LEVEL := 3

#add widevine libraries
PRODUCT_PROPERTY_OVERRIDES += \
        drm.service.enabled=true

PRODUCT_PACKAGES += \
    com.google.widevine.software.drm.xml \
    com.google.widevine.software.drm \
    libdrmwvmplugin \
    libwvm \
    libWVStreamControlAPI_L${BOARD_WIDEVINE_OEMCRYPTO_LEVEL} \
    libwvdrm_L${BOARD_WIDEVINE_OEMCRYPTO_LEVEL} \
    libdrmdecrypt \
    libwvdrmengine

ifeq ($(BOARD_WIDEVINE_OEMCRYPTO_LEVEL), 1)
PRODUCT_PACKAGES += \
    liboemcrypto \
    libtee_client
endif
