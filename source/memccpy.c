#include <stddef.h>

void *memccpy(void *restrict dest, const void *restrict src, int c, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    unsigned char ch = (unsigned char)c;

    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
        if (s[i] == ch) {
            return d + i + 1;
        }
    }
    return NULL;
}