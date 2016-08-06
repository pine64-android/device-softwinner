sensor_list_cfg.ini配置
　　考虑到客户需要使用同一个固件支持多种不同camera组合的需求，和以往平台相比，A80重新定义了一套camera detector方案，如果需要使用该方案，需要在sysconfig中作出相应的配置：
　　
    设置相应csi上的vip_define_sensor_list = 1。
　　
    明确定义出前后摄像头，例如vip_dev0_pos = "rear" ，vip_dev1_pos = "front";
　　如果csi0或者csi1定义了vip_define_sensor_list = 1，则驱动就会去试图读取/system/etc/hawkview/sensor_list_cfg.ini，如果读取成功，则驱动会用sensor_list_cfg.ini中的相应信息替换掉原来从sysconfig中读取的信息，如果读取失败，则驱动会继续使用sysconfig中的配置。
　　

   在Android\device\softwinner\kylin-xxx\kylin_xxx.mk文件中按如下方法增加配置:

# camera config for camera detector
PRODUCT_COPY_FILES += \
	device/softwinner/kylin-xxx/hawkview/sensor_list_cfg.ini:system/etc/hawkview/sensor_list_cfg.ini
　　
使用说明：
    1、sensor_list_cfg.ini中整体上分为前置和后置两套camera配置。
    2、每套camera的配置分为bus configs, power configs和sensor configs：

        ①Bus configs：考虑到客户已经习惯在sysconfig中配置相关的bus，在这里暂不配置。

        ②Power configs：该部分可以根据客户或者开发人员需要，通过power_settings_enable来选择使用sysconfig中配置还是sensor_list_cfg.ini中的配置，例如power_settings_enable = 0：代表使用sysconfig中配置，power_settings_enable = 1代表使用sensor_list_cfg.ini中配置。

        ③Sensor configs：考虑到检测速度等方面原因，对前置和后置最大检测数量做出了限制，最大都为3。

    3、各个sensor实体配置比较灵活，可以YUV sensor也可以是RAW sensor，也可以独立配置各自的hflip和vflip。对于RAW sensor也可以独立配置VCM。

注意：
    1、目前驱动不支持对供电电压要求不同的sensor列表做自动检测
    2、驱动也不能检测出相同的sensor使用不同的VCM的情况。
    3、假如一个固件需要兼容多款电压相同的sensor，需要在init.sun9i.rc文件中加载相关的sensor驱动
	如：    ...............................
		insmod /system/vendor/modules/ov5640.ko
		insmod /system/vendor/modules/ov5647.ko
		insmod /system/vendor/modules/gc2035.ko
		......
		insmod /system/vendor/modules/vfe_v4l2.ko