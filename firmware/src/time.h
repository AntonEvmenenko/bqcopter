#ifndef TIME_H
#define TIME_H

extern unsigned long ms_from_start;

void SysTick_init( void );
unsigned long get_ms_from_start( void );
unsigned long get_us_from_start( void );
void delay_ms( int delay );
void delay_us( int delay );

extern "C" {
    void SysTick_Handler( void );
}

#endif
