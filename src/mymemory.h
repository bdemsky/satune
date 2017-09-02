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

#if 0
void * model_malloc(size_t size);
void model_free(void *ptr);
void * model_calloc(size_t count, size_t size);
void * model_realloc(void *ptr, size_t size);


#define ourmalloc model_malloc
#define ourfree model_free
#define ourrealloc model_realloc
#define ourcalloc model_calloc

#else
static inline void *ourmalloc(size_t size) { return malloc(size); }
static inline void ourfree(void *ptr) { free(ptr); }
static inline void *ourcalloc(size_t count, size_t size) { return calloc(count, size); }
static inline void *ourrealloc(void *ptr, size_t size) { return realloc(ptr, size); }
#endif

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
