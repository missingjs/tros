#ifndef TROS_STDIO_H
#define TROS_STDIO_H
#include "stdint.h"
typedef char* va_list;
uint32_t printf(const char* str, ...);
uint32_t vsprintf(char* str, const char* format, va_list ap);
uint32_t sprintf(char* buf, const char* format, ...);

struct file_object;

typedef struct {
    struct file_object *handle;
} FILE;

FILE *fopen(const char *filename, const char *mode);
void fclose(FILE *fp);
uint32_t fscanf(FILE *fp, const char *format, ...);
uint32_t fprintf(FILE *fp, const char *format, ...);

#endif

