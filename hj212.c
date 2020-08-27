#include <stdio.h>
#include <string.h>


typedef unsigned int u16;
typedef unsigned char u8;

/**
 * @Brief : 将两个u8数据合并成1个u16数据
 * @param  data             
 * @param  type ：0和2代表高字节在前，1和3代表低字节在前            
 * @return u16 
 */
u16 u8tou16(const u8 *data, u8 type)
{
    u16 u16_data = 0;
   
    if (type == 0)//U16 HL
    {
        u16_data = (*data << 8) | *(data + 1);
    }
    else if (type == 1)//U16 LH
    {
        u16_data = (*(data + 1) << 8) | *data;
    }
    else if (type == 2)//S16 HL
    {
        u16_data = (*data << 8) | *(data + 1);
    }
    else if (type == 3)//S16 LH
    {
        u16_data = (*(data + 1) << 8) | *data;
    }

    return u16_data;
}

/**
 * @Brief : 将16进制整数转化为字符串
 * @param  dat              
 * @param  str              
 * @param  type：0代表正数，1代表负数          
 */
void u16tostr(u16 dat,u8* str,u8 type)
{
    u8 temp[8];
    u8 i=0,j=0;

    while(dat)
    {
        temp[i]=dat%10+0x30;
        i++;
        dat/=10;
    }
    j=i;

    if( 1 == type)
    {
        temp[j] = '-';
        j = j + 1;
    }

    for (i = 0; i < j; i++)
    {
        str[i] = temp[j - i - 1];
    }

    if(!i) {str[i]='0';}
    else   {str[i]=0;}

}

/**
 * @Brief : 将16进制整数转化为字符串
 * @param  data  16进制数           
 * @param  type             
 * @param  str              
 */
void handle_datatype(u16 data, u8 type, u8 *str)
{
    if (type <= 1) //u16
    {
        u16tostr(data, str,0);
    }
    else if (type <= 3) //s16
    {
        if(data & 0x8000)//<0
        {
            data = ~data;
            data = data + 1;
            data = data << 16;
            data = data >> 16;
            u16tostr(data, str,1);
        }
        else//>0
        {
            u16tostr(data, str,0);
        }

    }

}

void handle_accracy(u8 type,u8* str)
{
    u8 len=0;
    len = strlen(str);
    //printf("len1 = %d\n",len);

    if(type==0 || str[0]=='0')
    {
        return ;
    }
    if('-' == str[0])//负数
    {
        if(1 == type && len < 3)
        {
            str[len] = str[len-1];
            str[len-1] = '0';
            str[3] = 0;
        }
        else if(2 == type && len < 4)
        {
            for(int i=0;i<4-len;i++)//1.先填充0
            {
                str[3-i] = '0';
            }
            for(int i=0;i<len-1;i++)//2.向后移动
            {
                str[3-i] = str[len-1-i];
                str[len-1-i] = '0';
            }
            str[4] = 0;

        }
        else if(3 == type && len < 5)
        {
            for(int i=0;i<5-len;i++)//1.先填充0
            {
                str[4-i] = '0';
            }
            for(int i=0;i<len-1;i++)//2.向后移动
            {
                str[4-i] = str[len-1-i];
                str[len-1-i] = '0';
            }
            str[5] = 0;

        }
        else if(4 == type && len < 6)
        {
            for(int i=0;i<6-len;i++)//1.先填充0
            {
                str[5-i] = '0';
            }
            for(int i=0;i<len-1;i++)//2.向后移动
            {
                str[5-i] = str[len-1-i];
                str[len-1-i] = '0';
            }
            str[6] = 0;

        }

    }
    else
    {
        if(1 == type && len < 2)
        {
            str[len] = str[len-1];
            str[len-1] = '0';
            str[2] = 0;
        }
        else if(2 == type && len < 3)
        {
            for(int i=0;i<3-len;i++)//1.先填充0
            {
                str[2-i] = '0';
            }
            for(int i=0;i<len;i++)//2.向后移动
            {
                str[2-i] = str[len-1-i];
                str[len-1-i] = '0';
            }
            str[3] = 0;

        }
        else if(3 == type && len < 4)
        {
            for(int i=0;i<4-len;i++)//1.先填充0
            {
                str[3-i] = '0';
            }
            for(int i=0;i<len;i++)//2.向后移动
            {
                str[3-i] = str[len-1-i];
                str[len-1-i] = '0';
            }
            str[4] = 0;

        }
        else if(4 == type && len < 5)
        {
            for(int i=0;i<5-len;i++)//1.先填充0
            {
                str[4-i] = '0';
            }
            for(int i=0;i<len;i++)//2.向后移动
            {
                str[4-i] = str[len-1-i];
                str[len-1-i] = '0';
            }
            str[5] = 0;

        }
    }

    //printf("str = %s\n",str);

    len = strlen(str);
    //printf("len2 = %d\n",len);

    if(type == 1)//0.1
    {
        str[len] = str[len - 1];
        str[len - 1] = '.';
    }
    else if(type == 2)//0.01
    {
        str[len] = str[len - 1];
        str[len - 1] = str[len - 2];
        str[len - 2] = '.';
    }
    else if(type == 3)//0.001
    {
        for(int i=0;i<3;i++)
            str[len- i] = str[len - i -1];
        str[len - 3] = '.';
    }
    else if(type == 4)//0.0001
    {
        for(int i=0;i<4;i++)
            str[len- i] = str[len - i -1];
        str[len - 4] = '.';
    }
    str[len+1] = 0 ;

}

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


int main() {
    u8 data[10] = {0x00, 0x01};
    u8 sensor[8];
    u16 hand;
    u8 crc[]="QN=20200823170034002;ST=31;CN=2011;PW=123457;MN=123456789012345678901234;Flag=4;CP=&&DataTime=20200823170034;A001-Rtd=-40.0&&";
    printf("data = %x\n", u8tou16(data, 0));

    handle_datatype(u8tou16(data, 0), 0, sensor);
    printf("data = %s\n", sensor);

    handle_accracy(1,sensor);
    printf("data = %s\n", sensor);

//    printf("-12=%x\n",-12);
//    hand = environment_212_protocol_crc16_check(crc, 125);
//    printf("crc = %x\n", hand);



    return 0;
}