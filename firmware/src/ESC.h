#ifndef ESC_H
#define ESC_H

extern const int _PWM_MIN_SIGNAL;
extern const int _PWM_MAX_SIGNAL;

extern volatile int _M1_POWER;
extern volatile int _M2_POWER;
extern volatile int _M3_POWER;
extern volatile int _M4_POWER;

void ESC_init( void );

extern "C" {
    void TIM3_IRQHandler( void );
}

extern "C" {
    void TIM2_IRQHandler( void );
}

void turnMotorsOff(void);

#endif
