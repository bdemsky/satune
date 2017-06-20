#include "predicate.h"
#include "structs.h"


Predicate* allocPredicate(CompOp op, Set ** domain, uint numDomain){
    Predicate* predicate = (Predicate*) ourmalloc(sizeof(Predicate));
    predicate->domains = allocDefVectorSet();
    for(uint i=0; i<numDomain; i++)
        pushVectorSet(predicate->domains,domain[i]);
    predicate->op=op;
    return predicate;
}

void deletePredicate(Predicate* predicate){
    deleteVectorSet(predicate->domains);
    ourfree(predicate);
}
