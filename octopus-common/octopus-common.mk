# inherit common.mk
$(call inherit-product, device/softwinner/common/common.mk)

DEVICE_PACKAGE_OVERLAYS := \
	device/softwinner/octopus-common/overlay \
	$(DEVICE_PACKAGE_OVERLAYS)
PRODUCT_PACKAGES += \
	hwcomposer.octopus \
	lights.octopus \
	camera.octopus

# camera libs
PRODUCT_PACKAGES += \
        libcnr              \
        libhdr              \
        libproc             \
        libfacedetection    \
        libsmileeyeblink    \
        libapperceivepeople
	
# face detection
PRODUCT_COPY_FILES += \
	device/softwinner/octopus-common/facedetection/sm.awl:system/usr/share/bmd/sm.awl 

# audio
PRODUCT_PACKAGES += \
	audio.primary.octopus \
	audio.a2dp.default \
	audio.usb.default \
	audio.r_submix.default \
	libaudioroute \
	libaudio-resampler

PRODUCT_PACKAGES +=\
	charger_res_images \
	charger

PRODUCT_COPY_FILES += \
	hardware/aw/audio/octopus/audio_policy.conf:system/etc/audio_policy.conf \
	hardware/aw/audio/octopus/phone_volume.conf:system/etc/phone_volume.conf \
	hardware/aw/audio/octopus/ac100_paths.xml:system/etc/ac100_paths.xml     \
	
PRODUCT_COPY_FILES += \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:system/etc/media_codecs_google_telephony.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:system/etc/media_codecs_google_video.xml
	
PRODUCT_COPY_FILES += \
	device/softwinner/octopus-common/media_codecs.xml:system/etc/media_codecs.xml \
	device/softwinner/octopus-common/media_codecs_performance.xml:system/etc/media_codecs_performance.xml \
	device/softwinner/octopus-common/init.sun8i.usb.rc:root/init.sun8i.usb.rc \
       device/softwinner/octopus-common/init.sun8i.common.rc:root/init.sun8i.common.rc \
	device/softwinner/octopus-common/configs/cfg-videoplayer.xml:system/etc/cfg-videoplayer.xml \
	frameworks/native/data/etc/android.hardware.location.xml:system/etc/permissions/android.hardware.location.xml \
	frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
	frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml

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
	libawplayer		 \
	libawh264		\
	libawh265		\
	libawmjpeg		\
	libawmjpegplus		\
	libawmpeg2		\
	libawmpeg4base		\
	libawvp6soft		\
	libawvp8		\
	libawwmv3  		\
	libawvp9soft		\
	libawh265soft		\
	libawmpeg4h263		\
	libawmpeg4vp6		\
	libawmpeg4normal	\
	libawmpeg4dx    	\
	libawwmv12soft 		\
	libawavs		
	

# egl
PRODUCT_COPY_FILES += \
	device/softwinner/octopus-common/egl/pvrsrvctl:system/vendor/bin/pvrsrvctl \
	device/softwinner/octopus-common/egl/libusc.so:system/vendor/lib/libusc.so \
	device/softwinner/octopus-common/egl/libglslcompiler.so:system/vendor/lib/libglslcompiler.so \
	device/softwinner/octopus-common/egl/libIMGegl.so:system/vendor/lib/libIMGegl.so \
	device/softwinner/octopus-common/egl/libpvr2d.so:system/vendor/lib/libpvr2d.so \
	device/softwinner/octopus-common/egl/libpvrANDROID_WSEGL.so:system/vendor/lib/libpvrANDROID_WSEGL.so \
	device/softwinner/octopus-common/egl/libPVRScopeServices.so:system/vendor/lib/libPVRScopeServices.so \
	device/softwinner/octopus-common/egl/libsrv_init.so:system/vendor/lib/libsrv_init.so \
	device/softwinner/octopus-common/egl/libsrv_um.so:system/vendor/lib/libsrv_um.so \
	device/softwinner/octopus-common/egl/libEGL_POWERVR_SGX544_115.so:system/vendor/lib/egl/libEGL_POWERVR_SGX544_115.so \
	device/softwinner/octopus-common/egl/libGLESv1_CM_POWERVR_SGX544_115.so:system/vendor/lib/egl/libGLESv1_CM_POWERVR_SGX544_115.so \
	device/softwinner/octopus-common/egl/libGLESv2_POWERVR_SGX544_115.so:system/vendor/lib/egl/libGLESv2_POWERVR_SGX544_115.so \
	device/softwinner/octopus-common/egl/gralloc.sun8i.so:system/vendor/lib/hw/gralloc.sun8i.so \
	device/softwinner/octopus-common/egl/powervr.ini:system/etc/powervr.ini

PRODUCT_PROPERTY_OVERRIDES += \
	wifi.interface=wlan0 \
	wifi.supplicant_scan_interval=15 \
	keyguard.no_require_sim=true

PRODUCT_PROPERTY_OVERRIDES += \
	ro.kernel.android.checkjni=0

# 131072=0x20000 196608=0x30000
PRODUCT_PROPERTY_OVERRIDES += \
	ro.opengles.version=131072
	
PRODUCT_PROPERTY_OVERRIDES += \
	persist.sys.strictmode.visual=0 \
	persist.sys.strictmode.disable=1

PRODUCT_PROPERTY_OVERRIDES += \
	ro.sys.cputype=UltraOcta-A83

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
BOARD_WIDEVINE_OEMCRYPTO_LEVEL := 1
SECURE_OS_OPTEE := no

# widevine
PRODUCT_PROPERTY_OVERRIDES += \
    drm.service.enabled=true \
    ro.sys.widevine_oemcrypto_level=1

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
    PRODUCT_PACKAGES += liboemcrypto \
                        libtee_client
endif
