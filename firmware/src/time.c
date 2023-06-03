#include "time.h"
#include "stm32f10x.h"

unsigned long ms_from_start = 0; // time from start in milliseconds

void SysTick_init( void )
{
    SysTick_Config( SystemCoreClock / 1000 ); // 1ms per interrupt

    // set systick interrupt priority
    //NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 ); // 4 bits for preemp priority 0 bit for sub priority
    NVIC_SetPriority( SysTick_IRQn, 0 ); // i want to make sure systick has highest priority amount all other interrupts

    ms_from_start = 0;
}

void SysTick_Handler( void ) {
    ++ms_from_start;
}

unsigned long get_ms_from_start( void ) {
    return ms_from_start;
}

unsigned long get_us_from_start( void ) {
    return ms_from_start * 1000 + 1000 - SysTick->VAL / 72;
}

void delay_ms( int delay ) {
    unsigned long now = get_ms_from_start( );
    while( get_ms_from_start( ) - now < delay );
}

void delay_us( int delay ) {
    unsigned long now = get_us_from_start( );
    while( get_us_from_start( ) - now < delay );
}
