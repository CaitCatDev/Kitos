#pragma once

#include <types.h>

typedef struct {
	void *prev;
	void *next;
} llist_t;


#define offsetof(type, member) \
	((uint64_t)&(((type*)0)->m))

#define container_of(ptr, type, member, memtype) ({ \
		const typeof(((type*)0)->member) *__mptr = (ptr); \
		(type *)((char *)__mptr - offsetof(type, member)); \
		})
