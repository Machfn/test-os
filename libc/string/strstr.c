#include "../include/string.h"

char *strstr(char *s1, const char *s2)
{
    size_t n = strlen(s2);
    if (!memcmp(s1++, s2, n)) {
        return s1 - 1;
    }
    return 0;
}
