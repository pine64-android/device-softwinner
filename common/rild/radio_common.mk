# 3G Data Card Packages
PRODUCT_PACKAGES += \
	chat \
	pppd \
    rild

# 3G Data Card Configuration Flie
PRODUCT_COPY_FILES += \
	device/softwinner/common/rild/ip-down:system/etc/ppp/ip-down \
	device/softwinner/common/rild/ip-up:system/etc/ppp/ip-up \
	device/softwinner/common/rild/3g_dongle.cfg:system/etc/3g_dongle.cfg \
	device/softwinner/common/rild/usb_modeswitch:system/bin/usb_modeswitch \
	device/softwinner/common/rild/call-pppd:system/xbin/call-pppd \
	device/softwinner/common/rild/usb_modeswitch.sh:system/xbin/usb_modeswitch.sh \
	device/softwinner/common/rild/apns-conf_sdk.xml:system/etc/apns-conf.xml \
	device/softwinner/common/rild/lib/libsoftwinner-ril-6.0.so:system/lib/libsoftwinner-ril-6.0.so\
	device/softwinner/common/rild/lib64/libsoftwinner-ril-6.0.so:system/lib64/libsoftwinner-ril-6.0.so

# 3G Data Card usb modeswitch File
PRODUCT_COPY_FILES += \
	$(call find-copy-subdir-files,*,device/softwinner/common/rild/usb_modeswitch.d,system/etc/usb_modeswitch.d)

# Radio parameter
PRODUCT_PROPERTY_OVERRIDES += \
	rild.libargs=-d/dev/ttyUSB2 \
	rild.libpath=libsoftwinner-ril-6.0.so \
	ro.sw.embeded.telephony=false
