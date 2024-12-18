#pragma once


#include <stdint.h>

uint64_t readmsr(uint64_t msr);
uint64_t writemsr(uint64_t msr, uint64_t val);
