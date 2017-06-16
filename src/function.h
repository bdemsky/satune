#ifndef FUNCTION_H
#define FUNCTION_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
#include "structs.h"
struct Function {
    enum ArithOp op;
    VectorSet* domains; 
    Set * range;
    enum OverFlowBehavior overflowbehavior;
    Table* table;
};
#endif
