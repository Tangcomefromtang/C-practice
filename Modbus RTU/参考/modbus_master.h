/*****************************************************************************
* File Name      : modbus_master.h
* Author         : Inhand Embedded Team
* Description    : 
* 					简单的Modbus Master
* 
* Copyright      : 
* Website        : www.inhand.com.cn
******************************************************************************
* modification history :
* zhaocj created on Mon Sep 03 2018 09:42:10 GMT+0800 (中国标准时间)
* zhaocj modified on Tue Sep 04 2018 14:56:29 GMT+0800 (中国标准时间)
******************************************************************************/
/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __MODBUS_MASTER_H__
#define __MODBUS_MASTER_H__
/* Includes -----------------------------------------------------------------*/
/* Exported types -----------------------------------------------------------*/

/*MODBUS读写回调*/
typedef int (*mb_write_callback_t)(const unsigned char*, unsigned int);
typedef int (*mb_read_callback_t)(unsigned char*,unsigned int);

/*Modbus 功能码*/
typedef enum{
	/*读线圈*/
	MB_FUNC_READ_COILS = 0,
	/*读离散量输入*/
	MB_FUNC_READ_DISCRETE_INPUTS = 1,
	/*读保持寄存器*/
	MB_FUNC_READ_HOLDING_REGISTER=  3,
	/*读输入寄存器*/
	MB_FUNC_READ_INPUT_REGISTER  =  4,
	/*写单个线圈*/
	MB_FUNC_WRITE_SINGLE_COIL = 5,
	/*写多个线圈*/
	MB_FUNC_WRITE_MULTIPLE_COILS = 15,
	/*写单个保持寄存器*/
	MB_FUNC_WRITE_REGISTER    = 6,
	/*写多个寄存器*/
	MB_FUNC_WRITE_MULTIPLE_REGISTERS = 16,
}mb_func_t;

/*modbus错误码*/
typedef enum{
	MB_ERROR_NONE = 0,
	/*非法功能*/
	MB_ERROR_FUNC = 1,
	/*非法数据地址*/
	MB_ERROR_ADDR = 2,
	/*非法数据值*/
	MB_ERROR_VALUE = 3,
	/*从站设备故障*/
	MB_ERROR_DEV = 4,
	/*确认:服务器(或从站)已经接受请求，并切正在处理这个请求，
但是需要长的持续时间进行这些操作。返回这个响应防止在客户机(或主站)中
发生超时错误。客户机(或主站)可以继续发送轮询程序完成报文来确定是否完
成处理*/
	MB_ERROR_CONFIRM = 5,
	/*从属设备忙*/
	MB_ERROR_BUSY = 6,
	/*存储奇偶性差错 */
	MB_ERROR_PARITY = 8,
	/*不可用网关路径*/
	MB_ERROR_UNAVAILABLE = 10,
	/*网关目标设备响应失败*/
	MB_ERROR_RESPONSE = 11,
	MB_ERROR_ARGS = -1,
	MB_ERROR_TIMEOUT = -2,
	MB_ERROR_NO_DATA = -3,
	MB_ERROR_CRC = -4,
}mb_error_t;

typedef struct {
	unsigned char func;
	/*Modbus 执行结果*/
	mb_error_t err_code;
	/*从机回复的数据长度*/
	unsigned short data_len;
	/*从机回复的数据*/
	const unsigned char *data;
}mb_msg_t;

/*modbus master event callback*/
//typedef int (*mb_callback_t)(mb_msg_t *);
typedef enum{
	MB_MODE_NONE_BLOCK = 0,
	MB_MODE_BLOCK,
}modbus_mode_t;
/*Modbus master state*/
typedef enum{
	MB_STATE_IDLE = 0,
	MB_STATE_SEND,
	MB_STATE_WAIT_ACK,
	MB_STATE_FINISH
}mb_state_t;
typedef struct{
	mb_state_t state;
	unsigned long timestamp;
	unsigned long timeout;
	unsigned int receive_len;
	unsigned int send_len;
	mb_msg_t ack;
	unsigned char buffer[50];
	mb_write_callback_t write;
	mb_read_callback_t read;
	//mb_callback_t event;
}modbus_handle_t;
/* Exported constants -------------------------------------------------------*/
/* Exported macro -----------------------------------------------------------*/
/* Exported functions -------------------------------------------------------*/
void modbus_master_init(modbus_handle_t *phandle,mb_write_callback_t write,mb_read_callback_t read);
int mbReadHoldingRegister(modbus_handle_t *phandle,const unsigned char slave,unsigned short addr,\
	unsigned short reg_num,unsigned long timeout,modbus_mode_t mode);
int mbReadInputRegister(modbus_handle_t *phandle,const unsigned char slave,unsigned short addr,
	unsigned short reg_num,unsigned long timeout,modbus_mode_t mode);
int mbWriteSingalRegister(modbus_handle_t *phandle,const unsigned char slave,unsigned short addr,
	unsigned short reg_value,unsigned long timeout,modbus_mode_t mode);
int mbWriteMultiRegister(modbus_handle_t *phandle,const unsigned char slave,unsigned short addr,
	unsigned short val_list[],unsigned short val_count, unsigned long timeout,modbus_mode_t mode);	
int mb_master_yield(modbus_handle_t *phandle);
//u16 environment_212_protocol_crc16_check(u8 *puchMsg, u16 usDataLen);

#endif /*__MODBUS_MASTER_H__*/
/* End of file****************************************************************/
