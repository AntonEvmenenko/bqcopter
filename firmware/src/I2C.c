#include "I2C.h"

void I2C_init(I2C_TypeDef* I2Cx, uint32_t I2C_ClockSpeed)
{
	GPIO_InitTypeDef gpio;
	I2C_InitTypeDef i2c;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);	
	
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;	
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio);

	i2c.I2C_ClockSpeed = I2C_ClockSpeed; 
	i2c.I2C_Mode = I2C_Mode_I2C;
	i2c.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c.I2C_OwnAddress1 = 0x42;
	i2c.I2C_Ack = I2C_Ack_Enable;
	i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	
	I2C_DeInit(I2C1);
	I2C_Cmd(I2Cx, ENABLE);
	I2C_Init(I2Cx, &i2c);
}

void I2C_write(uint8_t deviceAddress, uint8_t address, uint8_t val) 
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); 
	
 	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	
	I2C_Send7bitAddress(I2C1, deviceAddress << 1, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1, address);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_SendData(I2C1, val);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
}

void I2C_read(uint8_t deviceAddress, uint8_t address, uint8_t* buffer, uint8_t buffer_size)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
	I2C_Send7bitAddress(I2C1, deviceAddress << 1, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
		
	I2C_SendData(I2C1, address);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    
	I2C_Send7bitAddress(I2C1, deviceAddress << 1, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
		
	for (uint8_t i = 0; i < buffer_size; i++)
	{
		if (i == buffer_size - 1)
		{
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
		}
		else 
			I2C_AcknowledgeConfig(I2C1, ENABLE);
	
		while( !I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	
	  buffer[i] = I2C_ReceiveData(I2C1);
	}
}
