#include "boolean.h"

Boolean* allocBoolean(VarType t){
    Boolean* tmp = (Boolean*) ourmalloc(sizeof (Boolean));
    tmp->vtype=t;
    tmp->btype=_BOOLEAN;
    return tmp;
}

Boolean* allocBooleanOrder(Order* order,uint64_t first, uint64_t second){
    Boolean* tmp = (Boolean*) ourmalloc(sizeof (Boolean));
    tmp ->btype= _ORDER;
    tmp->order = order;
    tmp->first=first;
    tmp->second=second;
}