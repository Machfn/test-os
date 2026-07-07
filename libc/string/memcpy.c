#include "../include/string.h"

void *memcpy(void *restrict dest, const void *restrict src, size_t size)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    for (size_t i = 0; i < size; ++i) {
        d[i] = s[i];
    }
    return (void *)d;
}
