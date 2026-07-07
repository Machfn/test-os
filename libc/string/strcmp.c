#include "../include/string.h"

int strcmp(const char *s1, const char *s2)
{
    const unsigned char *p1 = (unsigned char *)s1;
    const unsigned char *p2 = (unsigned char *)s2;

    while (*p1 && (*p1 == *p2)) {
        p1++;
        p2++;
    }
    return *p1 - *p2;
}
