#pragma once

#include <stdint.h>

#define COM1 0x3f8

int init_serial(uint16_t port);
int is_transmit_empty(uint16_t port);
void write_serial(uint8_t a);
