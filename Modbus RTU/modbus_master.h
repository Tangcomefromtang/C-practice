
typedef enum{
    //正常状态
    MB_NONE_ERROR = 0,
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
    //参数填写错误
	MB_ERROR_ARGS = -1,
    //响应超时
	MB_ERROR_TIMEOUT = -2,
    //返回数据为空
	MB_ERROR_NO_DATA = -3,
    //CRC校验错误
	MB_ERROR_CRC = -4,

}mb_err;

typedef enum{
	/*读离散量输入*/
	MB_FUNC_READ_DISCRETE_INPUTS = 1,
	/*读保持寄存器*/
	MB_FUNC_READ_HOLDING_REGISTER=  3,
	/*写单个保持寄存器*/
	MB_FUNC_WRITE_REGISTER    = 6,
	/*写多个寄存器*/
	MB_FUNC_WRITE_MULTIPLE_REGISTERS = 16,

}mb_fuc;

typedef struct{
    unsigned char address;
    unsigned char function;
    unsigned int init_addr;
    unsigned int reg_num;
    unsigned int crc
}mb_command;

