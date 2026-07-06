#include "usart.h"

ring_buffer_t rx_ring_buffer = {0}; /**< Received ring buffer **/
volatile uint8_t rx_rb_overflow_flag = 0U; /**< Received ring buffer overflow flag **/
volatile uint8_t command_received_flag = 0U;

/**
 * @brief Initialize USART.
 * @note Configure PA9(TX) as: high speed, no pull-up / no pull-down.
 * @note Configure PA10(RX) as: high speed, pull-up.
 * @note Enable RX and IDLE interrupts.
 * @note Set priority level as 6.
 */
void usart_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC->AHB1ENR;
    //Configure PA9 as: 7 alternate function, high speed, no pull-up / no pull-down
    //          PA10 as: 7 alternate function, high speed, pull-up
    GPIOA->MODER &= ~(GPIO_MODER_MODE9_Msk | GPIO_MODER_MODE10_Msk);
    GPIOA->MODER |= (GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1);
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT9_Msk | GPIO_OTYPER_OT10_Msk);
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED9_Msk | GPIO_OSPEEDR_OSPEED10_Msk);
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9_Msk | GPIO_PUPDR_PUPD10_Msk);
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD10_0;
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL9_Msk | GPIO_AFRH_AFSEL10_Msk);
    GPIOA->AFR[1] |= ((7U << GPIO_AFRH_AFSEL9_Pos) | (7U << GPIO_AFRH_AFSEL10_Pos));

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    (void)RCC->APB2ENR;

    USART1->BRR = BRR_9600_BAUD_100MHz_16OVR;
    USART1->CR1 = 0U;
    USART1->CR2 = 0U;
    USART1->CR3 = 0U;
    USART1->CR1 |= (USART_CR1_IDLEIE | USART_CR1_RXNEIE | USART_CR1_RE |
                    USART_CR1_TE | USART_CR1_UE);

    NVIC_ClearPendingIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 6U);
    NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void)
{
    //Receive
    if (USART1->SR & USART_SR_RXNE)
    {
        uint8_t byte = USART1->DR;
        if (ring_buffer_put(&rx_ring_buffer, byte) == RING_BUFFER_OVF)
            rx_rb_overflow_flag = 1U;
    }
    //Transmit
    if ((USART1->SR & USART_SR_TXE) &&
        (USART1->CR1 & USART_CR1_TXEIE))
    {
        uint8_t byte;
        if (ring_buffer_get(&tx_ring_buffer, &byte) == RING_BUFFER_EMPTY)
            USART1->CR1 &= ~USART_CR1_TXEIE;
        else
            USART1->DR = byte;
    }
    //IDLE
    if (USART1->SR & USART_SR_IDLE)
    {
        command_received_flag = 1U;
        USART1->SR;
        USART1->DR;
    }
}

/**
 * @brief Transmit string via USART1.
 * @param[in] str Pointer to the string.
 */
void usart1_transmit(char * str)
{
    while (*str)
    {
        if (ring_buffer_put(&tx_ring_buffer, (uint8_t) *str) == RING_BUFFER_OVF)
        {
            //SWO print
        }
        str++;
    }
    USART1->CR1 |= USART_CR1_TXEIE;
}

