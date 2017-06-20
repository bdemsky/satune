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
struct SATEncoder;
typedef struct SATEncoder SATEncoder;


struct Constraint;
typedef struct Constraint Constraint;

typedef struct BooleanOrder BooleanOrder;
typedef struct BooleanVar BooleanVar;
typedef struct BooleanLogic BooleanLogic;
typedef struct BooleanComp BooleanComp;
typedef struct BooleanPredicate BooleanPredicate;

struct Boolean;
typedef struct Boolean Boolean;

struct IncrementalSolver;
typedef struct IncrementalSolver IncrementalSolver;

struct Set;
typedef struct Set Set;
typedef struct Set MutableSet;

typedef struct ElementFunction ElementFunction;
typedef struct ElementSet ElementSet;

struct Element;
typedef struct Element Element;

typedef struct FunctionOperator FunctionOperator;
typedef struct FunctionTable FunctionTable;

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

struct OrderEncoding;
typedef struct OrderEncoding OrderEncoding;

struct TableEntry;
typedef struct TableEntry TableEntry;

typedef unsigned int uint;
typedef uint64_t VarType;
#endif
