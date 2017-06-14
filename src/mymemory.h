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

#ifndef _MY_MEMORY_H
#define _MY_MEMORY_H
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>

#include "config.h"

/** MEMALLOC declares the allocators for a class to allocate
 *	memory in the non-snapshotting heap. */
/*
#define MEMALLOC										 \
	void * operator new(size_t size) { \
		return model_malloc(size); \
	} \
	void operator delete(void *p, size_t size) { \
		model_free(p);                                   \
	} \
	void * operator new[](size_t size) { \
		return model_malloc(size); \
	} \
	void operator delete[](void *p, size_t size) { \
		model_free(p); \
	} \
	void * operator new(size_t size, void *p) {     \
		return p;                                                                                                                                                               \
	}
*/

void * ourmalloc(size_t size);
void ourfree(void *ptr);
void * ourcalloc(size_t count, size_t size);
void * ourrealloc(void *ptr, size_t size);

void *model_malloc(size_t size);
void *model_calloc(size_t count, size_t size);
void * model_realloc(void *ptr, size_t size);
void model_free(void *ptr);

#endif/* _MY_MEMORY_H */
