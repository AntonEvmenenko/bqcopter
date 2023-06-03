#include "HMC5883L.h"

const uint8_t HMC5883L_address = 0x1E;
const uint8_t MODE_REGISTER = 0x02;
const uint8_t DATA_REGISTER_BEGIN = 0x03;


void setupHMC5883L( void )
{
    // setting measurement mode to continous
    I2C_write( HMC5883L_address, MODE_REGISTER, 0x00 );
}

void getRawCompassValues( int16_t *x, int16_t *y, int16_t *z )
{
    uint8_t buffer[ 6 ];

    I2C_read( HMC5883L_address, DATA_REGISTER_BEGIN, buffer, 6 );

    *x = ( ( ( int16_t )buffer[ 0 ]) << 8) | ( int16_t )buffer[ 1 ];   
    *y = ( ( ( int16_t )buffer[ 2 ]) << 8) | ( int16_t )buffer[ 3 ];
    *z = ( ( ( int16_t )buffer[ 4 ]) << 8) | ( int16_t )buffer[ 5 ];
}
