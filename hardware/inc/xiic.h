#ifndef __XIIC_H
#define __XIIC_H
#include "sys.h"


#define MAX30102_SCL_GPIO_PORT    	GPIOB		                /* MAX30102 SCL�˿� */
#define MAX30102_SCL_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* MAX30102 SCL�˿�ʱ�� */
#define MAX30102_SCL_GPIO_PIN		GPIO_Pin_10			        /* MAX30102 SCL�ܽ� */

#define MAX30102_SDA_GPIO_PORT    	GPIOB		                /* MAX30102 SDA�˿� */
#define MAX30102_SDA_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* MAX30102 SDA�˿�ʱ�� */
#define MAX30102_SDA_GPIO_PIN		GPIO_Pin_11			        /* MAX30102 SDA�ܽ� */

#define IIC_SCL                     PBout(10)                    //SCL
#define IIC_SDA                     PBout(11)                    //SDA	 
#define READ_SDA                    PBin(11)                     //����SDA 




void IIC_Init(void);                //��ʼ��IIC��IO��
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
void IIC_Read_One_Byte(u8 daddr,u8 addr,u8* data);

void IIC_WriteBytes(u8 WriteAddr,u8* data,u8 dataLength);
void IIC_ReadBytes(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength);
void SDA_IN(void);
void SDA_OUT(void);
#endif















