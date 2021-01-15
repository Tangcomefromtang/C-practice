/*****************************************************************************
* File Name      : modbus_master.c
* Author         : Inhand Embedded Team
* Description    : 
* 					单线程Modbus Master
* 
* Copyright      : 
* Website        : www.inhand.com.cn
******************************************************************************
* modification history :
* zhaocj created on Mon Sep 03 2018 09:42:10 GMT+0800 (中国标准时间)
* zhaocj modified on Tue Sep 04 2018 16:16:47 GMT+0800 (中国标准时间)
******************************************************************************/
/* Includes -----------------------------------------------------------------*/
#include "modbus_master.h"
#include <string.h>
#include "time.h"
#include <stdlib.h>
#include "types.h"
#include "task_check.h"
#include "ipc.h"
#include "crc.h"
#include "syslog.h"

/* Private typedef ----------------------------------------------------------*/

/* Private define -----------------------------------------------------------*/
#define MB_PDU_SLAVE_OFFSET		0
#define REQ_PDU_FUNC_OFFSET		(MB_PDU_SLAVE_OFFSET+1)
#define REQ_PDU_DATA_OFFFSET	(REQ_PDU_FUNC_OFFSET+1)

#define RES_PDU_FUNC_OFFSET		(MB_PDU_SLAVE_OFFSET+1)
#define RES_PDU_DATA_OFFSET		(RES_PDU_FUNC_OFFSET+1)

