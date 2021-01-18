#include <stdio.h>
#include <string.h>
#include "hj212.h"


E212_ARGS e212_args;
//u8 pkt[] = "##0113QN=20210111180219000;ST=32;CN=3032;PW=123456;MN=20210107123456;Flag=5;CP=&&PolId=a34004,MB_BgnAddr=192,MB_Wrt=3&&0780";
u8 pkt[] = "##0113QN=20210118201054000;ST=32;CN=3032;PW=123456;MN=20210107123456;Flag=5;CP=&&PolId=a34004,MB_BgnAddr=192,MB_Wrt=5&&0F00\r\n";



u8 *copy_str(u8 *ad_a, u8 *ad_b, u8 num)
{
    u8 i = 0;
    if (ad_b[0] == 0)
    {
        ad_a[0] = '0';
        return ad_a + 1;
    }

    for (i = 0; i < num; i++)
    {
        *(ad_a + i) = *(ad_b + i);
    }
    return (ad_a + i);
}

void crc2str(u16 crc, u8 *str)
{
    if ((crc / 4096) < 10)
        *str = (crc / 4096) + 0x30;
    else
        *str = (crc / 4096) + 0x37;
    if (((crc % 4096) / 256) < 10)
        *(str + 1) = ((crc % 4096) / 256) + 0x30;
    else
        *(str + 1) = ((crc % 4096) / 256) + 0x37;
    if (((crc % 256) / 16) < 10)
        *(str + 2) = ((crc % 256) / 16) + 0x30;
    else
        *(str + 2) = ((crc % 256) / 16) + 0x37;
    if ((crc % 16) < 10)
        *(str + 3) = (crc % 16) + 0x30;
    else
        *(str + 3) = (crc % 16) + 0x37;

}

void len2str(u16 len, u8 *str)
{
    str[0] = (len / 1000) + 0X30;
    str[1] = ((len % 1000) / 100) + 0x30;
    str[2] = ((len % 100) / 10) + 0x30;
    str[3] = (len % 10) + 0x30; //数据段长度ascii表示
}

void hj212_creat_QnRtn_pkt(u8 *pkt, E212_ARGS* args,char qn)
{
    u8 *p = NULL;
    //struct tm tmv;
    u16 crc_tem=0,u16_temp=0;
    u8 str_tem[5];

    pkt[0] = '#';
    pkt[1] = '#';
    //数据段长度后面补加先占个位
    p = copy_str(&pkt[2], "0000", 4);
    p = copy_str(&pkt[6], "QN=", 3);
    p = copy_str(&pkt[9], "20210107123456", 14);
    p = copy_str(p, ";", 1);
    p = copy_str(p, "ST=91;", 6);
    p = copy_str(p, "CN=9011;", 8);
    p = copy_str(p, "PW=", 3);
    p = copy_str(p, args->password, strlen((const char *)e212_args.password));
    p = copy_str(p, ";", 1);
    p = copy_str(p, "MN=", 3);
    p = copy_str(p, args->device_code, strlen((const char *)e212_args.device_code));
    p = copy_str(p, ";", 1);
    p = copy_str(p, "Flag=4;", 7);
    p = copy_str(p, "CP=&&", 5);

    p = copy_str(p, "QnRtn=", 6);
    *p = qn;
    p++;
    p = copy_str(p, "&&", 2);

    u16_temp = strlen((const char *)pkt) - 6;
    len2str(u16_temp, str_tem);
    copy_str(&pkt[2], str_tem, 4);

    crc_tem = hj212_crc16(&pkt[6],strlen((const char *)pkt) - 6);
    crc2str(crc_tem, str_tem);
    p = copy_str(p, str_tem, 4);
    p = copy_str(p, "\r\n", 2);

}

int main()
{

    s8 re_mn = 0, re_pw = 0;
    s16 re_cn = 0;
    u16 crc = 0, addre = 0, value = 0;
    u8 re_pkt[100];

    strcpy(e212_args.device_code, "20210107123456");
    strcpy(e212_args.password, "123456");

    //re_mn = hj212_check_mn(pkt, e212_args.device_code);
    //re_pw = hj212_check_pw(pkt, e212_args.password);
    //re_cn = hj212_ver_cn(pkt);
    crc  = hj212_crc16(&pkt[6], strlen(pkt) - 12);
    hj212_pkt_getadress(pkt, &addre);
    hj212_pkt_getvalue(pkt, &value);

    //hj212_creat_QnRtn_pkt(re_pkt, &e212_args,hj212_ver_QnRtn(pkt,&e212_args));

    //printf("re_pkt = %s\n", re_pkt);

    printf("QnRtn = %c\n",hj212_check_crc(pkt, crc));
    //printf("crc = %x\n", crc);
   // printf("re_mn = %d\n", re_mn);
   // printf("re_pw = %d\n", re_pw);
   // printf("re_cn = %d\n", re_cn);

    printf("address = %d\n", addre);
    printf("value = %d\n", value);

    return (0);
}
