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
#include "astnode.h"


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
class Transformer;
class AnalysisData;

class ElementEncoding;
class FunctionEncoding;
class OrderEncoding;

class OrderGraph;
class OrderNodeKey;
class OrderNode;
class OrderEdge;
class DOREdge;

class AutoTuner;
class CompTuner;
class SearchTuner;
class TunableSetting;

class SerializeTuner;

class TunableDesc;

class OrderResolver;
class DecomposeOrderResolver;

class EncodingGraph;
class EncodingNode;
class EncodingEdge;
class EncodingSubGraph;
class SignatureEnc;
class Signature;
class ElementSig;
class SetSig;
class BooleanSig;
struct IncrementalSolver;
typedef struct IncrementalSolver IncrementalSolver;
struct TableEntry;
typedef struct TableEntry TableEntry;
typedef int TunableParam;
#endif
