#ifndef TROS_BUILTIN_H
#define TROS_BUILTIN_H

#include "stdint.h"

char* buildin_cd(uint32_t argc, char** argv, char *final_path);
void buildin_ps(uint32_t argc, char** argv);
void buildin_help(uint32_t argc, char** argv);
#endif