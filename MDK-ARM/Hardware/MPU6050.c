#include "main.h"
#include "MyI2C.h"
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS		0xD0

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(Data);
	MyI2C_ReceiveAck();
	MyI2C_Stop();
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();
	
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);
	MyI2C_ReceiveAck();
	Data = MyI2C_ReceiveByte();
	MyI2C_SendAck(1);
	MyI2C_Stop();
	
	return Data;
}

void MPU6050_ReadRegs(uint8_t RegAddress,uint8_t *Array,uint8_t Length)
{
	uint8_t i;
	
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveAck();
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveAck();
	
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);
	MyI2C_ReceiveAck();
	
	for(i=0;i<Length;i++)//连续读取寄存器
	{
		Array[i] = MyI2C_ReceiveByte();
		if(i<Length-1)
		{
			MyI2C_SendAck(0);//连续读取字节的时候不需要发送应答
		}
		else
		{
			MyI2C_SendAck(1);
		}
	}

	MyI2C_Stop();
	
}

void MPU6050_Init(void)
{
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x07);
	MPU6050_WriteReg(MPU6050_CONFIG, 0x00);
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t Array[14];
	MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H,Array,14);
	
	*AccX = (Array[0] << 8) | Array[1];
	*AccY = (Array[2] << 8) | Array[3];
	*AccZ = (Array[4] << 8) | Array[5];
	
	*GyroX = (Array[8] << 8) | Array[9];
	*GyroY = (Array[10] << 8) | Array[11];
	*GyroZ = (Array[12] << 8) | Array[13];
}
