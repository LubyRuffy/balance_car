#ifndef _DEAL_DATAPACKET
#define _DEAL_DATAPACKET

#include "stm32f4xx.h"
#include "sys.h"	

extern u8 rxbuf[9];	
extern u8 dataPID;

void UnpackData(void);
void importData(u8 res);
u8 CheckSum(u8* data,u8 length,u8 checkCode);





#endif
