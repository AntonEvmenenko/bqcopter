/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
 
 /* spaniakos <spaniakos@gmail.com>
  Added __ARDUINO_X86__ support
*/

#ifndef __RF24_CONFIG_H__
#define __RF24_CONFIG_H__

  #include <stdint.h>
  #include <string.h>

  /*** USER DEFINES:  ***/  
  //#define FAILURE_HANDLING
  //#define SERIAL_DEBUG
  #define MINIMAL
  //#define SPI_UART  // Requires library from https://github.com/TMRh20/Sketches/tree/master/SPI_UART
  //#define SOFTSPI   // Requires library from https://github.com/greiman/DigitalIO
  #define STM32
  
  /**********************/
  #define rf24_max(a,b) (a>b?a:b)
  #define rf24_min(a,b) (a<b?a:b)
  
  // RF modules support 10 Mhz SPI bus speed
  const uint32_t RF24_SPI_SPEED = 10000000;  

  #define IF_SERIAL_DEBUG(x)
  #define _BV(x) (1<<(x))
  
  #define HIGH true
  #define LOW false
  
  #include "SPI.h"
  #define _SPI mySPI

  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
  #define pgm_read_word(x) (*(x))

  #include "time.h"
  #define delayMicroseconds(x) delay_us(x)
  #define delay(x) delay_ms(x)
  #define millis() get_ms_from_start()

  #define NULL 0

//  typedef uint16_t prog_uint16_t;
//  #define PSTR(x) (x)
//  #define printf_P printf
//  #define strlen_P strlen
  #define PROGMEM
//  #define pgm_read_word(p) (*(p))
//  #define PRIPSTR "%s"
//  typedef char const char;

#endif // __RF24_CONFIG_H__
