#ifndef BOOLEAN_H
#define BOOLEAN_H
#include "classlist.h"
#include "mymemory.h"
#include "ops.h"
struct  Boolean {
    VarType vtype;
    enum BooleanType btype;
    Order* order;
    uint64_t first;
    uint64_t second;
};

Boolean* allocBoolean(VarType t);
Boolean* allocBooleanOrder(Order* order,uint64_t first, uint64_t second);
void deleteBoolean(Boolean * this);

#endif
