#ifndef _WS2811_H
#define	_WS2811_H

#include "stm32f4xx.h"

#define nWs 12		// 有多少颗WS2811级联（个人认为也可以是闪烁）
extern unsigned long WsDat[];

void WS2811_Breath(void);//渐变呼吸

void WS_SetAll(void);

void LED_SPI_LowLevel_Init(void);
void LED_SPI_WriteByte(uint16_t Data);
void LED_SPI_SendBits(uint8_t bits);
void LED_SPI_SendPixel(uint32_t color);
void LED_SPI_Update(unsigned long buffer[], uint32_t length);
#endif /* __LED_H */
