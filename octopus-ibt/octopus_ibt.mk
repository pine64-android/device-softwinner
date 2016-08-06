$(call inherit-product, build/target/product/full_base.mk)
$(call inherit-product, device/softwinner/octopus-common/octopus-common.mk)
$(call inherit-product-if-exists, device/softwinner/octopus-ibt/modules/modules.mk)

DEVICE_PACKAGE_OVERLAYS := device/softwinner/octopus-ibt/overlay \
                           $(DEVICE_PACKAGE_OVERLAYS)

PRODUCT_PACKAGES += gatord

PRODUCT_PACKAGES += \
    ESFileExplorer \
    VideoPlayer \
    Bluetooth

PRODUCT_COPY_FILES += \
    device/softwinner/octopus-ibt/kernel:kernel \
    device/softwinner/octopus-ibt/fstab.sun8i:root/fstab.sun8i \
    device/softwinner/octopus-ibt/init.sun8i.rc:root/init.sun8i.rc \
    device/softwinner/octopus-ibt/verity/rsa_key/verity_key:root/verity_key \
    device/softwinner/octopus-ibt/init.recovery.sun8i.rc:root/init.recovery.sun8i.rc \
    device/softwinner/octopus-ibt/ueventd.sun8i.rc:root/ueventd.sun8i.rc \
    device/softwinner/octopus-ibt/recovery.fstab:recovery.fstab \
    device/softwinner/octopus-ibt/modules/modules/nand.ko:root/nand.ko \
    device/softwinner/octopus-ibt/modules/modules/sunxi_tr.ko:root/sunxi_tr.ko \
    device/softwinner/octopus-ibt/modules/modules/disp.ko:root/disp.ko \
    device/softwinner/octopus-ibt/modules/modules/sw-device.ko:obj/sw-device.ko \
    device/softwinner/octopus-ibt/modules/modules/gt9xxf_ts.ko:obj/gt9xxf_ts.ko \

PRODUCT_COPY_FILES += \
    device/softwinner/octopus-ibt/configs/camera.cfg:system/etc/camera.cfg \
    device/softwinner/octopus-ibt/configs/cfg-Gallery2.xml:system/etc/cfg-Gallery2.xml \
    device/softwinner/octopus-ibt/configs/media_profiles.xml:system/etc/media_profiles.xml \
	device/softwinner/octopus-ibt/configs/sunxi-ir.kl:system/usr/keylayout/sunxi-ir.kl \
    device/softwinner/octopus-ibt/configs/tp.idc:system/usr/idc/tp.idc \
	device/softwinner/octopus-ibt/configs/gsensor.cfg:system/usr/gsensor.cfg \
	device/softwinner/octopus-ibt/configs/sunxi-keyboard.kl:system/usr/keylayout/sunxi-keyboard.kl \

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
    device/softwinner/octopus-ibt/bluetooth/bt_vendor.conf:system/etc/bluetooth/bt_vendor.conf

PRODUCT_COPY_FILES += \
    device/softwinner/octopus-ibt/initlogo.rle:root/initlogo.rle  \
	device/softwinner/octopus-ibt/bootlogo.bmp:system/media/bootlogo.bmp \

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml   \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml

# Low mem(memory <= 512M) device should not copy android.software.managed_users.xml
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.managed_users.xml:system/etc/permissions/android.software.managed_users.xml

#PRODUCT_COPY_FILES += \
#	device/softwinner/octopus-ibt/media/bootanimation.zip:system/media/bootanimation.zip \

#camera config for camera detector
PRODUCT_COPY_FILES += \
	device/softwinner/octopus-ibt/hawkview/sensor_list_cfg.ini:system/etc/hawkview/sensor_list_cfg.ini

# Radio Packages and Configuration Flie
$(call inherit-product, device/softwinner/common/rild/radio_common.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/huawei/mu509/huawei_mu509.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/Oviphone/em55/oviphone_em55.mk)

## camera config for isp
#PRODUCT_COPY_FILES += \
#    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/isp_3a_param.ini:system/etc/hawkview/ov8858_4lane/isp_3a_param.ini \
#    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/isp_iso_param.ini:system/etc/hawkview/ov8858_4lane/isp_iso_param.ini \
#    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/isp_test_param.ini:system/etc/hawkview/ov8858_4lane/isp_test_param.ini \
#    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/isp_tuning_param.ini:system/etc/hawkview/ov8858_4lane/isp_tuning_param.ini \
#    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/bin/gamma_tbl.bin:system/etc/hawkview/ov8858_4lane/bin/gamma_tbl.bin \
#    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/bin/hdr_tbl.bin:system/etc/hawkview/ov8858_4lane/bin/hdr_tbl.bin \
#    device/softwinner/octopus-common/hawkview/8M/ov8858_4lane/bin/lsc_tbl.bin:system/etc/hawkview/ov8858_4lane/bin/lsc_tbl.bin


RODUCT_PACKAGES += Bluetooth
#rtl8723bs bt fw and config
$(call inherit-product, hardware/realtek/bluetooth/rtl8723bs/firmware/rtlbtfw_cfg.mk)

#sensor    
PRODUCT_COPY_FILES += \
	device/softwinner/octopus-ibt/sensor.sh:system/bin/sensor.sh
  
## gps
#$(call inherit-product, device/softwinner/octopus-ibt/gps/gps.mk)

# usb
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp,adb \
    ro.adb.secure=0 \
    ro.sys.mutedrm=false \
    rw.logger=0

PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.firmware=v3.0rc1

PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapsize=512m \
    dalvik.vm.heapstartsize=8m \
    dalvik.vm.heapgrowthlimit=192m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.heapminfree=2m \
    dalvik.vm.heapmaxfree=8m \
    ro.zygote.disable_gl_preload=true

PRODUCT_PROPERTY_OVERRIDES += \
    ro.property.tabletUI=false \
    
PRODUCT_PROPERTY_OVERRIDES += \
	persist.sys.language=en	\
	persist.sys.country=US \
	ro.sf.lcd_density=120 \
	ro.sf.rotation=270 \
	ro.inet.wlan.type=8723bs_vq0 \
	ro.product.8723b_bt.used=true \

# function
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sys.bootfast=true \
#    ro.dmic.used=true

#PRODUCT_PROPERTY_OVERRIDES += \
#    persist.sys.timezone=Asia/Shanghai \
#    persist.sys.country=CN \
#   persist.sys.language=zh

PRODUCT_AAPT_CONFIG := large xlarge hdpi xhdpi
PRODUCT_AAPT_PERF_CONFIG := xhdpi
PRODUCT_CHARACTERISTICS := tablet

$(call inherit-product-if-exists, vendor/google/products/gms_base.mk)

PRODUCT_BRAND := Inet
PRODUCT_NAME := octopus_ibt
PRODUCT_DEVICE := octopus-ibt
PRODUCT_MODEL := N1021L1BC
PRODUCT_BOARD := N102
PRODUCT_MANUFACTURER := Allwinner