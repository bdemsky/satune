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

class CSolver;
class SATEncoder;
class Boolean;
class BooleanOrder;
class BooleanVar;
class BooleanLogic;
class BooleanPredicate;
class ASTNode;
class Set;
class MutableSet;

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
class OrderPair;

class OrderElement;
class IntegerEncodingRecord;
class Transform;

class ElementEncoding;
class FunctionEncoding;
class OrderEncoding;

class OrderGraph;
class OrderNode;
class OrderEdge;

class AutoTuner;
class SearchTuner;
class TunableSetting;
class Pass;
class Transform;

struct IncrementalSolver;
typedef struct IncrementalSolver IncrementalSolver;

struct TableEntry;
typedef struct TableEntry TableEntry;

class Tuner;
class TunableDesc;

typedef int TunableParam;

typedef unsigned int uint;
typedef long int int64;
typedef uint64_t VarType;
#endif
