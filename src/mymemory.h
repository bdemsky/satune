/*      Copyright (c) 2015 Regents of the University of California
 *
 *      Author: Brian Demsky <bdemsky@uci.edu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 as published by the Free Software Foundation.
 */

/** @file mymemory.h
 *  @brief Memory allocation functions.
 */

#ifndef CSAT_MY_MEMORY_H
#define CSAT_MY_MEMORY_H
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>

#include "config.h"

/*
   void * ourmalloc(size_t size);
   void ourfree(void *ptr);
   void * ourcalloc(size_t count, size_t size);
   void * ourrealloc(void *ptr, size_t size);
 */

static inline void *ourmalloc(size_t size) { return malloc(size); }
static inline void ourfree(void *ptr) { free(ptr); }
static inline void *ourcalloc(size_t count, size_t size) { return calloc(count, size); }
static inline void *ourrealloc(void *ptr, size_t size) { return realloc(ptr, size); }

#define CMEMALLOC                           \
	void *operator new(size_t size) {       \
		return ourmalloc(size);                \
	}                                                  \
	void operator delete(void *p, size_t size) {       \
		ourfree(p);                                      \
	}                                                  \
	void *operator new[](size_t size) {               \
		return ourmalloc(size);                          \
	}                                                    \
	void operator delete[](void *p, size_t size) {       \
		ourfree(p);                                        \
	}                                                                     \
	void *operator new(size_t size, void *p) {															/* placement new */ \
		return p;                                                           \
	}

#endif/* _MY_MEMORY_H */
