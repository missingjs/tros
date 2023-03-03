#include "kernel/print.h"
void main(void) {
    put_char('k');
    put_char('e');
    put_char('r');
    put_char('n');
    put_char('e');
    put_char('l');
    put_char('\n');
    put_char('m');
    put_char('i');
    put_char('s');
    put_char('s');
    put_char('i');
    put_char('n');
    put_char('g');
    put_char('j');
    put_char('s');
    put_char('\n');
    put_char('1');
    put_char('2');
    put_char('\b');
    put_char('3');
    int i=100;
    while(i--)
    {
        put_char('x');
    }
    put_char('\n');
    put_str("I'm a kernel\n");
    while(1);
}
