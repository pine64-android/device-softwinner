# inherit common.mk
$(call inherit-product, device/softwinner/common/common.mk)

DEVICE_PACKAGE_OVERLAYS := \
    device/softwinner/kylin-common/overlay \
    $(DEVICE_PACKAGE_OVERLAYS)

PRODUCT_PACKAGES += \
	hwcomposer.kylin \
	lights.kylin \
	camera.kylin

# audio
PRODUCT_PACKAGES += \
	audio.primary.kylin \
	audio.a2dp.default \
	audio.usb.default \
	audio.r_submix.default

PRODUCT_PACKAGES += \
	charger_res_images \
	charger

PRODUCT_COPY_FILES += \
	hardware/aw/audio/kylin/audio_policy.conf:system/etc/audio_policy.conf \
	hardware/aw/audio/kylin/phone_volume.conf:system/etc/phone_volume.conf

PRODUCT_COPY_FILES += \
	device/softwinner/kylin-common/media_codecs.xml:system/etc/media_codecs.xml \
	device/softwinner/kylin-common/init.sun9i.usb.rc:root/init.sun9i.usb.rc \
	frameworks/native/data/etc/android.hardware.location.xml:system/etc/permissions/android.hardware.location.xml \
	frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
	frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \

# video libs
PRODUCT_PACKAGES += \
	libMemAdapter \
	libcdx_base \
	libcdx_stream \
	libnormal_audio \
	libcdx_parser \
	libVE \
	libvdecoder \
	libvencoder \
	libadecoder \
	libsdecoder \
	libplayer \
	libad_audio \
	libaw_plugin \
	libthumbnailplayer \
	libaw_wvm \
	libstagefrighthw \
	libOmxCore \
	libOmxVdec \
	libOmxVenc \
	libI420colorconvert \
	libawmetadataretriever \
	libawplayer

PRODUCT_COPY_FILES += \
    device/softwinner/kylin-common/egl/pvrsrvctl:system/vendor/bin/pvrsrvctl \
    device/softwinner/kylin-common/egl/libusc.so:system/vendor/lib/libusc.so \
    device/softwinner/kylin-common/egl/libglslcompiler.so:system/vendor/lib/libglslcompiler.so \
    device/softwinner/kylin-common/egl/libIMGegl.so:system/vendor/lib/libIMGegl.so \
    device/softwinner/kylin-common/egl/libpvrANDROID_WSEGL.so:system/vendor/lib/libpvrANDROID_WSEGL.so \
    device/softwinner/kylin-common/egl/libPVRScopeServices.so:system/vendor/lib/libPVRScopeServices.so \
    device/softwinner/kylin-common/egl/libsrv_init.so:system/vendor/lib/libsrv_init.so \
    device/softwinner/kylin-common/egl/libsrv_um.so:system/vendor/lib/libsrv_um.so \
    device/softwinner/kylin-common/egl/libEGL_POWERVR_ROGUE.so:system/vendor/lib/egl/libEGL_POWERVR_ROGUE.so \
    device/softwinner/kylin-common/egl/libGLESv1_CM_POWERVR_ROGUE.so:system/vendor/lib/egl/libGLESv1_CM_POWERVR_ROGUE.so \
    device/softwinner/kylin-common/egl/libGLESv2_POWERVR_ROGUE.so:system/vendor/lib/egl/libGLESv2_POWERVR_ROGUE.so \
    device/softwinner/kylin-common/egl/gralloc.sunxi.so:system/vendor/lib/hw/gralloc.sun9i.so \
    device/softwinner/kylin-common/egl/powervr.ini:system/etc/powervr.ini

PRODUCT_PROPERTY_OVERRIDES += \
	wifi.interface=wlan0 \
	wifi.supplicant_scan_interval=15 \
	keyguard.no_require_sim=true

PRODUCT_PROPERTY_OVERRIDES += \
	ro.kernel.android.checkjni=0

PRODUCT_PROPERTY_OVERRIDES += \
	ro.opengles.version=196608
	
PRODUCT_PROPERTY_OVERRIDES += \
	ro.sys.cputype=UltraOcta-A80

# Enabling type-precise GC results in larger optimized DEX files.  The
# additional storage requirements for ".odex" files can cause /system
# to overflow on some devices, so this is configured separately for
# each product.
PRODUCT_TAGS += dalvik.gc.type-precise

# if DISPLAY_BUILD_NUMBER := true then
# BUILD_DISPLAY_ID := $(BUILD_ID).$(BUILD_NUMBER)
# required by gms.
DISPLAY_BUILD_NUMBER := true
BUILD_NUMBER := $(shell date +%Y%m%d)

# widevine
BOARD_WIDEVINE_OEMCRYPTO_LEVEL := 3

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
