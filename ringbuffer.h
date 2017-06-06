#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>

#define MAX_BUFFER_LENGTH 16

enum BUFFER_STATUS {
    BUFFER_OK,
    BUFFER_EMPTY,
    BUFFER_FULL,
    UNDEFINED
};

typedef struct{
    uint8_t data[MAX_BUFFER_LENGTH];
    uint8_t head;
    uint8_t tail;
} ringbuffer_t;

uint8_t buffer_add(volatile ringbuffer_t* buffer, uint8_t byte);
uint8_t buffer_get(volatile ringbuffer_t* buffer, uint8_t* byte);
uint8_t buffer_peek(volatile ringbuffer_t* buffer, uint8_t* byte);

#endif
