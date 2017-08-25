#include "satencoder.h"
#include "common.h"
#include "function.h"
#include "ops.h"
#include "predicate.h"
#include "boolean.h"
#include "table.h"
#include "tableentry.h"
#include "set.h"
#include "element.h"
#include "common.h"
#include "satfuncopencoder.h"

Edge encodeOperatorPredicateSATEncoder(SATEncoder *This, BooleanPredicate *constraint) {
	switch (constraint->encoding.type) {
	case ENUMERATEIMPLICATIONS:
		return encodeEnumOperatorPredicateSATEncoder(This, constraint);
	case CIRCUIT:
		return encodeCircuitOperatorPredicateEncoder(This, constraint);
	default:
		ASSERT(0);
	}
	exit(-1);
}

Edge encodeEnumOperatorPredicateSATEncoder(SATEncoder *This, BooleanPredicate *constraint) {
	PredicateOperator *predicate = (PredicateOperator *)constraint->predicate;
	uint numDomains = predicate->domains.getSize();

	FunctionEncodingType encType = constraint->encoding.type;
	bool generateNegation = encType == ENUMERATEIMPLICATIONSNEGATE;

	/* Call base encoders for children */
	for (uint i = 0; i < numDomains; i++) {
		Element *elem = constraint->inputs.get(i);
		encodeElementSATEncoder(This, elem);
	}
	VectorEdge *clauses = allocDefVectorEdge();	// Setup array of clauses

	uint indices[numDomains];	//setup indices
	bzero(indices, sizeof(uint) * numDomains);

	uint64_t vals[numDomains];//setup value array
	for (uint i = 0; i < numDomains; i++) {
		Set *set = predicate->domains.get(i);
		vals[i] = set->getElement(indices[i]);
	}

	bool notfinished = true;
	while (notfinished) {
		Edge carray[numDomains];

		if (predicate->evalPredicateOperator(vals) ^ generateNegation) {
			//Include this in the set of terms
			for (uint i = 0; i < numDomains; i++) {
				Element *elem = constraint->inputs.get(i);
				carray[i] = getElementValueConstraint(This, elem, vals[i]);
			}
			Edge term = constraintAND(This->cnf, numDomains, carray);
			pushVectorEdge(clauses, term);
		}

		notfinished = false;
		for (uint i = 0; i < numDomains; i++) {
			uint index = ++indices[i];
			Set *set = predicate->domains.get(i);

			if (index < set->getSize()) {
				vals[i] = set->getElement(index);
				notfinished = true;
				break;
			} else {
				indices[i] = 0;
				vals[i] = set->getElement(0);
			}
		}
	}
	if (getSizeVectorEdge(clauses) == 0) {
		deleteVectorEdge(clauses);
		return E_False;
	}
	Edge cor = constraintOR(This->cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	deleteVectorEdge(clauses);
	return generateNegation ? constraintNegate(cor) : cor;
}


void encodeOperatorElementFunctionSATEncoder(SATEncoder *This, ElementFunction *func) {
#ifdef TRACE_DEBUG
	model_print("Operator Function ...\n");
#endif
	FunctionOperator *function = (FunctionOperator *) func->function;
	uint numDomains = func->inputs.getSize();

	/* Call base encoders for children */
	for (uint i = 0; i < numDomains; i++) {
		Element *elem = func->inputs.get(i);
		encodeElementSATEncoder(This, elem);
	}

	VectorEdge *clauses = allocDefVectorEdge();	// Setup array of clauses

	uint indices[numDomains];	//setup indices
	bzero(indices, sizeof(uint) * numDomains);

	uint64_t vals[numDomains];//setup value array
	for (uint i = 0; i < numDomains; i++) {
		Set *set = getElementSet(func->inputs.get(i));
		vals[i] = set->getElement(indices[i]);
	}

	Edge overFlowConstraint = ((BooleanVar *) func->overflowstatus)->var;

	bool notfinished = true;
	while (notfinished) {
		Edge carray[numDomains + 1];

		uint64_t result = function->applyFunctionOperator(numDomains, vals);
		bool isInRange = ((FunctionOperator *)func->function)->isInRangeFunction(result);
		bool needClause = isInRange;
		if (function->overflowbehavior == OVERFLOWSETSFLAG || function->overflowbehavior == FLAGIFFOVERFLOW) {
			needClause = true;
		}

		if (needClause) {
			//Include this in the set of terms
			for (uint i = 0; i < numDomains; i++) {
				Element *elem = func->inputs.get(i);
				carray[i] = getElementValueConstraint(This, elem, vals[i]);
			}
			if (isInRange) {
				carray[numDomains] = getElementValueConstraint(This, func, result);
			}

			Edge clause;
			switch (function->overflowbehavior) {
			case IGNORE:
			case NOOVERFLOW:
			case WRAPAROUND: {
				clause = constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), carray[numDomains]);
				break;
			}
			case FLAGFORCESOVERFLOW: {
				clause = constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), constraintAND2(This->cnf, carray[numDomains], constraintNegate(overFlowConstraint)));
				break;
			}
			case OVERFLOWSETSFLAG: {
				if (isInRange) {
					clause = constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), carray[numDomains]);
				} else {
					clause = constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), overFlowConstraint);
				}
				break;
			}
			case FLAGIFFOVERFLOW: {
				if (isInRange) {
					clause = constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), constraintAND2(This->cnf, carray[numDomains], constraintNegate(overFlowConstraint)));
				} else {
					clause = constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), overFlowConstraint);
				}
				break;
			}
			default:
				ASSERT(0);
			}
#ifdef TRACE_DEBUG
			model_print("added clause in operator function\n");
			printCNF(clause);
			model_print("\n");
#endif
			pushVectorEdge(clauses, clause);
		}

		notfinished = false;
		for (uint i = 0; i < numDomains; i++) {
			uint index = ++indices[i];
			Set *set = getElementSet(func->inputs.get(i));

			if (index < set->getSize()) {
				vals[i] = set->getElement(index);
				notfinished = true;
				break;
			} else {
				indices[i] = 0;
				vals[i] = set->getElement(0);
			}
		}
	}
	if (getSizeVectorEdge(clauses) == 0) {
		deleteVectorEdge(clauses);
		return;
	}
	Edge cor = constraintAND(This->cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	addConstraintCNF(This->cnf, cor);
	deleteVectorEdge(clauses);
}

Edge encodeCircuitOperatorPredicateEncoder(SATEncoder *This, BooleanPredicate *constraint) {
	PredicateOperator *predicate = (PredicateOperator *) constraint->predicate;
	Element *elem0 = constraint->inputs.get(0);
	encodeElementSATEncoder(This, elem0);
	Element *elem1 = constraint->inputs.get(1);
	encodeElementSATEncoder(This, elem1);
	ElementEncoding *ee0 = getElementEncoding(elem0);
	ElementEncoding *ee1 = getElementEncoding(elem1);
	ASSERT(ee0->numVars == ee1->numVars);
	uint numVars = ee0->numVars;
	switch (predicate->op) {
		case EQUALS:
			return generateEquivNVConstraint(This->cnf, numVars, ee0->variables, ee1->variables);
		case LT:
			return generateLTConstraint(This->cnf, numVars, ee0->variables, ee1->variables);
		case GT:
			return generateLTConstraint(This->cnf, numVars, ee1->variables, ee0->variables);
		default:
			ASSERT(0);
	}
	exit(-1);
}

