#include "predicate.h"

Predicate* allocPredicate(CompOp op, Set ** domain, uint numDomain){
	PredicateOperator* predicate = ourmalloc(sizeof(PredicateOperator));
	GETPREDICATETYPE(predicate)=OPERATORPRED;
	allocInlineArrayInitSet(&predicate->domains, domain, numDomain);
	predicate->op=op;
	return &predicate->base;
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

