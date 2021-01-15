/**
 * @File Name: hj212.h
 * @Brief : HJ212-2017 协议的实现
 * @Author : ty (tangt_t@foxmail.com)
 * @Version : 1.0
 * @Creat Date : 2021-01-15
 * 
 */

#ifndef __HJ212_H__
#define __HJ212_H__

// 数据类型重定义
typedef signed long    s32;
typedef signed short   s16;
typedef signed char     s8;
typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char   u8;

// HJ212相关参数
typedef struct{
    u8 e212_enable;
    u8 system_type; // ST
    u8 function_code; //
    u8 device_address; //
    u8 monitor_time_sec;
    u8 monitor_time_min;
    u8 monitor_time_hour;
    u8 monitor_time_day;
    u8 monitor_num; //监测因子个数
    u32 monitor_time_all;
    u8 password[8]; // PW
    u8 device_code[25]; // MN
}E212_ARGS;

// 请求命令返回表
typedef enum{
    QnRtn_ready = '1',
    QnRtn_rejected = '2',
    QnRtn_err_pw = '3',
    QnRtn_err_mn = '4',
    QnRtn_err_st = '5',
    QnRtn_err_flag = '6',
    QnRtn_err_qn = '7',
    QnRtn_err_cn = '8',
    QnRtn_err_crc = '9',
    QnRtn_err_unknow = 100 // 注意这里是数值100

}QnRtn_code;

// 执行结果定义表
typedef enum{
    ExeRtn_success = '1',
    ExeRtn_err_unknow = '2',
    ExeRtn_err_condition = '3',
    ExeRtn_err_timeout = '4',
    ExeRtn_err_busy = '5',
    ExeRtn_err_fault = '6',
    ExeRtn_err_nodata = 100 // 注意这里是数值100

}ExeRtn_code;


// 函数声明ver
u16 hj212_crc16(u8 *puchMsg, u16 usDataLen);
s8  hj212_check_mn(const u8* pkt,const u8* mn);
s8  hj212_check_pw(const u8* pkt,const u8* pw);
s16 hj212_ver_cn(const u8* pkt);
s8  hj212_ver_QnRtn(const u8* pkt,E212_ARGS* args);
s8  hj212_pkt_getadress(const u8* pkt, u16* address);
s8  hj212_pkt_getvalue(const u8* pkt, u16* value);



#endif 