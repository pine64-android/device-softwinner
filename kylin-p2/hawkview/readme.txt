sensor_list_cfg.ini����
�������ǵ��ͻ���Ҫʹ��ͬһ���̼�֧�ֶ��ֲ�ͬcamera��ϵ����󣬺�����ƽ̨��ȣ�A80���¶�����һ��camera detector�����������Ҫʹ�ø÷�������Ҫ��sysconfig��������Ӧ�����ã�
����
    ������Ӧcsi�ϵ�vip_define_sensor_list = 1��
����
    ��ȷ�����ǰ������ͷ������vip_dev0_pos = "rear" ��vip_dev1_pos = "front";
�������csi0����csi1������vip_define_sensor_list = 1���������ͻ�ȥ��ͼ��ȡ/system/etc/hawkview/sensor_list_cfg.ini�������ȡ�ɹ�������������sensor_list_cfg.ini�е���Ӧ��Ϣ�滻��ԭ����sysconfig�ж�ȡ����Ϣ�������ȡʧ�ܣ������������ʹ��sysconfig�е����á�
����

   ��Android\device\softwinner\kylin-xxx\kylin_xxx.mk�ļ��а����·�����������:

# camera config for camera detector
PRODUCT_COPY_FILES += \
	device/softwinner/kylin-xxx/hawkview/sensor_list_cfg.ini:system/etc/hawkview/sensor_list_cfg.ini
����
ʹ��˵����
    1��sensor_list_cfg.ini�������Ϸ�Ϊǰ�úͺ�������camera���á�
    2��ÿ��camera�����÷�Ϊbus configs, power configs��sensor configs��

        ��Bus configs�����ǵ��ͻ��Ѿ�ϰ����sysconfig��������ص�bus���������ݲ����á�

        ��Power configs���ò��ֿ��Ը��ݿͻ����߿�����Ա��Ҫ��ͨ��power_settings_enable��ѡ��ʹ��sysconfig�����û���sensor_list_cfg.ini�е����ã�����power_settings_enable = 0������ʹ��sysconfig�����ã�power_settings_enable = 1����ʹ��sensor_list_cfg.ini�����á�

        ��Sensor configs�����ǵ�����ٶȵȷ���ԭ�򣬶�ǰ�úͺ���������������������ƣ����Ϊ3��

    3������sensorʵ�����ñȽ�������YUV sensorҲ������RAW sensor��Ҳ���Զ������ø��Ե�hflip��vflip������RAW sensorҲ���Զ�������VCM��

ע�⣺
    1��Ŀǰ������֧�ֶԹ����ѹҪ��ͬ��sensor�б����Զ����
    2������Ҳ���ܼ�����ͬ��sensorʹ�ò�ͬ��VCM�������
    3������һ���̼���Ҫ���ݶ���ѹ��ͬ��sensor����Ҫ��init.sun9i.rc�ļ��м�����ص�sensor����
	�磺    ...............................
		insmod /system/vendor/modules/ov5640.ko
		insmod /system/vendor/modules/ov5647.ko
		insmod /system/vendor/modules/gc2035.ko
		......
		insmod /system/vendor/modules/vfe_v4l2.ko