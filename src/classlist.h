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
#include "classes.h"


class BooleanOrder;
class BooleanVar;

class BooleanPredicate;
class ASTNode;



class ElementFunction;
class ElementSet;
class ElementConst;

class FunctionOperator;
class FunctionTable;



class PredicateTable;
class PredicateOperator;


class OrderPair;

class OrderElement;
class IntegerEncodingRecord;
class Transform;
class Pass;

class ElementEncoding;
class FunctionEncoding;
class OrderEncoding;

class OrderGraph;
class OrderNode;
class OrderEdge;

class AutoTuner;
class SearchTuner;
class TunableSetting;

class TunableDesc;

struct IncrementalSolver;
typedef struct IncrementalSolver IncrementalSolver;

struct TableEntry;
typedef struct TableEntry TableEntry;

typedef int TunableParam;
typedef unsigned int uint;
typedef long int int64;

#endif