#define MB_RESEND_INTERVAL		1000
/* Private variables --------------------------------------------------------*/
u32 modbus_mutex = IPC_INVALID_ID;
/* Extern variables ---------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/

u16 environment_212_protocol_crc16_check(u8 *puchMsg, u16 usDataLen)
{
	u16 i,j,crc_reg,check;
	crc_reg = 0xFFFF;
	for(i=0;i<usDataLen;i++)
	{
		crc_reg = (crc_reg>>8) ^ puchMsg[i];
		for(j=0;j<8;j++)
		{
		check = crc_reg & 0x0001;
		crc_reg >>= 1;
		if(check==0x0001)
		{
		crc_reg ^= 0xA001;
		}
		}
	}
	return crc_reg;
}

static int modbus_lock(void)
{
	if(IPC_INVALID_ID != modbus_mutex){
		if(sem_mutex_pend(modbus_mutex,IPC_PEND_BLOCK) == IPC_ERROR_OK){
			return TRUE;
		}
	}
	return FALSE;
}

static int modbus_unlock(void)
{
	sem_mutex_post(modbus_mutex);
	return TRUE;
}

static int send(modbus_handle_t *phandle)
{
	if(NULL == phandle || NULL == phandle->write || NULL == phandle->read){
		syslog(LOG_ERR,"handle is null!");
		return MB_ERROR_ARGS;
	}
	if(phandle->send_len == 0 || phandle->send_len > (sizeof(phandle->buffer))){
		syslog(LOG_ERR,"Invalid send len: %d",phandle->send_len);
		return MB_ERROR_ARGS;
	}
	phandle->write((const unsigned char*)phandle->buffer,phandle->send_len);
	return MB_ERROR_NONE;
}

static int parse(modbus_handle_t *phandle)
{
	// ASSERT(phandle);
	// ASSERT(phandle->read);
	u16 crc_rcv = 0, crc_calc = 0xFFFF;
	int rcv = 0;
	mb_msg_t *msg = &phandle->ack;
	if(phandle->receive_len >= sizeof(phandle->buffer)){
		phandle->receive_len = 0;
	}
	rcv = phandle->read(phandle->buffer+phandle->receive_len,sizeof(phandle->buffer)-phandle->receive_len);
	if(rcv > 0){
		phandle->receive_len += rcv;
	}
	else{
		msg->err_code = MB_ERROR_NO_DATA;
		goto exit;
	}
	if(phandle->receive_len < 4){
		syslog(LOG_WARNING,"receive incomplete data!");
		msg->err_code = MB_ERROR_NO_DATA;
		goto exit;
	}
	
	msg->func = phandle->buffer[RES_PDU_FUNC_OFFSET];
	/*receive error*/
	msg->err_code = MB_ERROR_NONE;
	
	if(msg->func&0x80){
		msg->func &= 0x7F;
		msg->err_code = (mb_error_t)phandle->buffer[RES_PDU_FUNC_OFFSET+1];
		switch(msg->err_code){
			case MB_ERROR_FUNC:
				syslog(LOG_ERR,"Invalid function code");
				break;
			case MB_ERROR_ADDR:
				syslog(LOG_ERR,"Invalid register addr");
				break;
			case MB_ERROR_VALUE:
				syslog(LOG_ERR,"Invalid data value");
				break;
			case MB_ERROR_DEV:
				syslog(LOG_ERR,"Device fault");
				break;
			case MB_ERROR_CONFIRM:
				syslog(LOG_ERR,"Wait a while...");
				return MB_ERROR_NONE;
			case MB_ERROR_BUSY:
				syslog(LOG_ERR,"Device busy");
				break;
			case MB_ERROR_PARITY:
				syslog(LOG_ERR,"Parity error");
				break;
			default:  
				syslog(LOG_ERR,"Unknown error");
				break;
		}
		phandle->receive_len = 0;
		phandle->state = MB_STATE_IDLE;
		return msg->err_code;
	}
	
	//MODBUS_DEBUG("rcv func: %d",msg.func);
	switch(msg->func)
	{
		/*读线圈: 未实现*/
		case MB_FUNC_READ_COILS:
			break;
		/*读离散量输入: 未实现*/
		case MB_FUNC_READ_DISCRETE_INPUTS:
			break;
		/*读保持寄存器*/
		case MB_FUNC_READ_HOLDING_REGISTER:
		/*读输入寄存器*/
		case MB_FUNC_READ_INPUT_REGISTER:
			msg->data_len = phandle->buffer[RES_PDU_FUNC_OFFSET+1];
			//MODBUS_DEBUG("Rcv %dB reg data",msg.data_len);
			if(msg->data_len >= sizeof(phandle->buffer)){
				syslog(LOG_WARNING,"Rcv invalid reg num: %d",msg->data_len);
				msg->data_len = sizeof(phandle->buffer);
			}
			if(msg->data_len > (phandle->receive_len-5)){
				msg->err_code = MB_ERROR_NO_DATA;
			}
			else{
				msg->data = &phandle->buffer[RES_PDU_FUNC_OFFSET+2];
				crc_rcv = *(u16*)&phandle->buffer[RES_PDU_FUNC_OFFSET+2+msg->data_len];
				crc_calc = crc16(phandle->buffer,msg->data_len+3);
			}
			break;
		/*写单个线圈: 未实现*/
		case MB_FUNC_WRITE_SINGLE_COIL:
			break;
		/*写多个线圈: 未实现*/
		case MB_FUNC_WRITE_MULTIPLE_COILS:
			break;
		/*写单个保持寄存器*/
		case MB_FUNC_WRITE_REGISTER:
			msg->data_len = 2;
			//MODBUS_DEBUG("Rcv %dB reg data",msg->data_len);
			if(msg->data_len >= sizeof(phandle->buffer)){
				syslog(LOG_ERR,"Rcv invalid reg num: %d",msg->data_len);
				msg->data_len = sizeof(phandle->buffer);
			}
			if(msg->data_len > (phandle->receive_len-5)){
				msg->err_code = MB_ERROR_NO_DATA;
			}
			else{
				msg->data = &phandle->buffer[RES_PDU_FUNC_OFFSET+3];
				crc_rcv = *(u16*)&phandle->buffer[RES_PDU_FUNC_OFFSET+3+msg->data_len];
				crc_calc = crc16(phandle->buffer,msg->data_len+4);
			}
			break;
		/*写多个寄存器*/
		case MB_FUNC_WRITE_MULTIPLE_REGISTERS:
			msg->data_len = 2;
			syslog(LOG_DEBUG,"Rcv %dB reg data",msg->data_len);
			if(msg->data_len >= sizeof(phandle->buffer)){
				syslog(LOG_WARNING,"Rcv invalid reg num: %d",msg->data_len);
				msg->data_len = sizeof(phandle->buffer);
			}
			if(msg->data_len > (phandle->receive_len-5)){
				msg->err_code = MB_ERROR_NO_DATA;
			}
			else{
				msg->data = &phandle->buffer[RES_PDU_FUNC_OFFSET+3];
				crc_rcv = *(u16*)&phandle->buffer[RES_PDU_FUNC_OFFSET+3+msg->data_len];
				crc_calc = crc16(phandle->buffer,msg->data_len+4);
			}
			break;
		default:
			syslog(LOG_WARNING,"Unsupported function code: %d",msg->func);
			msg->err_code = MB_ERROR_FUNC;
			break;
	}
	
	
	
	if(crc_rcv != crc_calc){
		syslog(LOG_WARNING,"Rcv error crc: %X:%X(rcv|calc)",crc_rcv,crc_calc);
		msg->err_code = MB_ERROR_CRC;
	}
	
