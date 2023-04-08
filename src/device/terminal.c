#include "device/console.h"
#include "device/ioqueue.h"
#include "device/keyboard.h"
#include "device/terminal.h"

int32_t terminal_read(char *buf, uint32_t size) {
    char* buffer = buf;
    uint32_t bytes_read = 0;
    while (bytes_read < size) {
        int data = ioq_getchar(&kbd_buf);
        if (data < 0) {
            break;
        }
        *buffer = (char) data;
        bytes_read++;
        buffer++;
    }
    return (int32_t)bytes_read;
}

int32_t terminal_write(const char *content, uint32_t size) {
    console_put_str_n(content, size);
    return (int32_t) size;
}