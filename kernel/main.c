#include "kernel/print.h"
void main(void) {
    put_str("I'm a kernel\n");
    put_str("missingjs 2023-03-04 09:12 hi~\n");
    put_int(0);
    put_char('\n');
    put_int(9);
    put_char('\n');
    put_int(0x00021a3f);
    put_char('\n');
    put_int(0x12345678);
    put_char('\n');
    put_int(0x00000000);
    while(1);
}
