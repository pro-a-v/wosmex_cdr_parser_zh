#ifndef BOYERMOOREHORSPOOL_H
#define BOYERMOOREHORSPOOL_H
#include <cstring>
#include <limits.h>

const unsigned char *
boyermoore_horspool_memmem(const unsigned char* haystack, size_t hlen,
                           const unsigned char* needle,   size_t nlen);


#endif // BOYERMOOREHORSPOOL_H

