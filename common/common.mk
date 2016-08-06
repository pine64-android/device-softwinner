# inherit tools.mk
$(call inherit-product, device/softwinner/common/prebuild/tools/tools.mk)

DEVICE_PACKAGE_OVERLAYS := \
    device/softwinner/common/overlay

PRODUCT_COPY_FILES += \
    device/softwinner/common/init_parttion.sh:root/sbin/init_parttion.sh \
    device/softwinner/common/init.common.rc:root/init.common.rc \
	device/softwinner/common/sensors.sh:system/bin/sensors.sh

ifeq ($(TARGET_BUILD_VARIANT),eng)
PRODUCT_PROPERTY_OVERRIDES += \
       dalvik.vm.image-dex2oat-filter="" \
       dalvik.vm.dex2oat-filter=""
endif

# preinstall apk
# PRODUCT_PACKAGES += \
    DragonFire \
    DragonPhone \
    DragonAging
	
# usb
PRODUCT_PACKAGES += \
    com.android.future.usb.accessory

# wifi
PRODUCT_PACKAGES += \
    libwpa_client \
    hostapd \
    dhcpcd.conf \
    wpa_supplicant \
    wpa_supplicant.conf

# xml
PRODUCT_COPY_FILES += \
    device/softwinner/common/config/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.location.xml:system/etc/permissions/android.hardware.location.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \

PRODUCT_PACKAGES += \
    sensors.exdroid \
    keystore.exdroid \
    sayeye \
    sdc

PRODUCT_PACKAGES += \
    SoundRecorder \
    LoggerService \
    LogCopyService \
    GooglePinyinIME

PRODUCT_COPY_FILES += \
    device/softwinner/common/config/config_mem.ini:root/config_mem.ini

PRODUCT_COPY_FILES += \
	$(call find-copy-subdir-files,*.so,device/softwinner/common/library-crack/lib32,system/lib)

PRODUCT_COPY_FILES += \
	$(call find-copy-subdir-files,*.so,device/softwinner/common/library-crack/lib64,system/lib64)


# add for start bg service at the same time
PRODUCT_PROPERTY_OVERRIDES += \
    ro.config.max_starting_bg=8
