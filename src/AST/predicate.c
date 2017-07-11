#include "predicate.h"
#include "boolean.h"
#include "set.h"

Predicate* allocPredicateOperator(CompOp op, Set ** domain, uint numDomain){
	PredicateOperator* This = ourmalloc(sizeof(PredicateOperator));
	GETPREDICATETYPE(This)=OPERATORPRED;
	initArrayInitSet(&This->domains, domain, numDomain);
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

bool evalPredicateOperator(PredicateOperator * This, uint64_t * inputs) {
	switch(This->op) {
	case EQUALS:
		return inputs[0]==inputs[1];
	case LT:
		return inputs[0]<inputs[1];
	case GT:
		return inputs[0]>inputs[1];
	case LTE:
		return inputs[0]<=inputs[1];
	case GTE:
		return inputs[0]>=inputs[1];
	}
	ASSERT(0);
	return false;
}
