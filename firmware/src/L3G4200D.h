#ifndef L3G4200D_H
#define L3G4200D_H

#include "I2C.h"

void setupL3G4200D(int scale);
void getGyroValues(int16_t *x, int16_t *y, int16_t *z);

#endif
