/*      Copyright (c) 2015 Regents of the University of California
 *
 *      Author: Brian Demsky <bdemsky@uci.edu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 as published by the Free Software Foundation.
 */

/** @file common.h
 *  @brief General purpose macros.
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include "config.h"

extern int model_out;
extern int model_err;
extern int switch_alloc;

#define model_dprintf(fd, fmt, ...) do { switch_alloc = 1; dprintf(fd, fmt, ## __VA_ARGS__); switch_alloc = 0; } while (0)

#define model_print(fmt, ...) do { model_dprintf(model_out, fmt, ## __VA_ARGS__); } while (0)

#define model_print_err(fmt, ...) do { model_dprintf(model_err, fmt, ## __VA_ARGS__); } while (0)



#ifdef CONFIG_DEBUG
#define DEBUG(fmt, ...) do { model_print("*** %15s:%-4d %25s() *** " fmt, __FILE__, __LINE__, __func__, ## __VA_ARGS__); } while (0)
#define DBG() DEBUG("\n")
#define DBG_ENABLED() (1)
#else
#define DEBUG(fmt, ...)
#define DBG()
#define DBG_ENABLED() (0)
#endif

void assert_hook(void);

#ifdef CONFIG_ASSERT
#define ASSERT(expr) \
	do { \
		if (!(expr)) { \
			fprintf(stderr, "Error: assertion failed in %s at line %d\n", __FILE__, __LINE__); \
			/* print_trace(); // Trace printing may cause dynamic memory allocation */ \
			assert_hook();                           \
			exit(EXIT_FAILURE); \
		} \
	} while (0)
#else
#define ASSERT(expr) \
	do { } while (0)
#endif/* CONFIG_ASSERT */

#define error_msg(...) fprintf(stderr, "Error: " __VA_ARGS__)

void print_trace(void);
#endif/* __COMMON_H__ */
