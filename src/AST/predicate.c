#include "predicate.h"
#include "boolean.h"
#include "set.h"

Predicate* allocPredicateOperator(CompOp op, Set ** domain, uint numDomain){
	PredicateOperator* predicate = ourmalloc(sizeof(PredicateOperator));
	GETPREDICATETYPE(predicate)=OPERATORPRED;
	allocInlineArrayInitSet(&predicate->domains, domain, numDomain);
	predicate->op=op;
	return &predicate->base;
}

Predicate* allocPredicateTable(Table* table, UndefinedBehavior undefBehavior){
	PredicateTable* predicate = ourmalloc(sizeof(PredicateTable));
	GETPREDICATETYPE(predicate) = TABLEPRED;
	predicate->table=table;
	predicate->undefinedbehavior=undefBehavior;
	return &predicate->base;
}

void getEqualitySetIntersection(PredicateOperator* predicate, uint* size, uint64_t* result){
	ASSERT( predicate->op == EQUALS);
	//make sure equality has 2 operands
	ASSERT(getSizeArraySet( &predicate->domains) == 2);
	*size=0;
	VectorInt* mems1 = getArraySet(&predicate->domains, 0)->members; 
	uint size1 = getSizeVectorInt(mems1);
	VectorInt* mems2 = getArraySet(&predicate->domains, 1)->members;
	uint size2 = getSizeVectorInt(mems2);
	//FIXME:This isn't efficient, if we a hashset datastructure for Set, we
	// can reduce it to O(n), but for now .... HG
	for(uint i=0; i<size1; i++){
		uint64_t tmp= getVectorInt(mems1, i);
		for(uint j=0; j<size2; j++){
			if(tmp == getVectorInt(mems2, j)){
				result[(*size)++]=tmp;
				break;
			}
		}
	}
	
}

void deletePredicate(Predicate* predicate){
	switch(GETPREDICATETYPE(predicate)) {
	case OPERATORPRED: {
		PredicateOperator * operpred=(PredicateOperator *) predicate;
		deleteInlineArraySet(&operpred->domains);
		break;
	}
	case TABLEPRED: {
		break;
	}
	}
	//need to handle freeing array...
	ourfree(predicate);
}

