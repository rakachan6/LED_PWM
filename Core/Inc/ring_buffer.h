#ifndef LED_PWM_RING_BUFFER_H
#define LED_PWM_RING_BUFFER_H

#define RING_BUFFER_SIZE 256
#include <stdint.h>

/**
 * @brief Returned values for ring buffer operations.
 */
typedef enum
{
    RING_BUFFER_OK = 0U,    /**< Function completed normally. **/
    RING_BUFFER_OVF,        /**< Ring buffer overflow. **/
    RING_BUFFER_EMPTY,      /**< Ring buffer is empty. **/
}ring_buffer_status_t;

/**
 * @brief Ring buffer type.
 */
typedef struct
{
    uint8_t data[RING_BUFFER_SIZE]; /**< Storage for data. **/
    volatile uint8_t head;          /**< Write index. **/
    volatile uint8_t tail;          /**< Read index. **/
}ring_buffer_t;

ring_buffer_status_t ring_buffer_put(ring_buffer_t * rb, uint8_t data);
ring_buffer_status_t ring_buffer_get(ring_buffer_t * rb, uint8_t * data);

#endif //LED_PWM_RING_BUFFER_H
