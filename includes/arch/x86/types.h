#pragma once

#include <stdint.h>

#define UINT32_FULL_MASK 0xffffffffU
#define UINT64_FULL_MASK 0xffffffffffffffffULL

#define NULL (void*)0

#define UINT32_TO_POINTER(u) (void*)((uintptr_t)u)
#define UINT64_TO_POINTER(u) (void*)((uintptr_t)u)


typedef uint64_t reg_t;
typedef uintptr_t phy_address;
typedef uintptr_t virt_address;
