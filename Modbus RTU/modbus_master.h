/**
 * @File Name: modbus_master.h
 * @Brief :
 * @Author : TangYi
 * @Version : 0.1
 * @Creat Date : 2020-09-04
 *
 * modification history :
 * Date:       Version:      Author:      Changes:
 */


#ifndef __MODBUS_MASTER_H__
#define __MODBUS_MASTER_H__

#define BUF_MAX 128;//数据缓存字节数

typedef enum
{
    MB_NONE_ERROR = 0,//正常状态
    MB_ERROR_FUNC = 1,/*非法功能*/
    MB_ERROR_ADDR = 2,/*非法数据地址*/
    MB_ERROR_VALUE = 3,/*非法数据值*/
    MB_ERROR_DEV = 4,/*从站设备故障*/
    MB_ERROR_CONFIRM = 5,
    /*确认:服务器(或从站)已经接受请求，并切正在处理这个请求，
    但是需要长的持续时间进行这些操作。返回这个响应防止在客户机(或主站)中
    发生超时错误。客户机(或主站)可以继续发送轮询程序完成报文来确定是否完
    成处理*/
    MB_ERROR_BUSY = 6,/*从属设备忙*/
    MB_ERROR_PARITY = 8,/*存储奇偶性差错 */
    MB_ERROR_UNAVAILABLE = 10,/*不可用网关路径*/
    MB_ERROR_RESPONSE = 11,/*网关目标设备响应失败*/
    //自定义错误类型
    MB_ERROR_ARGS = -1,//参数填写错误
    MB_ERROR_TIMEOUT = -2, //响应超时
    MB_ERROR_NO_DATA = -3,//返回数据为空
    MB_ERROR_CRC = -4,//CRC校验错误
    MB_ERROR_INIT = -5,//初始化错误

} mb_err;

typedef enum
{
    /*读离散量输入*/
    MB_FUNC_READ_DISCRETE_INPUTS = 1,
    /*读保持寄存器*/
    MB_FUNC_READ_HOLDING_REGISTER =  3,
    /*写单个保持寄存器*/
    MB_FUNC_WRITE_REGISTER    = 6,
    /*写多个寄存器*/
    MB_FUNC_WRITE_MULTIPLE_REGISTERS = 16,

} mb_fuc;

//阻塞模式还是非阻塞模式
typedef enum
{
    MB_MODE_NONE_BLOCK = 0,
    MB_MODE_BLOCK,
} modbus_mode_t;

//运行状态
typedef enum
{
    MB_STATE_INIT = 0,
    MB_STATE_HANDLE,
    MB_STATE_SEND,
    MB_STATE_WAIT_ACK,
    MB_STATE_FINISH
} mb_state_t

typedef struct
{
    unsigned char address;//设备地址
    unsigned char function;//功能码
    unsigned int init_addr;//起始地址
    unsigned int reg_num;//寄存器数量
    unsigned char crc_L；//CRC低位
    unsigned char crc_H；//CRC高位
    unsigned char buf[BUF_MAX];

} mb_data;

typedef struct
{




} modbus_handle

int Modbus_master_init()




#endif

