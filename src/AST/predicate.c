#include "predicate.h"
#include "boolean.h"
#include "set.h"

Predicate* allocPredicateOperator(CompOp op, Set ** domain, uint numDomain){
	PredicateOperator* This = ourmalloc(sizeof(PredicateOperator));
	GETPREDICATETYPE(This)=OPERATORPRED;
	allocInlineArrayInitSet(&This->domains, domain, numDomain);
	This->op=op;
	return &This->base;
}

Predicate* allocPredicateTable(Table* table, UndefinedBehavior undefBehavior){
	PredicateTable* This = ourmalloc(sizeof(PredicateTable));
	GETPREDICATETYPE(This) = TABLEPRED;
	This->table=table;
	This->undefinedbehavior=undefBehavior;
	return &This->base;
}

// BRIAN: REVISIT
void getEqualitySetIntersection(PredicateOperator* This, uint* size, uint64_t* result){
	ASSERT( This->op == EQUALS);
	//make sure equality has 2 operands
	ASSERT(getSizeArraySet( &This->domains) == 2);
	*size=0;
	VectorInt* mems1 = getArraySet(&This->domains, 0)->members; 
	uint size1 = getSizeVectorInt(mems1);
	VectorInt* mems2 = getArraySet(&This->domains, 1)->members;
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

void deletePredicate(Predicate* This){
	switch(GETPREDICATETYPE(This)) {
	case OPERATORPRED: {
		PredicateOperator * operpred=(PredicateOperator *) This;
		deleteInlineArraySet(&operpred->domains);
		break;
	}
	case TABLEPRED: {
		break;
	}
	}
	//need to handle freeing array...
	ourfree(This);
}

