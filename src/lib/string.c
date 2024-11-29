#include <stdint.h>
#include <stdbool.h>

#include <types.h>

#include <lib/memory.h>
#include <lib/string.h>

int strcmp(const char *s1, const char *s2) {
	if(strlen(s1) != strlen(s2)) return -1;

	for(uint64_t i = 0; s1[i]; ++i) {
		if(s1[i] != s2[i]) return s1[i] - s2[i];
	}
	return 0;
}

bool str_contains(char ch, const char *reject) {
	for(uint64_t i = 0; reject[i]; ++i) {
		if(ch == reject[i]) return true;
	}

	return false;
}

unsigned long strlen(const char *str) {
	unsigned long i = 0;
	for(i = 0; str[i]; i++) {

	}

	return i;
}


static char *store;

char *strtok(char *str, const char *delim) {
	uint64_t i = 0;
	uint64_t j = 0;

	if(str == NULL) {
		str = store;
		store = NULL;
		if(str == NULL) return NULL;
	}

	uint64_t len = strlen(str);
	uint64_t dlen = strlen(delim);


	for(i = 0; i < len && str_contains(str[i], delim); ++i) {
	}
	
	if(i == len) {
		store = NULL;
		return NULL;
	}


	for(j = 0; j < len - i && !str_contains(str[i+j], delim); ++j);

	str[i+j] = '\0';

	if(j + i == len) {
		store = NULL;
	} else {
		store = &str[i+j+1];
	}

	return &str[i];
}
