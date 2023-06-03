#ifndef ADXL345_H
#define ADXL345_H

#include "I2C.h"

void setupADXL345(void);
void getAccelValues(int16_t *x, int16_t *y, int16_t *z);

#endif
