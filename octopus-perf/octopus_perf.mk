$(call inherit-product, build/target/product/full_base.mk)
$(call inherit-product, device/softwinner/octopus-common/octopus-common.mk)
$(call inherit-product-if-exists, device/softwinner/octopus-perf/modules/modules.mk)

DEVICE_PACKAGE_OVERLAYS := device/softwinner/octopus-perf/overlay \
                           $(DEVICE_PACKAGE_OVERLAYS)

PRODUCT_PACKAGES += gatord

PRODUCT_PACKAGES += \
    ESFileExplorer \
    VideoPlayer \
    Bluetooth

PRODUCT_PACKAGES += \
    gps.exdroid \



PRODUCT_COPY_FILES += \
	device/softwinner/octopus-perf/configs/gps.conf:system/etc/gps.conf
	
PRODUCT_COPY_FILES += \
    device/softwinner/octopus-perf/kernel:kernel \
    device/softwinner/octopus-perf/fstab.sun8i:root/fstab.sun8i \
    device/softwinner/octopus-perf/init.sun8i.rc:root/init.sun8i.rc \
    device/softwinner/octopus-perf/init.recovery.sun8i.rc:root/init.recovery.sun8i.rc \
    device/softwinner/octopus-perf/ueventd.sun8i.rc:root/ueventd.sun8i.rc \
    device/softwinner/octopus-perf/recovery.fstab:recovery.fstab \
    device/softwinner/octopus-perf/modules/modules/nand.ko:root/nand.ko \
    device/softwinner/octopus-perf/modules/modules/sunxi_tr.ko:root/sunxi_tr.ko \
    device/softwinner/octopus-perf/modules/modules/disp.ko:root/disp.ko \
    device/softwinner/octopus-perf/modules/modules/sw-device.ko:obj/sw-device.ko \

PRODUCT_COPY_FILES += \
    device/softwinner/octopus-perf/configs/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml

PRODUCT_COPY_FILES += \
    device/softwinner/octopus-perf/configs/camera.cfg:system/etc/camera.cfg \
    device/softwinner/octopus-perf/configs/cfg-Gallery2.xml:system/etc/cfg-Gallery2.xml \
    device/softwinner/octopus-perf/configs/gsensor.cfg:system/usr/gsensor.cfg \
    device/softwinner/octopus-perf/configs/media_profiles.xml:system/etc/media_profiles.xml \
    device/softwinner/octopus-perf/configs/sunxi-keyboard.kl:system/usr/keylayout/sunxi-keyboard.kl \
	device/softwinner/octopus-perf/configs/sunxi-ir.kl:system/usr/keylayout/sunxi-ir.kl \
    device/softwinner/octopus-perf/configs/tp.idc:system/usr/idc/tp.idc

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.ethernet.xml:system/etc/permissions/android.hardware.ethernet.xml

PRODUCT_COPY_FILES += \
    device/softwinner/octopus-perf/bluetooth/bt_vendor.conf:system/etc/bluetooth/bt_vendor.conf


PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml   \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml
        
# Low mem(memory <= 512M) device should not copy android.software.managed_users.xml
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.managed_users.xml:system/etc/permissions/android.software.managed_users.xml

PRODUCT_COPY_FILES += \
	device/softwinner/octopus-perf/media/bootanimation.zip:system/media/bootanimation.zip \

#camera config for camera detector
PRODUCT_COPY_FILES += \
        device/softwinner/octopus-perf/hawkview/sensor_list_cfg.ini:system/etc/hawkview/sensor_list_cfg.ini

# Radio Packages and Configuration Flie
$(call inherit-product, device/softwinner/common/rild/radio_common.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/huawei/mu509/huawei_mu509.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/Oviphone/em55/oviphone_em55.mk)

#sensor    
PRODUCT_COPY_FILES += \
	device/softwinner/octopus-perf/sensor.sh:system/bin/sensor.sh



# When set ro.sys.adaptive_memory=1, firmware can adaptive different dram size.
# And dalvik vm parameters configuration will become invalid.

# dalvik vm parameters
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapsize=512m \
    dalvik.vm.heapstartsize=8m \
    dalvik.vm.heapgrowthlimit=192m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.heapminfree=2m \
    dalvik.vm.heapmaxfree=8m \
    ro.zygote.disable_gl_preload=true


# usb
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp,adb \
    ro.adb.secure=0         \
    ro.sys.mutedrm=true     \
    rw.logger=0
    #ro.udisk.lable=octopus \
# ui
PRODUCT_PROPERTY_OVERRIDES += \
    ro.property.tabletUI=false \
    ro.sf.lcd_density=160 \
    ro.property.fontScale=1.0 \
    ro.sf.hwrotation=0 
    
PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.firmware=v2.0rc5

# function
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sys.bootfast=true \
    ro.dmic.used=false

# default language setting
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.timezone=Asia/Shanghai \
    persist.sys.country=CN \
    persist.sys.language=zh


PRODUCT_PROPERTY_OVERRIDES += \
	persist.sys.layer0usefe=0


PRODUCT_AAPT_CONFIG := large xlarge hdpi xhdpi
PRODUCT_AAPT_PERF_CONFIG := xhdpi

# Overrides
PRODUCT_CHARACTERISTICS := tablet

$(call inherit-product-if-exists, vendor/google/products/gms_aw.mk)

PRODUCT_BRAND := Allwinner
PRODUCT_NAME := octopus_perf
PRODUCT_DEVICE := octopus-perf
PRODUCT_MODEL := UltraOcta A83 perf
PRODUCT_MANUFACTURER := Allwinner

