#include <stdint.h>
#include <lib/memory.h>

void *memset(void *dst, int val, uint64_t n) {
	uint8_t *dstu8 = dst;

	for(uint64_t i = 0; i < n; i++) {
		dstu8[i] = val;
	}

	return dst;
}

void *memcpy(void *dst, const void *src, uint64_t n) {
	uint8_t *u8dst = dst;
	const uint8_t *u8src = src;

	for(uint64_t i = 0; i < n; i++) {
		u8dst[i] = u8src[i];
	}

	return dst;
}

int memcmp(const void *m1, const void *m2, uint64_t size) {
	const uint8_t *m1u8 = m1;
	const uint8_t *m2u8 = m2;

	for(uint64_t i = 0; i < size; ++i) {
		if(m1u8[i] != m2u8[i]) {
			return m1u8 - m2u8;
		}
	}

	return 0;
}
