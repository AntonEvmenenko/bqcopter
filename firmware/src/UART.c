#include "UART.h"

#include "stm32f10x_rcc.h"

char _inputUARTBuffer[20];
unsigned _inputUARTBufferSize = 0;
int _gotSomething = 0;

int16_t _positionX = 0;
int16_t _positionY = 0;

void UART_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_Cmd(USART1, ENABLE);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_EnableIRQ(USART1_IRQn);
}

char hexStringToByte(char* buffer)
{
    char b1 = buffer[0];
    char b2 = buffer[1];
    return ((((b1 >= '0' && b1 <= '9') ? b1 - '0' : b1 - 'A' + 10) << 4) & 0xF0) |
            (((b2 >= '0' && b2 <= '9') ? b2 - '0' : b2 - 'A' + 10) & 0x0F);
}

int16_t hexStringToInt16(char* buffer)
{
    char b1 = hexStringToByte(buffer);
    char b2 = hexStringToByte(buffer + 2);
    return ((b1 << 8) & 0xFF00) | ((b2) & 0x00FF);
}

void UART_read(void)
{
    if (_gotSomething) {
        if (_inputUARTBufferSize == 1 && _inputUARTBuffer[0] == 'n') {
            _positionX = 0;
            _positionY = 0;
        } else if (_inputUARTBufferSize == 8) {
            int16_t x = hexStringToInt16(_inputUARTBuffer);
            int16_t y = hexStringToInt16(_inputUARTBuffer + 4);
            if (x >= -255 && x <= 255 && y >= -255 && y <= 255) {
                _positionX = x;
                _positionY = y;
            } else {
                _positionX = 0;
                _positionY = 0;
            }
        } else {
            _positionX = 0;
            _positionY = 0;
        }
        _gotSomething = 0;
        _inputUARTBufferSize = 0;
    }
}

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        char buffer = USART_ReceiveData(USART1);

        if (!_gotSomething) {
            if(!_inputUARTBufferSize && buffer == 'n') {
                _inputUARTBuffer[_inputUARTBufferSize++] = buffer;
                _gotSomething = 1;
            } else if (_inputUARTBufferSize != 8 && buffer == '\n') {
                _inputUARTBufferSize = 0;
            } else if ((buffer >= '0' && buffer <= '9') || (buffer >= 'A' && buffer <= 'F')) {
                _inputUARTBuffer[_inputUARTBufferSize++] = buffer;
                if (_inputUARTBufferSize == 8) {
                    _gotSomething = 1;
                }
            }
        }
    }
}
