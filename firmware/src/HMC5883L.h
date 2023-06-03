#ifndef HMC5883L_H
#define HMC5883L_H

#include "I2C.h"

void setupHMC5883L( void );
void getRawCompassValues(int16_t *x, int16_t *y, int16_t *z);

#endif
