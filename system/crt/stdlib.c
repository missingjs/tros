#include "stdlib.h"
#include "string.h"

char **environ;

char *getenv(const char *name) {
    char *const *p = environ;
    char *s;
    while ((s = *p++)) {
        char *r = s;
        while (*r && *r != '=') {
            ++r;
        }
        if (!*r) {  // has no '=', invalid format
            continue;
        }
        if (strncmp(s, name, (uint32_t)(r - s)) == 0) {
            return r + 1;
        }
    }
    return NULL;
}
