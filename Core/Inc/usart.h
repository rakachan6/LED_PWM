#ifndef LED_PWM_USART_H
#define LED_PWM_USART_H

#include "stm32f4xx.h"
#include "ring_buffer.h"

#define BRR_9600_BAUD_100MHz_16OVR (10417U)

extern ring_buffer_t tx_ring_buffer;

void usart_init(void);
void usart1_transmit(char * str);

#endif //LED_PWM_USART_H
