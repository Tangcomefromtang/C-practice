/**
 * @File Name: hj212.h
 * @Brief : HJ212协议实现，目前只支持上传模式
 * @Author : TangYi
 * @Version : 0.1
 * @Creat Date : 2020-09-30
 *
 * Creater:
 * modification history :
 * Date:       Version:      Author:      Changes:
 */

#ifndef __HJ212_H_
#define __HJ212_H_

#define E212_MONI_MAX       30//最大支持监测因子个数

typedef struct
{
    u8 e212_enable;//使能开关
    u8 system_type;//系统编码
    u8 function_code;//功能码
    u8 device_address;//设备地址
    u8 monitor_time_sec;
    u8 monitor_time_min;
    u8 monitor_time_hour;
    u8 monitor_time_day;
    u8 monitor_num;//监测因子个数
    u32 monitor_time_all;//总定时时间
    u8 password[8];//访问密码
    u8 device_code[25];//设备唯一标识码
} E212_CONFIG;//整体的配置

typedef struct
{
    u8 accuracy;//精度
    u8 data_type;//数据类型
    u8 register_num;//寄存器个数
    u16 register_address;//寄存器地址
    u8 monitor_factor[10];//监测因子
} E212_MONI;//每个监测因子的数据配置

typedef struct
{
    u16 u16data;
    u8 ascii[10];
} E212_DATA;//存储转换的数据

typedef struct
{
    u8 msec;//毫秒，0~999
    u8 sec;//0~59
    u8 min;//0~59
    u8 hour;//0~23
    u8 day;//1~31
    u8 month;//1~12
    int year;

} E212_TIME;//时间数据

void HJ212_init(E212_CONFIG *config, E212_MONI *moni, E212_DATA *data);
void HJ212_time_get();
u8 HJ212_handle(u8 *Sdata, u8 *HJdata);

#endif