exit:	
	if(MB_ERROR_NO_DATA == msg->err_code){
		if((uptimem() - phandle->timestamp) > phandle->timeout){
			//syslog("Modbus timeout!");
			msg->err_code = MB_ERROR_TIMEOUT;
			phandle->state = MB_STATE_IDLE;
			phandle->receive_len = 0;
		}
	}
	return msg->err_code;
}

static void wait_ack(modbus_handle_t *phandle)
{
	// ASSERT(phandle);
	// ASSERT(phandle->read);
	phandle->timestamp = uptimem();
	while(phandle->read(phandle->buffer,sizeof(phandle->buffer)) > 0){
		task_heartbeat();
		sleepm(3);
		if((uptimem() - phandle->timestamp) > 1000){
			syslog(LOG_WARNING,"Too many data!");
			break;
		}
	}
	
	int ret = send(phandle);
	if(MB_ERROR_NONE != ret){
		phandle->state = MB_STATE_IDLE;
		phandle->ack.err_code = MB_ERROR_ARGS;
		return;
	}
	phandle->state = MB_STATE_WAIT_ACK;
	phandle->receive_len = 0;
	phandle->ack.data = NULL;
	phandle->ack.data_len = 0;
	memset(phandle->buffer,0,sizeof(phandle->buffer));
	phandle->timestamp = uptimem();
	while(TRUE){
		task_heartbeat();
		sleepm(1);
		ret = parse(phandle);
		if(MB_ERROR_NO_DATA != ret){
			phandle->state = MB_STATE_IDLE;
			//phandle->receive_len = 0;
			break;
		}
	}
	phandle->state = MB_STATE_IDLE;
}

/**
 * @brief 初始化modbus
 * 
 * @param phandle 
 * @param write 
 * @param read 
 */
void modbus_master_init(modbus_handle_t *phandle,mb_write_callback_t write,mb_read_callback_t read)
{
	if(phandle){
		memset(&phandle->ack,0,sizeof(phandle->ack));
		memset(phandle->buffer,0,sizeof(phandle->buffer));
		phandle->read = read;
		phandle->write = write;
		phandle->receive_len = 0;
		phandle->send_len = 0;
		phandle->state = MB_STATE_IDLE;
		phandle->timeout = 0;
		phandle->timestamp = 0;
	}
	if(IPC_INVALID_ID == modbus_mutex){
		modbus_mutex = sem_mutex_create();
	}
}
/**
 * @brief 非阻塞模式调用此函数
 * 
 * @param phandle 
 * @return int 
 */
int mb_master_yield(modbus_handle_t *phandle)
{
	if(NULL == phandle || NULL == phandle->write || NULL == phandle->read){
		syslog(LOG_ERR,"handle is null!");
		return MB_ERROR_ARGS;
	}
	if(MB_STATE_WAIT_ACK != phandle->state){
		return phandle->state;
	}
	if(!modbus_lock()){
		return MB_ERROR_BUSY;
	}
	if(MB_ERROR_NO_DATA != parse(phandle)){
		phandle->receive_len = 0;
		phandle->state = MB_STATE_FINISH;
	}
	modbus_unlock();
	return phandle->ack.err_code;
}

/**
 * @brief 	读保持寄存器
 * 
 * @param phandle 
 * @param slave 
 * @param addr 
 * @param reg_num 
 * @param timeout 
 * @param mode 		阻塞/非阻塞
 * @return int 
 */
