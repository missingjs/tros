#include "setjmp.h"
#include "stdio.h"
 
jmp_buf my_jump_buffer;
 
static void foo(int status) 
{
    printf("foo(%d) called\n", status);
    longjmp(my_jump_buffer, status + 1); // setjmp() will return status + 1
}
 
int main(void)
{
    volatile int count = 0; // modified locals in setjmp scope must be volatile
    if (setjmp(my_jump_buffer) != 5) // equality against constant expression in an if
    {
        count += 1; // Increment of a volatile variable is deprecated since C++20 (P1152)
        foo(count); // This will cause setjmp() to exit
    }
}