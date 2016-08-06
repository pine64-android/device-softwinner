$(call inherit-product, build/target/product/full_base.mk)
$(call inherit-product, device/softwinner/octopus-common/octopus-common.mk)
$(call inherit-product-if-exists, device/softwinner/octopus-n1/modules/modules.mk)

DEVICE_PACKAGE_OVERLAYS := device/softwinner/octopus-n1/overlay \
                           $(DEVICE_PACKAGE_OVERLAYS)

# Abandon useless system app. Add which module name in apk/Android.mk octopus_n1_app section.
PRODUCT_PACKAGES += gatord

PRODUCT_PACKAGES += \
    ESFileExplorer \
    VideoPlayer \
    Bluetooth
    
# for recovery
PRODUCT_COPY_FILES += \
    device/softwinner/octopus-n1/kernel:kernel \
    device/softwinner/octopus-n1/fstab.sun8i:root/fstab.sun8i \
    device/softwinner/octopus-n1/init.sun8i.rc:root/init.sun8i.rc \
    device/softwinner/octopus-n1/init.recovery.sun8i.rc:root/init.recovery.sun8i.rc \
    device/softwinner/octopus-n1/ueventd.sun8i.rc:root/ueventd.sun8i.rc \
    device/softwinner/octopus-n1/recovery.fstab:recovery.fstab \
    device/softwinner/octopus-n1/modules/modules/nand.ko:root/nand.ko \
    device/softwinner/octopus-n1/modules/modules/sunxi_tr.ko:root/sunxi_tr.ko \
    device/softwinner/octopus-n1/modules/modules/disp.ko:root/disp.ko \
    device/softwinner/octopus-n1/modules/modules/sw-device.ko:obj/sw-device.ko \
    device/softwinner/octopus-n1/modules/modules/gt9xxnew_ts.ko:obj/gt9xxnew_ts.ko \

PRODUCT_COPY_FILES += \
    device/softwinner/octopus-n1/configs/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml

PRODUCT_COPY_FILES += \
    device/softwinner/octopus-n1/configs/camera.cfg:system/etc/camera.cfg \
    device/softwinner/octopus-n1/configs/cfg-Gallery2.xml:system/etc/cfg-Gallery2.xml \
    device/softwinner/octopus-n1/configs/gsensor.cfg:system/usr/gsensor.cfg \
    device/softwinner/octopus-n1/configs/media_profiles.xml:system/etc/media_profiles.xml \
    device/softwinner/octopus-n1/configs/sunxi-keyboard.kl:system/usr/keylayout/sunxi-keyboard.kl \
	  device/softwinner/octopus-n1/configs/sunxi-ir.kl:system/usr/keylayout/sunxi-ir.kl \
    device/softwinner/octopus-n1/configs/tp.idc:system/usr/idc/tp.idc

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml \
    frameworks/native/data/etc/android.hardware.ethernet.xml:system/etc/permissions/android.hardware.ethernet.xml

PRODUCT_COPY_FILES += \
    device/softwinner/octopus-n1/bluetooth/bt_vendor.conf:system/etc/bluetooth/bt_vendor.conf
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml   \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml

# Low mem(memory <= 512M) device should not copy android.software.managed_users.xml
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.managed_users.xml:system/etc/permissions/android.software.managed_users.xml

PRODUCT_COPY_FILES += \
	device/softwinner/octopus-n1/media/bootanimation.zip:system/media/bootanimation.zip \

#camera config for camera detector
PRODUCT_COPY_FILES += \
        device/softwinner/octopus-n1/hawkview/sensor_list_cfg.ini:system/etc/hawkview/sensor_list_cfg.ini

# Radio Packages and Configuration Flie
$(call inherit-product, device/softwinner/common/rild/radio_common.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/huawei/mu509/huawei_mu509.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/Oviphone/em55/oviphone_em55.mk)

# camera config for isp
PRODUCT_COPY_FILES += \
    device/softwinner/octopus-n1/hawkview/5M/ov5648/isp_3a_param.ini:system/etc/hawkview/ov5648/isp_3a_param.ini \
    device/softwinner/octopus-n1/hawkview/5M/ov5648/isp_iso_param.ini:system/etc/hawkview/ov5648/isp_iso_param.ini \
    device/softwinner/octopus-n1/hawkview/5M/ov5648/isp_test_param.ini:system/etc/hawkview/ov5648/isp_test_param.ini \
    device/softwinner/octopus-n1/hawkview/5M/ov5648/isp_tuning_param.ini:system/etc/hawkview/ov5648/isp_tuning_param.ini \
    device/softwinner/octopus-n1/hawkview/5M/ov5648/bin/gamma_tbl.bin:system/etc/hawkview/ov5648/bin/gamma_tbl.bin \
    device/softwinner/octopus-n1/hawkview/5M/ov5648/bin/hdr_tbl.bin:system/etc/hawkview/ov5648/bin/hdr_tbl.bin \
    device/softwinner/octopus-n1/hawkview/5M/ov5648/bin/lsc_tbl.bin:system/etc/hawkview/ov5648/bin/lsc_tbl.bin


#sensor
PRODUCT_COPY_FILES += \
  device/softwinner/octopus-n1/sensor.sh:system/bin/sensor.sh

# gps
$(call inherit-product, device/softwinner/octopus-n1/gps/gps.mk)




# When set ro.sys.adaptive_memory=1, firmware can adaptive different dram size.
# And dalvik vm parameters configuration will become invalid.

PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapsize=384m \
    dalvik.vm.heapstartsize=8m \
    dalvik.vm.heapgrowthlimit=192m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.heapminfree=512k \
    dalvik.vm.heapmaxfree=8m \
    ro.zygote.disable_gl_preload=true \
    ro.memopt.disable=true

PRODUCT_PROPERTY_OVERRIDES += \
    ro.hwui.texture_cache_size=72 \
    ro.hwui.layer_cache_size=48 \
    ro.hwui.gradient_cache_size=1 \
    ro.hwui.path_cache_size=32 \
    ro.hwui.shap_cache_size=3 \
    ro.hwui.drop_shadow_cache_size=6 \
    ro.hwui.r_buffer_cache_size=8 \
    ro.hwui.text_small_cache_height=1024 \
    ro.hwui.text_small_cache_width=1024 \
    ro.hwui.text_large_cache_height=1024 \
    ro.hwui.text_large_cache_width=2048 \
    ro.hwui.texture_cache_flushrate=0.4

# usb
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp,adb \
    ro.adb.secure=0         \
    ro.sys.mutedrm=true     \
    rw.logger=1
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
PRODUCT_NAME := octopus_n1
PRODUCT_DEVICE := octopus-n1
PRODUCT_MODEL :=  Octopus A83 N1
PRODUCT_MANUFACTURER := Allwinner


