对于RAW sensor如果使用A80的ISP模块, 需要增加了hawkview的配置, 所有sensor的ISP配置文件放在下面目录中:
	android\device\softwinner\kylin-common\hawkview

Hawkview中每个sensor的配置包括3个bin文件和4个ini文件:
./ov5647
  |---bin
  |     |---gamma_tbl.bin
  |     |---lsc_tbl.bin
  |     |---hdr_tbl.bin
  |---isp_3a_param.ini
  |---isp_iso_param.ini
  |---isp_test_param.ini
  |---isp_tuning_param.ini

　　如果使用ov5647，如果发现有问题，可以检查机器中的目录：/system/etc/hawkview/ov5647/ 看是否打包成功。
　　
在Android\device\softwinner\kylin-xxx\kylin_xxx.mk文件中按如下方法增加配置:
　　
对于单路RAW摄像头（以后置ov5647为例，适用于只使用一个raw sensor）：
# camera config for isp
PRODUCT_COPY_FILES += \
	device/softwinner/kylin-common/hawkview/5M/ov5647/isp_3a_param.ini:system/etc/hawkview/ov5647/isp_3a_param.ini \
	device/softwinner/kylin-common/hawkview/5M/ov5647/isp_iso_param.ini:system/etc/hawkview/ov5647/isp_iso_param.ini \
	device/softwinner/kylin-common/hawkview/5M/ov5647/isp_test_param.ini:system/etc/hawkview/ov5647/isp_test_param.ini \
	device/softwinner/kylin-common/hawkview/5M/ov5647/isp_tuning_param.ini:system/etc/hawkview/ov5647/isp_tuning_param.ini \
	device/softwinner/kylin-common/hawkview/5M/ov5647/bin/gamma_tbl.bin:system/etc/hawkview/ov5647/bin/gamma_tbl.bin \
	device/softwinner/kylin-common/hawkview/5M/ov5647/bin/hdr_tbl.bin:system/etc/hawkview/ov5647/bin/hdr_tbl.bin \
	device/softwinner/kylin-common/hawkview/5M/ov5647/bin/lsc_tbl.bin:system/etc/hawkview/ov5647/bin/lsc_tbl.bin	
　　
双路RAW摄像头（以后置ov5647，前置gc2235为例，适用于使用两个raw sensor）：
# camera config for isp
PRODUCT_COPY_FILES += \
	device/softwinner/kylin-common/hawkview/5M/ov5647/isp_3a_param.ini:system/etc/hawkview/ov5647/isp_3a_param.ini \
	device/softwinner/kylin-common/hawkview/5M/ov5647/isp_iso_param.ini:system/etc/hawkview/ov5647/isp_iso_param.ini \
	device/softwinner/kylin-common/hawkview/5M/ov5647/isp_test_param.ini:system/etc/hawkview/ov5647/isp_test_param.ini \
	device/softwinner/kylin-common/hawkview/5M/ov5647/isp_tuning_param.ini:system/etc/hawkview/ov5647/isp_tuning_param.ini \
	device/softwinner/kylin-common/hawkview/5M/ov5647/bin/gamma_tbl.bin:system/etc/hawkview/ov5647/bin/gamma_tbl.bin \
	device/softwinner/kylin-common/hawkview/5M/ov5647/bin/hdr_tbl.bin:system/etc/hawkview/ov5647/bin/hdr_tbl.bin \
	device/softwinner/kylin-common/hawkview/5M/ov5647/bin/lsc_tbl.bin:system/etc/hawkview/ov5647/bin/lsc_tbl.bin	

	device/softwinner/kylin-common/hawkview/2M/gc2235/isp_3a_param.ini:system/etc/hawkview/gc2235/isp_3a_param.ini \
	device/softwinner/kylin-common/hawkview/2M/gc2235/isp_iso_param.ini:system/etc/hawkview/gc2235/isp_iso_param.ini \
	device/softwinner/kylin-common/hawkview/2M/gc2235/isp_test_param.ini:system/etc/hawkview/gc2235/isp_test_param.ini \
	device/softwinner/kylin-common/hawkview/2M/gc2235/isp_tuning_param.ini:system/etc/hawkview/gc2235/isp_tuning_param.ini \
	device/softwinner/kylin-common/hawkview/2M/gc2235/bin/gamma_tbl.bin:system/etc/hawkview/gc2235/bin/gamma_tbl.bin \
	device/softwinner/kylin-common/hawkview/2M/gc2235/bin/hdr_tbl.bin:system/etc/hawkview/gc2235/bin/hdr_tbl.bin \
	device/softwinner/kylin-common/hawkview/2M/gc2235/bin/lsc_tbl.bin:system/etc/hawkview/gc2235/bin/lsc_tbl.bin	
	
　　
注意：hawkview的配置在使用raw sensor时候才需要进行配置并打包到固件中，除非AW做出修改，请勿自行改动，否则影响效果。
	对应不同的raw sensor，在使用不同模组厂生产的模组时候，由于镜头/马达外围材料等不同，camera.ini里面的参数均需要经过调试之后才会达到最好的效果，否则可能出现各种异常。RAW格式的sensor调试工作暂时由AW内部进行，不对外部自行调试做支持。