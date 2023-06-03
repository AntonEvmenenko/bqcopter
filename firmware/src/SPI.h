#ifndef SPI_H
#define SPI_H

#include "stm32f10x_spi.h"

class SPI {
    public:
        SPI();

        void begin();
        void transfer(uint8_t* dataout, uint8_t* datain, uint8_t len);
        uint8_t transfer(uint8_t dataout);
};

extern SPI mySPI;

#endif
