#include "order.h"
#include "structs.h"
#include "set.h"


Order* allocOrder(enum OrderType type, Set * set){
    Order* order = (Order*)ourmalloc(sizeof(Order));
    order->set=set;
    order->constraints = allocDefVectorBoolean();
    order->type=type;
    return order;
}

Boolean* getOrderConstraint(Order* order, uint64_t first, uint64_t second){
    uint size = getSizeVectorInt(order->set->members);
    //First we need to make sure that first and second are part of the list!
    bool exist1=false, exist2=false;
    for(int i=0; i<size; i++){
        if(getVectorInt(order->set->members, i)==first){
            exist1=true;
        }else if(getVectorInt(order->set->members, i) == second){
            exist2=true;
        }else if(exist1 && exist2){
            break;
        }
    }
    ASSERT(exist1 && exist2);
    
}