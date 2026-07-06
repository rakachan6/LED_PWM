#ifndef LED_PWM_RING_BUFFER_H
#define LED_PWM_RING_BUFFER_H

#define RING_BUFFER_SIZE 256
#include <stdint.h>

typedef enum
{
    RING_BUFFER_OK = 0U,
    RING_BUFFER_OVF,
    RING_BUFFER_EMPTY,
}ring_buffer_status_t;

typedef struct
{
    uint8_t data[RING_BUFFER_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
}ring_buffer_t;

ring_buffer_status_t ring_buffer_put(ring_buffer_t * rb, uint8_t data);
ring_buffer_status_t ring_buffer_get(ring_buffer_t * rb, uint8_t * data);

#endif //LED_PWM_RING_BUFFER_H
