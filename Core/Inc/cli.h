#ifndef LED_PWM_CLI_H
#define LED_PWM_CLI_H

#include "usart.h"
#include "string.h"
#include "utils.h"

#define CLI_COMMAND_SIZE (64U)

extern volatile uint32_t uptime_ms;
extern ring_buffer_t rx_ring_buffer;

void cli_process(void);

#endif //LED_PWM_CLI_H
