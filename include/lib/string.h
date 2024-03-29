#ifndef __LIB_STRING_H
#define __LIB_STRING_H
#include "stddef.h"
#include "stdint.h"
void memset(void* dst_, uint8_t value, uint32_t size);
void memcpy(void* dst_, const void* src_, uint32_t size);
int memcmp(const void* a_, const void* b_, uint32_t size);
char* strcpy(char* dst_, const char* src_);
char *strncpy( char *dest, const char *src, size_t count );
uint32_t strlen(const char* str);
int strcmp (const char *a, const char *b);
int strncmp(const char *lhs, const char *rhs, uint32_t count);
char* strchr(const char* string, const uint8_t ch);
char* strrchr(const char* string, const uint8_t ch);
char* strcat(char* dst_, const char* src_);
uint32_t strchrs(const char* filename, uint8_t ch);
#endif
