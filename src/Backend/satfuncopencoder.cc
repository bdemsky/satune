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

Edge SATEncoder::encodeOperatorPredicateSATEncoder(BooleanPredicate *constraint) {
	switch (constraint->encoding.type) {
	case ENUMERATEIMPLICATIONS:
		return encodeEnumOperatorPredicateSATEncoder(constraint);
	case CIRCUIT:
		return encodeCircuitOperatorPredicateEncoder(constraint);
	default:
		ASSERT(0);
	}
	exit(-1);
}

Edge SATEncoder::encodeEnumEqualsPredicateSATEncoder(BooleanPredicate *constraint) {
	Polarity polarity = constraint->polarity;

	/* Call base encoders for children */
	for (uint i = 0; i < 2; i++) {
		Element *elem = constraint->inputs.get(i);
		encodeElementSATEncoder(elem);
	}
	VectorEdge *clauses = vector;

	Set *set0 = constraint->inputs.get(0)->getRange();
	uint size0 = set0->getSize();

	Set *set1 = constraint->inputs.get(1)->getRange();
	uint size1 = set1->getSize();

	uint64_t val0 = set0->getElement(0);
	uint64_t val1 = set1->getElement(0);
	if (size0 != 0 && size1 != 0)
		for (uint i = 0, j = 0; true; ) {
			if (val0 == val1) {
				Edge carray[2];
				carray[0] = getElementValueConstraint(constraint->inputs.get(0), polarity, val0);
				carray[1] = getElementValueConstraint(constraint->inputs.get(1), polarity, val0);
				Edge term = constraintAND(cnf, 2, carray);
				pushVectorEdge(clauses, term);
				i++; j++;
				if (i < size0)
					val0 = set0->getElement(i);
				else
					break;
				if (j < size1)
					val1 = set1->getElement(j);
				else
					break;
			} else if (val0 < val1) {
				i++;
				if (i < size0)
					val0 = set0->getElement(i);
				else
					break;
			} else {
				j++;
				if (j < size1)
					val1 = set1->getElement(j);
				else
					break;
			}
		}
	if (getSizeVectorEdge(clauses) == 0) {
		return E_False;
	}
	Edge cor = constraintOR(cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	clearVectorEdge(clauses);
	return cor;
}

Edge SATEncoder::encodeUnaryPredicateSATEncoder(BooleanPredicate *constraint) {
	Polarity polarity = constraint->polarity;
	PredicateOperator *predicate = (PredicateOperator *)constraint->predicate;
	CompOp op = predicate->getOp();

	/* Call base encoders for children */
	for (uint i = 0; i < 2; i++) {
		Element *elem = constraint->inputs.get(i);
		encodeElementSATEncoder(elem);
	}
	VectorEdge *clauses = vector;

	Element *elem0 = constraint->inputs.get(0);
	Element *elem1 = constraint->inputs.get(1);

	//Eliminate symmetric cases
	if (op == SATC_GT) {
		op = SATC_LT;
		Element *tmp = elem0;
		elem0 = elem1;
		elem1 = elem0;
	} else if (op == SATC_GTE) {
		op = SATC_LTE;
		Element *tmp = elem0;
		elem0 = elem1;
		elem1 = elem0;
	}

	Set *set0 = elem0->getRange();
	uint size0 = set0->getSize();
	Edge *vars0 = elem0->getElementEncoding()->variables;

	Set *set1 = elem1->getRange();
	uint size1 = set1->getSize();
	Edge *vars1 = elem1->getElementEncoding()->variables;


	uint64_t val0 = set0->getElement(0);
	uint64_t val1 = set1->getElement(0);
	if (size0 != 0 && size1 != 0) {
		for (uint i = 0, j = 0; true; ) {
			if (val0 > val1 || (op == SATC_LT && val0 == val1)) {
				j++;
				if (j == size1) {
					//need to assert val0 isn't this big
					if (i == 0)
						return E_False;//Can't satisfy this constraint
					pushVectorEdge(clauses, constraintNegate(vars0[i - 1]));
					break;
				}
				val1 = set1->getElement(j);
			} else {
				if (i == 0) {
					if (j != 0) {
						pushVectorEdge(clauses, vars1[j - 1]);
					}
				} else {
					if (j != 0) {
						Edge term = constraintIMPLIES(cnf, vars0[i - 1], vars1[j - 1]);
						pushVectorEdge(clauses, term);
					}
				}
				i++;
				if (i == size0)
					break;
				val0 = set0->getElement(i);
			}
		}
	}
	//Trivially true constraint
	if (getSizeVectorEdge(clauses) == 0)
		return E_True;

	Edge cand = constraintAND(cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	clearVectorEdge(clauses);
	return cand;
}

Edge SATEncoder::encodeEnumOperatorPredicateSATEncoder(BooleanPredicate *constraint) {
	PredicateOperator *predicate = (PredicateOperator *)constraint->predicate;
	uint numDomains = constraint->inputs.getSize();
	Polarity polarity = constraint->polarity;
	FunctionEncodingType encType = constraint->encoding.type;
	bool generateNegation = encType == ENUMERATEIMPLICATIONSNEGATE;
	if (generateNegation)
		polarity = negatePolarity(polarity);

	CompOp op = predicate->getOp();
	if (!generateNegation && op == SATC_EQUALS)
		return encodeEnumEqualsPredicateSATEncoder(constraint);

	if (!generateNegation && numDomains == 2 &&
			(op == SATC_LT || op == SATC_GT || op == SATC_LTE || op == SATC_GTE) &&
			constraint->inputs.get(0)->encoding.type == UNARY &&
			constraint->inputs.get(1)->encoding.type == UNARY) {
		return encodeUnaryPredicateSATEncoder(constraint);
	}

	/* Call base encoders for children */
	for (uint i = 0; i < numDomains; i++) {
		Element *elem = constraint->inputs.get(i);
		encodeElementSATEncoder(elem);
	}
	VectorEdge *clauses = vector;

	uint indices[numDomains];	//setup indices
	bzero(indices, sizeof(uint) * numDomains);

	uint64_t vals[numDomains];//setup value array
	for (uint i = 0; i < numDomains; i++) {
		Set *set = constraint->inputs.get(i)->getRange();
		vals[i] = set->getElement(indices[i]);
	}

	bool notfinished = true;
	Edge carray[numDomains];
	while (notfinished) {
		if (predicate->evalPredicateOperator(vals) != generateNegation) {
			//Include this in the set of terms
			for (uint i = 0; i < numDomains; i++) {
				Element *elem = constraint->inputs.get(i);
				carray[i] = getElementValueConstraint(elem, polarity, vals[i]);
			}
			Edge term = constraintAND(cnf, numDomains, carray);
			pushVectorEdge(clauses, term);
			ASSERT(getSizeVectorEdge(clauses) > 0);
		}

		notfinished = false;
		for (uint i = 0; i < numDomains; i++) {
			uint index = ++indices[i];
			Set *set = constraint->inputs.get(i)->getRange();

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
		return E_False;
	}
	Edge cor = constraintOR(cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	clearVectorEdge(clauses);
	return generateNegation ? constraintNegate(cor) : cor;
}


void SATEncoder::encodeOperatorElementFunctionSATEncoder(ElementFunction *func) {
#ifdef TRACE_DEBUG
	model_print("Operator Function ...\n");
#endif
	FunctionOperator *function = (FunctionOperator *) func->getFunction();
	uint numDomains = func->inputs.getSize();

	/* Call base encoders for children */
	for (uint i = 0; i < numDomains; i++) {
		Element *elem = func->inputs.get(i);
		encodeElementSATEncoder(elem);
	}

	VectorEdge *clauses = allocDefVectorEdge();	// Setup array of clauses

	uint indices[numDomains];	//setup indices
	bzero(indices, sizeof(uint) * numDomains);

	uint64_t vals[numDomains];//setup value array
	for (uint i = 0; i < numDomains; i++) {
		Set *set = func->inputs.get(i)->getRange();
		vals[i] = set->getElement(indices[i]);
	}

	bool notfinished = true;
	Edge carray[numDomains + 1];
	while (notfinished) {
		uint64_t result = function->applyFunctionOperator(numDomains, vals);
		bool isInRange = ((FunctionOperator *)func->getFunction())->isInRangeFunction(result);
		bool needClause = isInRange;
		if (function->overflowbehavior == SATC_OVERFLOWSETSFLAG || function->overflowbehavior == SATC_FLAGIFFOVERFLOW) {
			needClause = true;
		}

		if (needClause) {
			//Include this in the set of terms
			for (uint i = 0; i < numDomains; i++) {
				Element *elem = func->inputs.get(i);
				carray[i] = getElementValueConstraint(elem, P_FALSE, vals[i]);
			}
			if (isInRange) {
				carray[numDomains] = getElementValueConstraint(func, P_TRUE, result);
			}

			Edge clause;
			switch (function->overflowbehavior) {
			case SATC_IGNORE:
			case SATC_NOOVERFLOW:
			case SATC_WRAPAROUND: {
				clause = constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), carray[numDomains]);
				break;
			}
			case SATC_FLAGFORCESOVERFLOW: {
				Edge overFlowConstraint = encodeConstraintSATEncoder(func->overflowstatus);
				clause = constraintIMPLIES(cnf,constraintAND(cnf, numDomains, carray), constraintAND2(cnf, carray[numDomains], constraintNegate(overFlowConstraint)));
				break;
			}
			case SATC_OVERFLOWSETSFLAG: {
				if (isInRange) {
					clause = constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), carray[numDomains]);
				} else {
					Edge overFlowConstraint = encodeConstraintSATEncoder(func->overflowstatus);
					clause = constraintIMPLIES(cnf,constraintAND(cnf, numDomains, carray), overFlowConstraint);
				}
				break;
			}
			case SATC_FLAGIFFOVERFLOW: {
				Edge overFlowConstraint = encodeConstraintSATEncoder(func->overflowstatus);
				if (isInRange) {
					clause = constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), constraintAND2(cnf, carray[numDomains], constraintNegate(overFlowConstraint)));
				} else {
					clause = constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), overFlowConstraint);
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
			Set *set = func->inputs.get(i)->getRange();

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
	Edge cand = constraintAND(cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	addConstraintCNF(cnf, cand);
	deleteVectorEdge(clauses);
}

Edge SATEncoder::encodeCircuitOperatorPredicateEncoder(BooleanPredicate *constraint) {
	PredicateOperator *predicate = (PredicateOperator *) constraint->predicate;
	Element *elem0 = constraint->inputs.get(0);
	encodeElementSATEncoder(elem0);
	Element *elem1 = constraint->inputs.get(1);
	encodeElementSATEncoder(elem1);
	ElementEncoding *ee0 = elem0->getElementEncoding();
	ElementEncoding *ee1 = elem1->getElementEncoding();
	ASSERT(ee0->numVars == ee1->numVars);
	uint numVars = ee0->numVars;
	switch (predicate->getOp()) {
	case SATC_EQUALS:
		return generateEquivNVConstraint(cnf, numVars, ee0->variables, ee1->variables);
	case SATC_LT:
		return generateLTConstraint(cnf, numVars, ee0->variables, ee1->variables);
	case SATC_GT:
		return generateLTConstraint(cnf, numVars, ee1->variables, ee0->variables);
	default:
		ASSERT(0);
	}
	exit(-1);
}

