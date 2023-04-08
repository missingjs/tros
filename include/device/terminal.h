#ifndef __DEVICE_TERMINAL_H
#define __DEVICE_TERMINAL_H
#include "stdint.h"

int32_t terminal_read(char *buffer, uint32_t size);
int32_t terminal_write(const char *content, uint32_t size);

#endif