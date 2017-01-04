# inherit tools.mk
$(call inherit-product, device/softwinner/common/prebuild/tools/tools.mk)

DEVICE_PACKAGE_OVERLAYS := \
    device/softwinner/common/overlay

PRODUCT_COPY_FILES += \
    device/softwinner/common/init.common.rc:root/init.common.rc \
    device/softwinner/common/init.sensors.rc:root/init.sensors.rc \
    device/softwinner/common/ueventd.common.rc:root/ueventd.common.rc \

ifeq ($(TARGET_BUILD_VARIANT),eng)
PRODUCT_PROPERTY_OVERRIDES += \
       dalvik.vm.image-dex2oat-filter="" \
       dalvik.vm.dex2oat-filter=""
endif

# preinstall apk
PRODUCT_PACKAGES += \
    DragonFire \
    DragonPhone \
    DragonAging \
	
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

#wifi/bt mac
PRODUCT_PACKAGES += \
    setmacaddr \
    setbtmacaddr

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
    frameworks/native/data/etc/android.software.midi.xml:system/etc/permissions/android.software.midi.xml

PRODUCT_PACKAGES += \
    sensors.exdroid \
    keystore.exdroid \
    sayeye \
    sdc

PRODUCT_PACKAGES += \
    SoundRecorder
#    LoggerService \
#    LogCopyService \
#    GooglePinyinIME

PRODUCT_COPY_FILES += \
    device/softwinner/common/config/config_mem.ini:root/config_mem.ini

