#include "ADXL345.h"

const uint8_t ADXL345_DEVICE = 0x53;
const uint8_t ADXL345_POWER_CTL = 0x2D;
const uint8_t ADXL345_DATAX0 = 0x32;

void setupADXL345(void)
{
	I2C_write(ADXL345_DEVICE, ADXL345_POWER_CTL, 0);
	I2C_write(ADXL345_DEVICE, ADXL345_POWER_CTL, 16);
	I2C_write(ADXL345_DEVICE, ADXL345_POWER_CTL, 8);
}

void getAccelValues(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t buffer[6];
	
	I2C_read(ADXL345_DEVICE, ADXL345_DATAX0, buffer, 6);
	
	*x = (((int16_t)buffer[1]) << 8) | (int16_t)buffer[0];   
	*y = (((int16_t)buffer[3]) << 8) | (int16_t)buffer[2];
	*z = (((int16_t)buffer[5]) << 8) | (int16_t)buffer[4];
}