int mbReadHoldingRegister(modbus_handle_t *phandle,const unsigned char slave,unsigned short addr,
	unsigned short reg_num,unsigned long timeout,modbus_mode_t mode)
{
	//syslog(LOG_DEBUG,"printf read register");
	if(NULL == phandle || NULL == phandle->write || NULL == phandle->read){
		syslog(LOG_ERR,"handle is null!");
		return MB_ERROR_ARGS;
	}
#if 0
	if(MB_STATE_IDLE != phandle->state){
		MODBUS_ERR("Modbus busy!");
		return MB_ERROR_BUSY;
	}
#endif	
	if(!modbus_lock()){
		syslog(LOG_WARNING,"Get modbus mutex failed!");
		return MB_ERROR_BUSY;
	}
	phandle->buffer[0] = slave;
	phandle->buffer[1] = MB_FUNC_READ_HOLDING_REGISTER;
	phandle->buffer[2] = (addr>>8)&0xFF;
	phandle->buffer[3] = (addr)&0xFF;
	phandle->buffer[4] = (reg_num>>8)&0xFF;
	phandle->buffer[5] = reg_num&0xFF;
	phandle->send_len = 6;
	u16 crc = crc16((u8*)phandle->buffer,phandle->send_len);
	phandle->buffer[phandle->send_len++] = crc&0xFF;
	phandle->buffer[phandle->send_len++] = (crc>>8)&0xFF;
	phandle->timeout = timeout;
	phandle->timestamp = uptimem();
	phandle->state = MB_STATE_WAIT_ACK;
	phandle->ack.func = MB_FUNC_READ_HOLDING_REGISTER;
	if(MB_MODE_BLOCK == mode){
		/*阻塞模式*/
		wait_ack(phandle);
	}
	else{
		phandle->ack.err_code = (mb_error_t)send(phandle);
	}
	modbus_unlock();
	return phandle->ack.err_code;
}

/**
 * @brief 读输入寄存器
 * 
 * @param phandle 
 * @param slave 
 * @param addr 
 * @param reg_num 
 * @param timeout 
 * @param mode 		阻塞/非阻塞
 * @return int 
 */
int mbReadInputRegister(modbus_handle_t *phandle,const unsigned char slave,unsigned short addr,
	unsigned short reg_num,unsigned long timeout,modbus_mode_t mode)
{
	if(NULL == phandle || NULL == phandle->write || NULL == phandle->read){
		syslog(LOG_ERR,"handle is null!");
		return MB_ERROR_ARGS;
	}
#if 0
	if(MB_STATE_IDLE != phandle->state){
		MODBUS_ERR("Modbus busy!");
		return MB_ERROR_BUSY;
	}
#endif	
	if(!modbus_lock()){
		syslog(LOG_WARNING,"Get modbus mutex failed!");
		return MB_ERROR_BUSY;
	}
	phandle->buffer[0] = slave;
	phandle->buffer[1] = MB_FUNC_READ_INPUT_REGISTER;
	phandle->buffer[2] = (addr>>8)&0xFF;
	phandle->buffer[3] = (addr)&0xFF;
	phandle->buffer[4] = (reg_num>>8)&0xFF;
	phandle->buffer[5] = reg_num&0xFF;
	phandle->send_len = 6;
	u16 crc = crc16((u8*)phandle->buffer,phandle->send_len);
	phandle->buffer[phandle->send_len++] = crc&0xFF;
	phandle->buffer[phandle->send_len++] = (crc>>8)&0xFF;
	phandle->timeout = timeout;
	phandle->timestamp = uptimem();
	phandle->state = MB_STATE_WAIT_ACK;
	phandle->ack.func = MB_FUNC_READ_INPUT_REGISTER;
	if(MB_MODE_BLOCK == mode){
		/*阻塞模式*/
		wait_ack(phandle);
	}
	else{
		phandle->ack.err_code = (mb_error_t)send(phandle);
	}
	modbus_unlock();
	return phandle->ack.err_code;
}

/**
 * @brief 写单个寄存器
 * 
 * @param phandle 
 * @param slave 
 * @param addr 
 * @param reg_value 
 * @param timeout 
 * @param mode 		阻塞/非阻塞
 * @return int 
 */
