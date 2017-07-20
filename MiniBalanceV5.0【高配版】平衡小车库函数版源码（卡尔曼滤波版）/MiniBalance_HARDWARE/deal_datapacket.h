#ifndef _DEAL_DATAPACKET
#define _DEAL_DATAPACKET

#include "stm32f4xx.h"
#include "sys.h"	

extern u8 rxbuf[9];	
extern u8 dataPID;

extern u8 Bottom_1;
extern u8 Bottom_2;
extern u8 Bottom_3;
extern u8 Bottom_4;


void PackData(void);
void UnpackData(void);
void importData(u8 res);
u8 CheckSum(u8* data,u8 length,u8 checkCode);





#endif
