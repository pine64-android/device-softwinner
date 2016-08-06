$(call inherit-product, build/target/product/full_base.mk)
$(call inherit-product, device/softwinner/kylin-common/kylin-common.mk)
$(call inherit-product-if-exists, device/softwinner/kylin-p2/modules/modules.mk)

DEVICE_PACKAGE_OVERLAYS := device/softwinner/kylin-p2/overlay \
                           $(DEVICE_PACKAGE_OVERLAYS)

PRODUCT_PACKAGES += \
    ESFileExplorer \
    VideoPlayer \
    Bluetooth

PRODUCT_COPY_FILES += \
    device/softwinner/kylin-p2/kernel:kernel \
    device/softwinner/kylin-p2/fstab.sun9i:root/fstab.sun9i \
    device/softwinner/kylin-p2/init.sun9i.rc:root/init.sun9i.rc \
    device/softwinner/kylin-p2/init.recovery.sun9i.rc:root/init.recovery.sun9i.rc \
    device/softwinner/kylin-p2/ueventd.sun9i.rc:root/ueventd.sun9i.rc \
    device/softwinner/kylin-p2/recovery.fstab:recovery.fstab \
    device/softwinner/kylin-p2/modules/modules/nand.ko:root/nand.ko \
    device/softwinner/kylin-p2/modules/modules/disp.ko:root/disp.ko \
    device/softwinner/kylin-p2/modules/modules/lcd.ko:root/lcd.ko \
    device/softwinner/kylin-p2/modules/modules/sw-device.ko:obj/sw-device.ko \
    device/softwinner/kylin-p2/modules/modules/gt9xxnew_ts.ko:obj/gt9xxnew_ts.ko

PRODUCT_COPY_FILES += \
    device/softwinner/kylin-p2/configs/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml

# camera config for camera detector
PRODUCT_COPY_FILES += \
	device/softwinner/kylin-p2/hawkview/sensor_list_cfg.ini:system/etc/hawkview/sensor_list_cfg.ini

# Radio Packages and Configuration Flie
$(call inherit-product, device/softwinner/common/rild/radio_common.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/huawei/mu509/huawei_mu509.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/Oviphone/em55/oviphone_em55.mk)

PRODUCT_COPY_FILES += \
    device/softwinner/kylin-p2/configs/camera.cfg:system/etc/camera.cfg \
    device/softwinner/kylin-p2/configs/cfg-Gallery2.xml:system/etc/cfg-Gallery2.xml \
    device/softwinner/kylin-p2/configs/gsensor.cfg:system/usr/gsensor.cfg \
    device/softwinner/kylin-p2/configs/media_profiles.xml:system/etc/media_profiles.xml \
    device/softwinner/kylin-p2/configs/sunxi-keyboard.kl:system/usr/keylayout/sunxi-keyboard.kl \
	device/softwinner/kylin-p2/configs/sunxi-ir.kl:system/usr/keylayout/sunxi-ir.kl \
    device/softwinner/kylin-p2/configs/tp.idc:system/usr/idc/gt9xx.idc

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml   \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.ethernet.xml:system/etc/permissions/android.hardware.ethernet.xml \
	frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
	frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml
    
 # camera config for isp
PRODUCT_COPY_FILES += \
    device/softwinner/kylin-common/hawkview/8M/ov8858_4lane/isp_3a_param.ini:system/etc/hawkview/ov8858_4lane/isp_3a_param.ini \
    device/softwinner/kylin-common/hawkview/8M/ov8858_4lane/isp_iso_param.ini:system/etc/hawkview/ov8858_4lane/isp_iso_param.ini \
    device/softwinner/kylin-common/hawkview/8M/ov8858_4lane/isp_test_param.ini:system/etc/hawkview/ov8858_4lane/isp_test_param.ini \
    device/softwinner/kylin-common/hawkview/8M/ov8858_4lane/isp_tuning_param.ini:system/etc/hawkview/ov8858_4lane/isp_tuning_param.ini \
    device/softwinner/kylin-common/hawkview/8M/ov8858_4lane/bin/gamma_tbl.bin:system/etc/hawkview/ov8858_4lane/bin/gamma_tbl.bin \
    device/softwinner/kylin-common/hawkview/8M/ov8858_4lane/bin/hdr_tbl.bin:system/etc/hawkview/ov8858_4lane/bin/hdr_tbl.bin \
    device/softwinner/kylin-common/hawkview/8M/ov8858_4lane/bin/lsc_tbl.bin:system/etc/hawkview/ov8858_4lane/bin/lsc_tbl.bin

PRODUCT_COPY_FILES += \
    device/softwinner/kylin-p2/bluetooth/bt_vendor.conf:system/etc/bluetooth/bt_vendor.conf
	
# Low mem(memory <= 512M) device should not copy android.software.managed_users.xml
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.managed_users.xml:system/etc/permissions/android.software.managed_users.xml

# bootanimation
PRODUCT_COPY_FILES += \
	device/softwinner/kylin-p2/media/bootanimation.zip:system/media/bootanimation.zip

PRODUCT_PROPERTY_OVERRIDES += \
    ro.sys.storage_type = emulated

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp,adb \
    ro.adb.secure=0

PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapsize=512m \
    dalvik.vm.heapstartsize=8m \
    dalvik.vm.heapgrowthlimit=192m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.heapminfree=2m \
    dalvik.vm.heapmaxfree=8m \
    ro.zygote.disable_gl_preload=true

PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=320

PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.firmware=v2.1rc3

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.timezone=Asia/Shanghai \
    persist.sys.country=CN \
    persist.sys.language=zh

PRODUCT_CHARACTERISTICS := tablet

PRODUCT_AAPT_CONFIG := xlarge hdpi xhdpi large
PRODUCT_AAPT_PREF_CONFIG := xhdpi

$(call inherit-product-if-exists, vendor/google/products/gms_aw.mk)

PRODUCT_BRAND := Allwinner
PRODUCT_NAME := kylin_p2
PRODUCT_DEVICE := kylin-p2
PRODUCT_MODEL := UltraOcta A80 p2
PRODUCT_MANUFACTURER := Allwinner
