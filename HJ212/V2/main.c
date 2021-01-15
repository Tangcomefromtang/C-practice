#include <stdio.h>
#include <string.h>
#include "hj212.h"


E212_ARGS e212_args;
u8 pkt[] = "##0113QN=20210111180219000;ST=32;CN=3032;PW=123456;MN=20210107123456;Flag=5;CP=&&PolId=a34004,MB_BgnAddr=192,MB_Wrt=3&&0780";


int main()
{

    s8 re_mn = 0,re_pw = 0;
    s16 re_cn = 0;
    u16 crc=0,addre =0,value=0;

    strcpy(e212_args.device_code,"20210107123456");
    strcpy(e212_args.password,"123456");
    
    re_mn = hj212_check_mn(pkt,e212_args.device_code);
    re_pw = hj212_check_pw(pkt,e212_args.password);
    re_cn = hj212_ver_cn(pkt);ver
    crc  = hj212_crc16(&pkt[6],strlen(pkt)-10);
    hj212_pkt_getadress(pkt, &addre);
    hj212_pkt_getvalue(pkt, &value);

    printf("re_mn = %d\n", re_mn);
    printf("re_pw = %d\n", re_pw);
    printf("re_cn = %d\n", re_cn);
    printf("crc = 0x%x\n", crc);
    printf("address = %d\n", addre);
    printf("value = %d\n", value);

    return(0);
}
