#ifndef BSD_QSORT_H
#define BSD_QSORT_H


void bsdqsort(void *a, size_t n, size_t es, int (*cmp)(const void *, const void *));

#endif
