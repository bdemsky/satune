#include "mutableset.h"

MutableSet* allocMutableSet(VarType type){
    MutableSet* set = (MutableSet*)ourmalloc(sizeof(struct Set));
    set->type = type;
    set->members = allocDefVectorInt();
    set->isRange = false;
    return set;
}


void addElementMSet(MutableSet * set, uint64_t element) { pushVectorInt(set->members, element); }
