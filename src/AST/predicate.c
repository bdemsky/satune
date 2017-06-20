#include "predicate.h"
#include "structs.h"


Predicate* allocPredicate(CompOp op, Set ** domain, uint numDomain){
	PredicateOperator* predicate = ourmalloc(sizeof(PredicateOperator));
	GETPREDICATETYPE(predicate)=OPERATORPRED;
	predicate->numDomains=numDomain;
	predicate->domains = ourmalloc(numDomain * sizeof(Set *));
	memcpy(predicate->domains, domain, numDomain * sizeof(Set *));
	predicate->op=op;
	return &predicate->base;
}

void deletePredicate(Predicate* predicate){
	switch(GETPREDICATETYPE(predicate)) {
	case OPERATORPRED: {
		PredicateOperator * operpred=(PredicateOperator *) predicate;
		ourfree(operpred->domains);
		break;
	}
	case TABLEPRED: {
		break;
	}
	}

	//need to handle freeing array...
	ourfree(predicate);
}

