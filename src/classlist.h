/*      Copyright (c) 2015 Regents of the University of California
 *
 *      Author: Brian Demsky <bdemsky@uci.edu>
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      version 2 as published by the Free Software Foundation.
 */

#ifndef CLASSLIST_H
#define CLASSLIST_H

#include "mymemory.h"
#include <inttypes.h>
#define bool int
#define true 1
#define false 0

struct CSolver;
typedef struct CSolver CSolver;

struct Constraint;
typedef struct Constraint Constraint;

struct Boolean;
typedef struct Boolean Boolean;

struct IncrementalSolver;
typedef struct IncrementalSolver IncrementalSolver;

struct Set;
typedef struct Set Set;

typedef struct Set MutableSet;

struct Element;
typedef struct Element Element;

struct Function;
typedef struct Function Function;

struct Predicate;
typedef struct Predicate Predicate;

struct Table;
typedef struct Table Table;

struct Order;
typedef struct Order Order;

struct ElementEncoding;
typedef struct ElementEncoding ElementEncoding;

struct FunctionEncoding;
typedef struct FunctionEncoding FunctionEncoding;

struct TableEntry;
typedef struct TableEntry TableEntry;


typedef enum ArithOp ArithOp;
typedef enum LogicOp LogicOp;
typedef enum CompOp CompOp;
typedef enum OrderType OrderType;
typedef enum OverFlowBehavior OverFlowBehavior;

typedef unsigned int uint;
typedef uint64_t VarType;
#endif
