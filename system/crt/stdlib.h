#ifndef TROS_CRT_STDLIB_H
#define TROS_CRT_STDLIB_H

#include "stddef.h"

extern char **environ;

char *getenv(const char *name);

#endif