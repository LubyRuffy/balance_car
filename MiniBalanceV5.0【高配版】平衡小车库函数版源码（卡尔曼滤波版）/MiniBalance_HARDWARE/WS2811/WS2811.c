#include "WS2811.h"

/**********************说明*****************************
调用LED_SPI_LowLevel_Init初始化之后,
只需在main()中调用LED_SPI_Update(unsigned long buffer[], uint32_t length),
第一个形参为颜色代码，3字节，分别是R,G,B
第二个是串联的灯数量，一般为1

如果多灯珠，需要用到void WS_SetAll(void)和Wsdat[]等等,这里不做介绍。
*******************************************************/

uint16_t PixelBuffer[328] = {0};
uint16_t PixelPointer = 0;

unsigned long WsDat[nWs]={0xFF0000,0xFF0000,0xFF0000,
                          0xFF0000,0xFF0000,0xFF0000,
													0xFF0000,0xFF0000,0xFF0000,
													0xFF0000,0xFF0000,0xFF0000};
													

void WS2811_Breath()
{
		const static u8 brightness = 255;
		static unsigned long breath_color=brightness<<16;
		uint8_t Red, Green, Blue;  // 三原色
		// 绿 红 蓝 三原色分解
	  Red   = breath_color>>16;
	  Green = breath_color>>8;
	  Blue  = breath_color;
	
		if(Blue==0x00)
		{
			if(Green<brightness)
			{
				Green++;
				Red--;
			}
		}
		if(Red==0x00)
		{
			if(Blue<brightness)
			{
				Blue++;
				Green--;
			}
		}
		if(Green==0x00)
		{
			if(Red<brightness)
			{
				Red++;
				Blue--;
			}
		}
		breath_color=(Red<<16)+(Green<<8)+Blue;
		
		LED_SPI_Update(&breath_color,1);
}


													
													
void LED_SPI_LowLevel_Init(void)
{
    uint16_t i = 0;

    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef   SPI_InitStructure;
    DMA_InitTypeDef   DMA_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);  
	
    DMA_DeInit(DMA2_Stream5);
	
		DMA_InitStructure.DMA_Channel=DMA_Channel_3;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (& (SPI1->DR));
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)PixelBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    //DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA_Mode_Normal;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
		
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
		
    DMA_Init(DMA2_Stream5, &DMA_InitStructure); /* DMA1 CH3 = MEM -> DR */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);



    SPI_I2S_DeInit(SPI1);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; /* 48MHz / 8 = 6MHz */
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

    SPI_Cmd(SPI1, ENABLE);

    for (i = 0; i < 328; i++)
    {
        PixelBuffer[i] = 0xAAAA;
    }

    PixelPointer = 0;

}


void WS_SetAll()
{
	unsigned char j;
	unsigned long temp;

		temp=WsDat[0];
		for(j=0;j<11;j++)
		{
			WsDat[j]=WsDat[j+1];
		}
		WsDat[11]=temp;

}


void LED_SPI_Update(unsigned long buffer[], uint32_t length)
{
    uint8_t i = 0;
    uint8_t m = 0;
    if(DMA_GetCurrDataCounter(DMA2_Stream5) == 0)
    {

        for (i = 0; i < length; i++)
        {
            LED_SPI_SendPixel(buffer[i]);
        }

        if(length < 12)
        {
            for(i = 12 - length; i < length; i++)
            {
                for(m = 0; m < 3; m++)
                {
                    LED_SPI_SendBits(0x00);
                }
            }
        }
				
				/* (20+1) * 2.5 = 51.5 ~ 52.5us */
        for (i = 0; i < 20; i++)   
        {
            LED_SPI_WriteByte(0x00);
        }

        PixelPointer = 0;

        DMA_Cmd(DMA2_Stream5, DISABLE);
        DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_TCIF5);
        DMA_SetCurrDataCounter(DMA2_Stream5, 24*length+20);
        DMA_Cmd(DMA2_Stream5, ENABLE);
    }
}


void LED_SPI_SendBits(uint8_t bits)
{
    int zero = 0x7000;  //111000000000000
    int one = 0xFF00;  //0x7f00和oxff00一样效果
    int i = 0x00;

    for (i = 0x80; i >= 0x01; i >>= 1)
    {
        LED_SPI_WriteByte((bits & i) ? one : zero);
    }
}

void LED_SPI_WriteByte(uint16_t Data)
{
    /* Wait until the transmit buffer is empty */
    /*
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
    {
    }
    */

    PixelBuffer[PixelPointer] = Data;
    PixelPointer++;

    /* Send the byte */
    /* SPI_I2S_SendData16(SPI1, Data); */
}

void LED_SPI_SendPixel(uint32_t color)
{
    /*
     r7,r6,r5,r4,r3,r2,r1,r0,g7,g6,g5,g4,g3,g2,g1,g0,b7,b6,b5,b4,b3,b2,b1,b0
     \_____________________________________________________________________/
                               |      _________________...
                               |     /   __________________...
                               |    /   /   ___________________...
                               |   /   /   /
                              RGB,RGB,RGB,RGB,...,STOP
    */

    /*
    	BUG Fix : Actual is GRB,datasheet is something wrong.
    */
	  uint8_t Red, Green, Blue;  // 三原色
		// 绿 红 蓝 三原色分解
	  Red   = color>>16;
	  Green = color>>8;
	  Blue  = color;
	
		LED_SPI_SendBits(Green);
		LED_SPI_SendBits(Red);
    LED_SPI_SendBits(Blue);
}
