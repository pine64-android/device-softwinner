$(call inherit-product, build/target/product/full_base.mk)
$(call inherit-product, device/softwinner/astar-common/astar-common.mk)
$(call inherit-product-if-exists, device/softwinner/astar-yh/modules/modules.mk)

DEVICE_PACKAGE_OVERLAYS := device/softwinner/astar-yh/overlay \
                           $(DEVICE_PACKAGE_OVERLAYS)

PRODUCT_PACKAGES += \
    ESFileExplorer \
    VideoPlayer \
    Bluetooth

PRODUCT_COPY_FILES += \
    device/softwinner/astar-yh/kernel:kernel \
    device/softwinner/astar-yh/fstab.sun8i:root/fstab.sun8i \
    device/softwinner/astar-yh/init.sun8i.rc:root/init.sun8i.rc \
    device/softwinner/astar-yh/init.recovery.sun8i.rc:root/init.recovery.sun8i.rc \
    device/softwinner/astar-yh/ueventd.sun8i.rc:root/ueventd.sun8i.rc \
    device/softwinner/astar-yh/recovery.fstab:recovery.fstab \
    device/softwinner/astar-yh/modules/modules/nand.ko:root/nand.ko \
    device/softwinner/astar-yh/modules/modules/disp.ko:obj/disp.ko \
    device/softwinner/astar-yh/modules/modules/lcd.ko:obj/lcd.ko \
    device/softwinner/astar-yh/modules/modules/sunxi-keyboard.ko:obj/sunxi-keyboard.ko \
    device/softwinner/astar-yh/modules/modules/sw-device.ko:obj/sw-device.ko \
    device/softwinner/astar-yh/modules/modules/gslX680new.ko:obj/gslX680new.ko

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.ethernet.xml:system/etc/permissions/android.hardware.ethernet.xml \
	frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml

PRODUCT_COPY_FILES += \
    device/softwinner/astar-yh/configs/camera.cfg:system/etc/camera.cfg \
    device/softwinner/astar-yh/configs/gsensor.cfg:system/usr/gsensor.cfg \
    device/softwinner/astar-yh/configs/media_profiles.xml:system/etc/media_profiles.xml \
    device/softwinner/astar-yh/configs/sunxi-keyboard.kl:system/usr/keylayout/sunxi-keyboard.kl \
    device/softwinner/astar-yh/configs/tp.idc:system/usr/idc/tp.idc

# bootanimation
PRODUCT_COPY_FILES += \
    device/softwinner/astar-yh/media/bootanimation.zip:system/media/bootanimation.zip

# Radio Packages and Configuration Flie
$(call inherit-product, device/softwinner/common/rild/radio_common.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/huawei/mu509/huawei_mu509.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/Oviphone/em55/oviphone_em55.mk)

#esp8089 wifi firmware
#$(call inherit-product-if-exists, hardware/espressif/wlan/firmware/esp8089/device-esp.mk)

#rtl8723bs bt fw and config
$(call inherit-product, hardware/realtek/bluetooth/rtl8723bs/firmware/rtlbtfw_cfg.mk)

PRODUCT_PROPERTY_OVERRIDES += \
    ro.sys.storage_type = emulated \
	ro.product.8723b_bt.used = true

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.usb.config=mtp \
    ro.adb.secure=1 \
    ro.sys.mutedrm=true \
	  rw.logger=0
	

PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapsize=384m \
    dalvik.vm.heapstartsize=8m \
    dalvik.vm.heapgrowthlimit=64m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.heapminfree=512k \
    dalvik.vm.heapmaxfree=8m \
    ro.zygote.disable_gl_preload=false

PRODUCT_PROPERTY_OVERRIDES += \
    ro.sf.lcd_density=160 \

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.timezone=Asia/Shanghai \
    persist.sys.country=CN \
    persist.sys.language=zh

PRODUCT_CHARACTERISTICS := tablet

PRODUCT_AAPT_CONFIG := mdpi large
PRODUCT_AAPT_PREF_CONFIG := mdpi

$(call inherit-product-if-exists, vendor/google/products/gms_min.mk)

PRODUCT_BRAND := Allwinner
PRODUCT_NAME := astar_yh
PRODUCT_DEVICE := astar-yh
PRODUCT_MODEL := QUAD-CORE A33 yh
PRODUCT_MANUFACTURER := Allwinner
