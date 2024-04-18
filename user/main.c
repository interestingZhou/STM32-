/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	main.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.0
	*
	*	说明： 		接入onenet，上传数据和命令控制
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "dht11.h"
#include "oled.h"
#include "Timer.h"
#include "HCSR04.h"
#include "PWM.h"
#include "LightSensor.h"

//C库
#include <string.h>

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"
//183.230.40.96
//mqtts.heclouds.com

void Display_Init(void);
void Hardware_Init(void);
void Refresh_Data(void);
u8 temp,humi;
u16 Distance_mm;
//extern len;

//uint16_t numlen(uint16_t num)//计算数字的长度
//{
//    uint16_t len = 1;        // 初始长度为1
//    for(; num > 9; ++len)    // 判断num是否大于9，否则长度+1
//        num /= 10;	         // 使用除法进行运算，直到num小于1
//    return len;              // 返回长度的值
//}



/*
************************************************************
*	函数名称：	main
*
*	函数功能：	
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
************************************************************
*/

int main(void)
{
	
	unsigned short timeCount = 0;	//发送间隔变量
	
	unsigned char *dataPtr = NULL;
	
	Hardware_Init();				//初始化外围硬件
	
	ESP8266_Init();					//初始化ESP8266


//	UsartPrintf(USART_DEBUG, "Connect MQTTs Server...\r\n");
	
	OLED_ShowString(0,0,"Connect MQTTs Server...",8);
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		DelayXms(500);
	
//	UsartPrintf(USART_DEBUG, "Connect MQTT Server Success\r\n");
	
	OLED_ShowString(0,4,"Connect MQTT Server Success",8);
	DelayXms(500);
	OLED_Clear();
	
	OLED_ShowString(0,0,"Device login...",16);
	

	while(OneNet_DevLink())			//接入OneNET
	{
		ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT");
		DelayXms(500);
	}
	
	OneNET_Subscribe();
	Display_Init();

	
	while(1)
	{		
		Refresh_Data();
		if (LightSensor_Get() == 1)		//如果当前光敏输出1
		{
			Led_Set(LED_ON);				//led开启
		}
		else							//否则
		{
			Led_Set(LED_OFF); //led关闭
		}
		if(++timeCount >= 100)									//发送间隔5s
		{
			DHT11_Read_Data(&temp,&humi);
			
			HCSR04_Read_Data(&Distance_mm);
			
			OneNet_SendData();									//发送数据
			
			timeCount = 0;
			ESP8266_Clear();
		}
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);

		DelayMs(10);
	
	}

}

/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置

	Delay_Init();									//systick初始化
	
	Usart1_Init(115200);							//串口1，打印信息用
	
	Usart2_Init(115200);							//串口2，驱动ESP8266用
	
//  Key_Init();
	
	Led_Init();									//LED初始化
	
	OLED_Init();			//初始化OLED  
	
	Timer_Init();
	
	HC_SR04_Init();
	
	PWM_Init();
	
	LightSensor_Init();

	
	while(DHT11_Init())
	{
//		UsartPrintf(USART_DEBUG, "DHT11 Error \r\n");
		OLED_ShowString(0,0,"DHT11 Error",16);

		DelayMs(1000);
	}	
	
//	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	OLED_ShowString(0,0," Hardware init OK",16);
	DelayMs(1000);
	
	OLED_Clear();
	
}

void Display_Init(void){
	
	  OLED_Clear();
	
		OLED_ShowCHinese(0,0,1);//温
		OLED_ShowCHinese(18,0,2);//度
		OLED_ShowCHinese(36,0,0);//：
		OLED_ShowCHinese(72,0,3);//℃

		
		OLED_ShowCHinese(0,2,4);//湿
		OLED_ShowCHinese(18,2,5);//度
		OLED_ShowCHinese(36,2,0);//：
		OLED_ShowString(72,2,"%",16);//%

		
		OLED_ShowCHinese(0,4,6);//台
		OLED_ShowCHinese(18,4,7);//灯
		OLED_ShowCHinese(36,4,0);//：
	  

		OLED_ShowCHinese(0,6,10);//距
		OLED_ShowCHinese(18,6,11);//离
    OLED_ShowCHinese(36,6,0);//：
//		OLED_ShowString(50,6,"22",16);
		OLED_ShowString(82,6,"mm",16);

}
void Refresh_Data(void)
{
	char buf[3];
	char dis[2];
	char dim[4];
	
	sprintf(buf, "%2d", temp);
	OLED_ShowString(50,0,buf,16);//温度值
	
	sprintf(buf, "%2d", humi);
	OLED_ShowString(50,2,buf,16);//湿度值
	
	if (Distance_mm<100){
    OLED_PartClear(6,8);
		OLED_ShowCHinese(0,6,10);//距
		OLED_ShowCHinese(18,6,11);//离
    OLED_ShowCHinese(36,6,0);//：
		OLED_ShowString(82,6,"mm",16);

		sprintf(dis, "%2d", Distance_mm);
	  OLED_ShowString(50,6,dis,16);
	}
	if (Distance_mm>=1000){
		OLED_PartClear(6,8);
		OLED_ShowCHinese(0,6,10);//距
		OLED_ShowCHinese(18,6,11);//离
    OLED_ShowCHinese(36,6,0);//：
		OLED_ShowString(82,6,"mm",16);

		sprintf(dim, "%2d", Distance_mm);
	  OLED_ShowString(50,6,dim,16);
	}
	else{
		sprintf(buf, "%2d", Distance_mm);
	  OLED_ShowString(50,6,buf,16);
	}
	
	if (yled_info.YLed_Status==1&&wled_info.WLed_Status==0)
		{
			OLED_PartClear(4,6);
			OLED_ShowCHinese(0,4,6);//台
	    OLED_ShowCHinese(18,4,7);//灯
		  OLED_ShowCHinese(36,4,0);//：
			OLED_ShowCHinese(50,4,16);//睡
			OLED_ShowCHinese(66,4,17);//眠
			OLED_ShowCHinese(82,4,14);//模
			OLED_ShowCHinese(98,4,15);//式
		}
	if (yled_info.YLed_Status==0&&wled_info.WLed_Status==1)
		{
			OLED_PartClear(4,6);
			OLED_ShowCHinese(0,4,6);//台
	    OLED_ShowCHinese(18,4,7);//灯
		  OLED_ShowCHinese(36,4,0);//：
			OLED_ShowCHinese(50,4,12);//正
			OLED_ShowCHinese(66,4,13);//常
			OLED_ShowCHinese(82,4,14);//模
			OLED_ShowCHinese(98,4,15);//式
		}
	if (yled_info.YLed_Status==1&&wled_info.WLed_Status==1)
		{
			OLED_PartClear(4,6);
			OLED_ShowCHinese(0,4,6);//台
	    OLED_ShowCHinese(18,4,7);//灯
		  OLED_ShowCHinese(36,4,0);//：
			OLED_ShowCHinese(50,4,18);//全
			OLED_ShowCHinese(66,4,8);//亮
		}
	if (yled_info.YLed_Status==0&&wled_info.WLed_Status==0)
		{
			OLED_PartClear(4,6);
			OLED_ShowCHinese(0,4,6);//台
		  OLED_ShowCHinese(36,4,0);//：
	    OLED_ShowCHinese(18,4,7);//灯
			OLED_ShowCHinese(50,4,18);//全
			OLED_ShowCHinese(66,4,9);//灭
		}
}