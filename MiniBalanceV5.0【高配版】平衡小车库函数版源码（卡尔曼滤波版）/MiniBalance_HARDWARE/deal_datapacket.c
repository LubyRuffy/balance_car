#include "deal_datapacket.h"

u8 dataPID;												//保存接收到的数据包识别PID值

s32 err_cnt=0;
u8 errbufs[30][9];
u8 rxbuf[9];	
u8 rxcnt=0;
u8 RcvStatus=0;
#define RCVLENGTH 9

u8 Bottom_1=0;
u8 Bottom_2=0;
u8 Bottom_3=0;
u8 Bottom_4=0;

u8 Last_Bottom_1=0;
u8 Last_Bottom_2=0;
u8 Last_Bottom_3=0;
u8 Last_Bottom_4=0;

u8 senddata[5];

void UnpackData()
{
	
	Flag_sudu=1;//默认值
	
	blue =rxbuf[4];
	green=rxbuf[5];
	
	//油门死区
	if(rxbuf[4]>152)
		Flag_Qian=1,Flag_Hou=0;//,Flag_Left=0,Flag_Right=0;//////////////前
	else if(rxbuf[4]<102)
		Flag_Qian=0,Flag_Hou=1;//,Flag_Left=0,Flag_Right=0;//////////////后
	else 
		Flag_Qian=0,Flag_Hou=0;//,Flag_Left=0,Flag_Right=0;//////////////刹车
		
	//方向死区
	if(rxbuf[5]>152)
		Flag_Left=0,Flag_Right=1;  //左
	else if(rxbuf[5]<102)
		Flag_Left=1,Flag_Right=0;//右
	else 
		Flag_Left=0,Flag_Right=0;
 
	dataPID=rxbuf[7];
	
	if((rxbuf[2]&0x01)!=Last_Bottom_1)
		Bottom_1=1;
	else Bottom_1=0;
	
	if((rxbuf[2]&0x02)!=Last_Bottom_2)
		Bottom_2=1;
	else Bottom_2=0;
	
	if((rxbuf[2]&0x04)!=Last_Bottom_3)
		Bottom_3=1;
	else Bottom_4=0;
	
	if((rxbuf[2]&0x08))
		Bottom_4=1;
	else Bottom_4=0;
	
	Last_Bottom_1=rxbuf[2]&0x01;
	Last_Bottom_2=rxbuf[2]&0x02;
	Last_Bottom_3=rxbuf[2]&0x04;
	Last_Bottom_4=rxbuf[2]&0x08;
	
}

u8 CheckSum(u8* data,u8 length,u8 checkCode)
{
	u8 sum=0;
	while(length--)
		sum+=*(data++);
	return sum==checkCode;
}
void errcopy(u8* source,u8* dest,u8 length)
{
	while(length--)
			*(dest++)=*(source++);
}



void importData(u8 res)
{
	if(res==0xAA&&RcvStatus==0)
	{
		RcvStatus=1;
		rxbuf[0]=0xAA;
		rxcnt=1;
		return;
	}
	if(res==0xBB&&RcvStatus==1)
	{
		RcvStatus=2;
		rxcnt++;
		rxbuf[1]=res;
		return;
	}
	if(RcvStatus==2)
	{
		rxbuf[rxcnt++]=res;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
		if(rxcnt>=RCVLENGTH)
		{
			if(CheckSum(rxbuf,8,rxbuf[8]))
			{
				rxcnt=0;
				UnpackData();	
				RcvStatus=0;
				;//校验成功指示灯
			}
			else
			{
				RcvStatus=0;
				rxcnt=0;
				errcopy(rxbuf,errbufs[err_cnt%30],9);
				err_cnt++;
				;//校验不成功指示灯
			}
		}
	}
}


void  PackData()
{
	senddata[0]=0xAA;
	senddata[1]=*((u8*)&Voltage);
	senddata[2]=*(((u8*)&Voltage)+1);
	senddata[3]=*(((u8*)&Voltage)+2);
	senddata[4]=*(((u8*)&Voltage)+3);
	for(u8 i=0;i<5;i++)
		USART2_SendByByter(senddata[i]);
}

