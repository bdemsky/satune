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

class CSolver;
struct SATEncoder;
typedef struct SATEncoder SATEncoder;

class Boolean;
class BooleanOrder;
class BooleanVar;
class BooleanLogic;
class BooleanPredicate;
class ASTNode;
class Set;
typedef class Set MutableSet;

class ElementFunction;
class ElementSet;
class ElementConst;
class Element;

class FunctionOperator;
class FunctionTable;
class Function;

class Predicate;
class PredicateTable;
class PredicateOperator;
class Table;
class Order;

struct IncrementalSolver;
typedef struct IncrementalSolver IncrementalSolver;

struct OrderPair;
typedef struct OrderPair OrderPair;

struct OrderElement;
typedef struct OrderElement OrderElement;

struct ElementEncoding;
typedef struct ElementEncoding ElementEncoding;

struct FunctionEncoding;
typedef struct FunctionEncoding FunctionEncoding;

struct OrderEncoding;
typedef struct OrderEncoding OrderEncoding;

struct TableEntry;
typedef struct TableEntry TableEntry;

struct OrderGraph;
typedef struct OrderGraph OrderGraph;

struct OrderNode;
typedef struct OrderNode OrderNode;

struct OrderEdge;
typedef struct OrderEdge OrderEdge;

struct OrderEncoder;
typedef struct OrderEncoder OrderEncoder;

struct Tuner;
typedef struct Tuner Tuner;
struct TunableDesc;
typedef struct TunableDesc TunableDesc;
typedef int TunableParam;

typedef unsigned int uint;
typedef long int int64;
typedef uint64_t VarType;
#endif
