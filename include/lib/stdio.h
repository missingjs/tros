#ifndef __LIB_STDIO_H
#define __LIB_STDIO_H
#include "stdint.h"
#include "stdint.h"
typedef char* va_list;
int printf(const char* str, ...);
int vsprintf(char* str, const char* format, va_list ap);
int sprintf(char* buf, const char* format, ...);

typedef struct {
    int fileno;
    int flags;
    int status;
    char *read_buf;
    char *read_ptr;
    char *read_end;
} FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

FILE *fopen(const char *filename, const char *mode);
void fclose(FILE *fp);
// uint32_t fscanf(FILE *fp, const char *format, ...);
// uint32_t fprintf(FILE *fp, const char *format, ...);
char *fgets(char *str, int count, FILE *fp);
int fputs(const char *str, FILE *fp);
int fprintf(FILE *fp, const char* str, ...);
int vfprintf(FILE *fp, const char *str, va_list ap);
#endif
