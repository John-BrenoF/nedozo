/* ============================================================
   include/string.h — Protótipos para funções de memória e string
   ============================================================ */
#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void *memset(void *dst, int c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
int   memcmp(const void *a, const void *b, size_t n);
size_t strlen(const char *s);

#endif