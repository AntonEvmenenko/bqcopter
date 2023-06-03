#ifndef UART_H
#define UART_H

#include "stdint.h"

extern int16_t _positionX;
extern int16_t _positionY;

void UART_init(void);
void UART_read(void);

extern "C" {
    void USART1_IRQHandler(void);
}

#endif
