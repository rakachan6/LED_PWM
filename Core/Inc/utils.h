#ifndef LED_PWM_UTILS_H
#define LED_PWM_UTILS_H

#include "stm32f4xx.h"
#include "usart.h"

#define HSE_ON_TIMEOUT      (20U)
#define PLL_ON_TIMEOUT      (5U)
#define PLL_SWITCH_TIMEOUT  (5U)

extern volatile uint8_t rx_rb_overflow_flag;
extern volatile uint8_t command_received_flag;

/**
 * @brief Returned values for clock configuration functions.
 */
typedef enum
{
    CLOCK_WAIT_PENDING = 0, /**< Wait state. Neither the timeout has expired nor the peripheral is enabled yet. */
    CLOCK_WAIT_OK,          /**< The peripheral is enabled. */
    CLOCK_WAIT_TIMEOUT,     /**< A timeout occurred. */
}wait_status_t;

void system_init(void);
void flags_check(void);
void mcu_reset(void);
#endif //LED_PWM_UTILS_H
