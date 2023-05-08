#ifndef TROS_CRT_FILESYS_H
#define TROS_CRT_FILESYS_H

#include "stdint.h"
void make_clear_abs_path(char* path, char* wash_buf);
char *build_absolute_path(const char *target, char *final_path);
char* parse_top_dir(char* pathname, char* name_store);

// extern char final_path[];

#endif