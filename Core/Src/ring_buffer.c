#include "ring_buffer.h"

/**
 * @brief Put byte to ring buffer.
 * @param[in, out] rb Ring buffer.
 * @param[in] data Byte.
 * @retval RING_BUFFER_OVF - Ring buffer overflow.
 *         RING_BUFFER_OK - The byte was stored successfully.
 */
ring_buffer_status_t ring_buffer_put(ring_buffer_t* rb, const uint8_t data)
{
    uint8_t new_head = rb->head + 1U;
    if(new_head == rb->tail)
        return RING_BUFFER_OVF;
    rb->data[rb->head] = data;
    rb->head = new_head;
    return RING_BUFFER_OK;
}

/**
 * @brief Get the oldest byte from ring buffer.
 * @param[in, out] rb Ring buffer.
 * @param[out] data Pointer to the variable that will receive the byte.
 * @retval RING_BUFFER_EMPTY - Ring buffer is empty.
 *         RING_BUFFER_OK - The byte was retrieved successfully.
 */
ring_buffer_status_t ring_buffer_get(ring_buffer_t* rb, uint8_t* data)
{
    uint8_t new_tail = rb->tail + 1U;
    if (new_tail == rb->head)
        return RING_BUFFER_EMPTY;
    *data = rb->data[rb->tail];
    rb->tail = new_tail;
    return RING_BUFFER_OK;
}
