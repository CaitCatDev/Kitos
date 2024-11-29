#pragma once

#include <types.h>
#include <stdint.h>

void *memset(void *dst, int val, uint64_t n);
void *memcpy(void *dst, const void *src, uint64_t n);
int memcmp(const void *m1, const void *m2, uint64_t size);
