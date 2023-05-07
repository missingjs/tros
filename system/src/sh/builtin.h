#ifndef TROS_BUILTIN_H
#define TROS_BUILTIN_H

#include "stdint.h"

char* buildin_cd(uint32_t argc, char** argv, char *final_path);
int32_t buildin_mkdir(uint32_t argc, char** argv);
int32_t buildin_rmdir(uint32_t argc, char** argv);
int32_t buildin_rm(uint32_t argc, char** argv);
void buildin_pwd(uint32_t argc, char** argv);
void buildin_ps(uint32_t argc, char** argv);
void buildin_clear(uint32_t argc, char** argv);
void buildin_help(uint32_t argc, char** argv);
#endif