int mbWriteSingalRegister(modbus_handle_t *phandle,const unsigned char slave,unsigned short addr,
	unsigned short reg_value,unsigned long timeout,modbus_mode_t mode)
{
	if(NULL == phandle || NULL == phandle->write || NULL == phandle->read){
		syslog(LOG_ERR,"handle is null!");
		return MB_ERROR_ARGS;
	}
#if 0
	if(MB_STATE_IDLE != phandle->state){
		MODBUS_ERR("Modbus busy!");
		return MB_ERROR_BUSY;
	}
#endif	
	if(!modbus_lock()){
		syslog(LOG_WARNING,"Get modbus mutex failed!");
		return MB_ERROR_BUSY;
	}
	phandle->buffer[0] = slave;
	phandle->buffer[1] = MB_FUNC_WRITE_REGISTER;
	phandle->buffer[2] = (addr>>8)&0xFF;
	phandle->buffer[3] = (addr)&0xFF;
	phandle->buffer[4] = (reg_value>>8)&0xFF;
	phandle->buffer[5] = reg_value&0xFF;
	phandle->send_len = 6;
	u16 crc = crc16((u8*)phandle->buffer,phandle->send_len);
	phandle->buffer[phandle->send_len++] = crc&0xFF;
	phandle->buffer[phandle->send_len++] = (crc>>8)&0xFF;
	phandle->timeout = timeout;
	phandle->timestamp = uptimem();
	phandle->state = MB_STATE_WAIT_ACK;
	phandle->ack.func = MB_FUNC_WRITE_REGISTER;
	if(MB_MODE_BLOCK == mode){
		/*阻塞模式*/
		wait_ack(phandle);
	}
	else{
		phandle->ack.err_code = (mb_error_t)send(phandle);
	}
	modbus_unlock();
	return phandle->ack.err_code;
}

/**
 * @brief 写多个寄存器
 * 
 * @param phandle 
 * @param slave 
 * @param addr 
 * @param val_list 
 * @param val_count 
 * @param timeout 
 * @param mode 		阻塞/非阻塞
 * @return int 
 */
int mbWriteMultiRegister(modbus_handle_t *phandle,const unsigned char slave,unsigned short addr,
	unsigned short val_list[],unsigned short val_count, unsigned long timeout,modbus_mode_t mode)
{
	if(NULL == phandle || NULL == phandle->write || NULL == phandle->read){
		syslog(LOG_ERR,"handle is null!");
		return MB_ERROR_ARGS;
	}

	if(!modbus_lock()){
		syslog(LOG_WARNING,"Get modbus mutex failed!");
		return MB_ERROR_BUSY;
	}
#if 0
	if(MB_STATE_IDLE != phandle->state){
		MODBUS_ERR("Modbus busy!");
		return MB_ERROR_BUSY;
	}
#endif	
	int i = 0;
	phandle->send_len = 0;
	phandle->buffer[phandle->send_len++] = slave;
	phandle->buffer[phandle->send_len++] = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
	phandle->buffer[phandle->send_len++] = (addr>>8)&0xFF;
	phandle->buffer[phandle->send_len++] = (addr)&0xFF;
	val_count = val_count > 123 ? 123 : val_count;
	phandle->buffer[phandle->send_len++] = (val_count>>8)&0xFF;
	phandle->buffer[phandle->send_len++] = val_count&0xFF;
	phandle->buffer[phandle->send_len++] = val_count*2;
	for(i = 0;i<val_count;i++){
		phandle->buffer[phandle->send_len++] = (val_list[i]>>8)&0xFF;
		phandle->buffer[phandle->send_len++] = val_list[i]&0xFF;
	}
	u16 crc = crc16((u8*)phandle->buffer,phandle->send_len);
	phandle->buffer[phandle->send_len++] = crc&0xFF;
	phandle->buffer[phandle->send_len++] = (crc>>8)&0xFF;
	phandle->timeout = timeout;
	phandle->timestamp = uptimem();
	phandle->state = MB_STATE_WAIT_ACK;
	phandle->ack.func = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
	if(MB_MODE_BLOCK == mode){
		/*阻塞模式*/
		wait_ack(phandle);
	}
	else{
		phandle->ack.err_code = (mb_error_t)send(phandle);
	}
	modbus_unlock();
	return phandle->ack.err_code;
}


/* End of file****************************************************************/
