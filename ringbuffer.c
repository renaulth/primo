#include "ringbuffer.h"

uint8_t buffer_add(volatile ringbuffer_t* buffer, uint8_t byte) {
    uint8_t next_index = (buffer->head + 1) % MAX_BUFFER_LENGTH;

    if (next_index == buffer->tail) {
        return BUFFER_FULL;
    }

    buffer->data[buffer->head] = byte;
    buffer->head = next_index;
    return BUFFER_OK;
}

uint8_t buffer_get(volatile ringbuffer_t* buffer, uint8_t* byte) {
    if (buffer->head == buffer->tail){
        return BUFFER_EMPTY;
    }

    *byte = buffer->data[buffer->tail];
    buffer->tail = (buffer->tail + 1) % MAX_BUFFER_LENGTH;
    return BUFFER_OK;
}

uint8_t buffer_peek(volatile ringbuffer_t* buffer, uint8_t* byte) {
    if (buffer->head == buffer->tail){
        return BUFFER_EMPTY;
    }

    *byte = buffer->data[buffer->tail];
    return BUFFER_OK;
}
