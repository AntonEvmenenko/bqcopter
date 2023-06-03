#include "L3G4200D.h"

const uint8_t L3G4200D_Address = 0x69;
const uint8_t CTRL_REG1 = 0x20;
const uint8_t CTRL_REG2 = 0x21;
const uint8_t CTRL_REG3 = 0x22;
const uint8_t CTRL_REG4 = 0x23;
const uint8_t CTRL_REG5 = 0x24;

void setupL3G4200D(int scale)
{
	I2C_write( L3G4200D_Address, CTRL_REG1, 0xFF );
	I2C_write( L3G4200D_Address, CTRL_REG2, 0x00 );
	I2C_write( L3G4200D_Address, CTRL_REG3, 0x08 );

	if ( scale == 250 ) {
		I2C_write( L3G4200D_Address, CTRL_REG4, 0x00 );
	} else if ( scale == 500 ) {
		I2C_write( L3G4200D_Address, CTRL_REG4, 0x10 );
	} else {
		I2C_write(L3G4200D_Address, CTRL_REG4, 0x30);
	}

	I2C_write(L3G4200D_Address, CTRL_REG5, 0x00);
}

void getGyroValues(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t xMSB, xLSB, yMSB, yLSB, zMSB, zLSB;
	I2C_read(L3G4200D_Address, 0x29, &xMSB, 1);
	I2C_read(L3G4200D_Address, 0x28, &xLSB, 1);
	*x = ((((int16_t)xMSB) << 8) | (int16_t)xLSB);

	I2C_read(L3G4200D_Address, 0x2B, &yMSB, 1);
	I2C_read(L3G4200D_Address, 0x2A, &yLSB, 1);
	*y = (((int16_t)yMSB << 8) | (int16_t)yLSB);

	I2C_read(L3G4200D_Address, 0x2D, &zMSB, 1);
	I2C_read(L3G4200D_Address, 0x2C, &zLSB, 1);
	*z = (((int16_t)zMSB << 8) | (int16_t)zLSB);
}
