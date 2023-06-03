#include "ESC.h"

#include "stm32f10x.h"

const int PWM_FREQUENCY = 400; // hz

const int _PWM_MIN_SIGNAL = 1000; // us
const int _PWM_MAX_SIGNAL = 1700; // us

volatile int _M1_POWER = _PWM_MIN_SIGNAL;
volatile int _M2_POWER = _PWM_MIN_SIGNAL;
volatile int _M3_POWER = _PWM_MIN_SIGNAL;
volatile int _M4_POWER = _PWM_MIN_SIGNAL;

void ESC_init( void ) 
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72;
	TIM_TimeBaseInitStructure.TIM_Period = 1000000 / PWM_FREQUENCY; // us
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);	

	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = 0; // us 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_EnableIRQ(TIM2_IRQn);
}


void TIM3_IRQHandler( void )
{
    TIM_ClearITPendingBit( TIM3, TIM_IT_Update );
    TIM3->CCR3 = _M1_POWER;
    TIM3->CCR4 = _M2_POWER;
}

void TIM2_IRQHandler( void )
{
    TIM_ClearITPendingBit( TIM2, TIM_IT_Update );
    TIM2->CCR3 = _M3_POWER;
    TIM2->CCR4 = _M4_POWER;
}

void turnMotorsOff(void)
{
    _M1_POWER = _PWM_MIN_SIGNAL;
    _M2_POWER = _PWM_MIN_SIGNAL;
    _M3_POWER = _PWM_MIN_SIGNAL;
    _M4_POWER = _PWM_MIN_SIGNAL;
}
