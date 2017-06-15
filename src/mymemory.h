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

void * ourmalloc(size_t size);
void ourfree(void *ptr);
void * ourcalloc(size_t count, size_t size);
void * ourrealloc(void *ptr, size_t size);

#endif/* _MY_MEMORY_H */
