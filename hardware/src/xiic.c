#include "xiic.h"
#include "delay.h"

//初始化IIC
void IIC_Init(void)
{

    
    GPIO_InitTypeDef  GPIO_InitStructure;

    /*配置OLED SCLK*/
    RCC_APB2PeriphClockCmd(MAX30102_SCL_GPIO_CLK, ENABLE);	 //使能OLED_SCLK_GPIO_CLK端口时钟
    GPIO_InitStructure.GPIO_Pin = MAX30102_SCL_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
    GPIO_Init(MAX30102_SCL_GPIO_PORT, &GPIO_InitStructure);	  //初始化GPIOD3,6
    GPIO_SetBits(MAX30102_SCL_GPIO_PORT,MAX30102_SCL_GPIO_PIN);
    
    /*配置OLED SDIN*/
    RCC_APB2PeriphClockCmd(MAX30102_SDA_GPIO_CLK, ENABLE);	 //使能OLED_SCLK_GPIO_CLK端口时钟
    GPIO_InitStructure.GPIO_Pin = MAX30102_SDA_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
    GPIO_Init(MAX30102_SDA_GPIO_PORT, &GPIO_InitStructure);	  //初始化GPIOD3,6
    GPIO_SetBits(MAX30102_SDA_GPIO_PORT,MAX30102_SDA_GPIO_PIN);


}

void SDA_IN(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(MAX30102_SDA_GPIO_CLK, ENABLE);	 //使能OLED_SCLK_GPIO_CLK端口时钟
    GPIO_InitStructure.GPIO_Pin = MAX30102_SDA_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //  这里配置为上拉和下拉都可以
    GPIO_Init(MAX30102_SDA_GPIO_PORT, &GPIO_InitStructure);	  //初始化GPIOD3,6
}

void SDA_OUT(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(MAX30102_SDA_GPIO_CLK, ENABLE);	 //使能OLED_SCLK_GPIO_CLK端口时钟
    GPIO_InitStructure.GPIO_Pin = MAX30102_SDA_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
    GPIO_Init(MAX30102_SDA_GPIO_PORT, &GPIO_InitStructure);	  //初始化GPIOD3,6
    GPIO_SetBits(MAX30102_SDA_GPIO_PORT,MAX30102_SDA_GPIO_PIN);
}
//产生IIC起始信号
void IIC_Start(void)
{
    SDA_OUT();     //sda线输出
    IIC_SDA=1;
    IIC_SCL=1;
    DelayUs(4);
    IIC_SDA=0;//START:when CLK is high,DATA change form high to low
    DelayUs(4);
    IIC_SCL=0;//钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void IIC_Stop(void)
{
    SDA_OUT();//sda线输出
    IIC_SCL=0;
    IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    DelayUs(4);
    IIC_SCL=1;
    IIC_SDA=1;//发送I2C总线结束信号
    DelayUs(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    SDA_IN();      //SDA设置为输入
    IIC_SDA=1;
    DelayUs(1);
    IIC_SCL=1;
    DelayUs(1);
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL=0;//时钟输出0
    return 0;
}
//产生ACK应答
void IIC_Ack(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=0;
    DelayUs(2);
    IIC_SCL=1;
    DelayUs(2);
    IIC_SCL=0;
}
//不产生ACK应答
void IIC_NAck(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=1;
    DelayUs(2);
    IIC_SCL=1;
    DelayUs(2);
    IIC_SCL=0;
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0; t<8; t++)
    {
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1;
        DelayUs(2);   //对TEA5767这三个延时都是必须的
        IIC_SCL=1;
        DelayUs(2);
        IIC_SCL=0;
        DelayUs(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA设置为输入
    for(i=0; i<8; i++ )
    {
        IIC_SCL=0;
        DelayUs(2);
        IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;
        DelayUs(1);
    }
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK
    return receive;
}


void IIC_WriteBytes(u8 WriteAddr,u8* data,u8 dataLength)
{
    u8 i;
    IIC_Start();

    IIC_Send_Byte(WriteAddr);	    //发送写命令
    IIC_Wait_Ack();

    for(i=0; i<dataLength; i++)
    {
        IIC_Send_Byte(data[i]);
        IIC_Wait_Ack();
    }
    IIC_Stop();//产生一个停止条件
    DelayMs(10);
}

void IIC_ReadBytes(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength)
{
    u8 i;
    IIC_Start();

    IIC_Send_Byte(deviceAddr);	    //发送写命令
    IIC_Wait_Ack();
    IIC_Send_Byte(writeAddr);
    IIC_Wait_Ack();
    IIC_Send_Byte(deviceAddr|0X01);//进入接收模式
    IIC_Wait_Ack();

    for(i=0; i<dataLength-1; i++)
    {
        data[i] = IIC_Read_Byte(1);
    }
    data[dataLength-1] = IIC_Read_Byte(0);
    IIC_Stop();//产生一个停止条件
    DelayMs(10);
}

void IIC_Read_One_Byte(u8 daddr,u8 addr,u8* data)
{
    IIC_Start();

    IIC_Send_Byte(daddr);	   //发送写命令
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);//发送地址
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(daddr|0X01);//进入接收模式
    IIC_Wait_Ack();
    *data = IIC_Read_Byte(0);
    IIC_Stop();//产生一个停止条件
}

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data)
{
    IIC_Start();

    IIC_Send_Byte(daddr);	    //发送写命令
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);//发送地址
    IIC_Wait_Ack();
    IIC_Send_Byte(data);     //发送字节
    IIC_Wait_Ack();
    IIC_Stop();//产生一个停止条件
    DelayMs(10);
}



























