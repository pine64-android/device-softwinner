$(call inherit-product, device/softwinner/tulip-common/tulip_64_bit.mk)
$(call inherit-product, build/target/product/full_base.mk)
$(call inherit-product, device/softwinner/tulip-common/tulip-common.mk)
$(call inherit-product-if-exists, device/softwinner/tulip-perf/modules/modules.mk)

DEVICE_PACKAGE_OVERLAYS := device/softwinner/tulip-perf/overlay \
                           $(DEVICE_PACKAGE_OVERLAYS)

PRODUCT_PACKAGES += \
    ESFileExplorer \
    VideoPlayer \
    Bluetooth

PRODUCT_COPY_FILES += \
    device/softwinner/tulip-perf/kernel:kernel \
    device/softwinner/tulip-perf/fstab.sun50iw1p1:root/fstab.sun50iw1p1 \
    device/softwinner/tulip-perf/init.sun50iw1p1.rc:root/init.sun50iw1p1.rc \
    device/softwinner/tulip-perf/init.recovery.sun50iw1p1.rc:root/init.recovery.sun50iw1p1.rc \
    device/softwinner/tulip-perf/ueventd.sun50iw1p1.rc:root/ueventd.sun50iw1p1.rc \
    device/softwinner/tulip-perf/recovery.fstab:recovery.fstab \
    device/softwinner/tulip-perf/modules/modules/nand.ko:root/nand.ko \
    device/softwinner/tulip-perf/modules/modules/sunxi_tr.ko:root/sunxi_tr.ko \
    device/softwinner/tulip-perf/modules/modules/disp.ko:root/disp.ko \
    device/softwinner/tulip-perf/modules/modules/sunxi-keyboard.ko:obj/sunxi-keyboard.ko \
    device/softwinner/tulip-perf/modules/modules/sw-device.ko:obj/sw-device.ko \
    device/softwinner/tulip-perf/modules/modules/gslX680new.ko:obj/gslX680new.ko \

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.ethernet.xml:system/etc/permissions/android.hardware.ethernet.xml

#camera config for camera detector
PRODUCT_COPY_FILES += \
        device/softwinner/octopus-f1/hawkview/sensor_list_cfg.ini:system/etc/hawkview/sensor_list_cfg.ini

# camera config for isp
PRODUCT_COPY_FILES += \
    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/isp_3a_param.ini:system/etc/hawkview/ov8858_4lane/isp_3a_param.ini \
    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/isp_iso_param.ini:system/etc/hawkview/ov8858_4lane/isp_iso_param.ini \
    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/isp_test_param.ini:system/etc/hawkview/ov8858_4lane/isp_test_param.ini \
    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/isp_tuning_param.ini:system/etc/hawkview/ov8858_4lane/isp_tuning_param.ini \
    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/bin/gamma_tbl.bin:system/etc/hawkview/ov8858_4lane/bin/gamma_tbl.bin \
    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/bin/hdr_tbl.bin:system/etc/hawkview/ov8858_4lane/bin/hdr_tbl.bin \
    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/bin/lsc_tbl.bin:system/etc/hawkview/ov8858_4lane/bin/lsc_tbl.bin

PRODUCT_COPY_FILES += \
    device/softwinner/tulip-perf/configs/camera.cfg:system/etc/camera.cfg \
    device/softwinner/tulip-perf/configs/gsensor.cfg:system/usr/gsensor.cfg \
    device/softwinner/tulip-perf/configs/media_profiles.xml:system/etc/media_profiles.xml \
    device/softwinner/tulip-perf/configs/sunxi-keyboard.kl:system/usr/keylayout/sunxi-keyboard.kl \
    device/softwinner/tulip-perf/configs/tp.idc:system/usr/idc/tp.idc

PRODUCT_COPY_FILES += \
    device/softwinner/tulip-perf/bluetooth/bt_vendor.conf:system/etc/bluetooth/bt_vendor.conf

# bootanimation
PRODUCT_COPY_FILES += \
    device/softwinner/tulip-perf/media/bootanimation.zip:system/media/bootanimation.zip

# Radio Packages and Configuration Flie
$(call inherit-product, device/softwinner/common/rild/radio_common.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/huawei/mu509/huawei_mu509.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/Oviphone/em55/oviphone_em55.mk)

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp,adb \
    ro.adb.secure=0 \
    rw.logger=1

PRODUCT_PROPERTY_OVERRIDES += \
    ro.zygote.disable_gl_preload=false

PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=213 \

PRODUCT_PROPERTY_OVERRIDES += \
    ro.spk_dul.used=false \

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.timezone=Asia/Shanghai \
    persist.sys.country=CN \
    persist.sys.language=zh

PRODUCT_CHARACTERISTICS := tablet

PRODUCT_AAPT_CONFIG := tvdpi hdpi large
PRODUCT_AAPT_PREF_CONFIG := tvdpi

#include device/softwinner/common/prebuild/google/products/gms_base.mk

PRODUCT_BRAND := Allwinner
PRODUCT_NAME := tulip_perf
PRODUCT_DEVICE := tulip-perf
PRODUCT_MODEL := QUAD-CORE A64 perf
PRODUCT_MANUFACTURER := Allwinner
