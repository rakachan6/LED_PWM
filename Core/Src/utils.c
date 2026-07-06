#include "utils.h"

#include "cli.h"


volatile uint32_t uptime_ms = 0U; /**< Value for count application uptime **/

/**
 * @brief Endless idle loop.
 * @warning This function never returns.
 */
static void fatal_error(void)
{
    while (1)
    {}
}

/**
 * @brief Configure SysTick to generate an interrupt every 1 millisecond.
 */
static void SysTick_config(void)
{
    SysTick->LOAD = (SystemCoreClock / 1000U) - 1U;
    SysTick->VAL  = 0U;
    SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk);
}

void SysTick_Handler(void)
{
    uptime_ms++;
}

/**
 * @brief Not-blocking wait for a peripheral flag to be set, with timeout.
 * @param[in] reg Pointer to the peripheral status/control register to poll.
 * @param[in] mask Bitwise mask within *reg to check
 * @param[in] timeout_ms Timeout in milliseconds.
 * @retval CLOCK_WAIT_OK - peripheral is enabled.
 *         CLOCK_WAIT_TIMEOUT - timeout is occurred.
 *         CLOCK_WAIT_PENDING - wait state.
 */
static wait_status_t wait_flag_not_blocking(
                                            const volatile uint32_t *reg,
                                            uint32_t mask,
                                            uint32_t timeout_ms)
{
    if (*reg & mask)
        return CLOCK_WAIT_OK;

    if (uptime_ms > timeout_ms)
        return CLOCK_WAIT_TIMEOUT;

    return CLOCK_WAIT_PENDING;
}

/**
 * @brief Configure core clock to maximal frequency (100 MHz).
 *
 * @note If necessary peripheral has not been enabled, will be called
 * fatal_error() function (Endless idle loop).
 * @note Configure Flash to 3 wait states latency and enable prefetch buffer.
 * @note In the end SystemCoreClock is updated by called SystemCoreClockUpdate().
 */
static void set_core_clock_to_100MHz(void)
{
    wait_status_t status;
    SystemCoreClockUpdate();
    SysTick_config();
    //Enable HSE
    RCC->CR |= RCC_CR_HSEON;
    uptime_ms = 0U;
    do
        status = wait_flag_not_blocking(&RCC->CR, RCC_CR_HSERDY, HSE_ON_TIMEOUT);
    while (status == CLOCK_WAIT_PENDING);
    if (status == CLOCK_WAIT_TIMEOUT)
    {
        //printf("HSE ENABLE ERROR\r\n");
        fatal_error();
    }
    //FLASH configuration
    FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;
    FLASH->ACR |= FLASH_ACR_LATENCY_3WS | FLASH_ACR_PRFTEN;
    //The multipliers and divisors configuration
    //AHB = 100MHz    APB1 = 50 MHz    APB2 = 100 MHz
    RCC->CFGR &= ~(RCC_CFGR_PPRE1_Msk | RCC_CFGR_PPRE2_Msk | RCC_CFGR_HPRE_Msk);
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_Msk | RCC_PLLCFGR_PLLN_Msk |
                      RCC_PLLCFGR_PLLP_Msk | RCC_PLLCFGR_PLLSRC_Msk);
    RCC->PLLCFGR |= (12U << RCC_PLLCFGR_PLLM_Pos |
                     96U << RCC_PLLCFGR_PLLN_Pos |
                     RCC_PLLCFGR_PLLSRC_HSE);
    //Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    uptime_ms = 0U;
    do
    {
        status = wait_flag_not_blocking(&RCC->CR, RCC_CR_PLLRDY, PLL_ON_TIMEOUT);
    }
    while (status == CLOCK_WAIT_PENDING);
    if (status == CLOCK_WAIT_TIMEOUT)
    {
        //printf("PLL ENABLE ERROR\r\n");
        fatal_error();
    }
    //PLL source switch to HSE
    RCC->CFGR &= ~RCC_CFGR_SW_Msk;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    uptime_ms = 0U;
    do
    {
        status = wait_flag_not_blocking(&RCC->CFGR, RCC_CFGR_SWS_PLL, PLL_SWITCH_TIMEOUT);
    }
    while (status == CLOCK_WAIT_PENDING);
    if (status == CLOCK_WAIT_TIMEOUT)
    {
        //printf("PLL ENABLE ERROR\r\n");
        fatal_error();
    }

    SystemCoreClockUpdate();
    SysTick_config();
}

/**
 * @brief Initialize and enable necessary peripheral.
 */
void system_init(void)
{
    set_core_clock_to_100MHz();
    usart_init();
}

void flags_check(void)
{
    if (rx_rb_overflow_flag)
    {
        rx_rb_overflow_flag = 0U;
        usart1_transmit("RX ring buffer overflow\r\n");
    }
    if (command_received_flag)
    {
        command_received_flag = 0U;
        cli_process();
    }
}

void mcu_reset(void)
{
    usart1_transmit("RESET\r\n");
    NVIC_SystemReset();
}