#include "predicate.h"

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
	return predicate;
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

