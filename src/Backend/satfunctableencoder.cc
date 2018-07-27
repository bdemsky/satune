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
#include "tunable.h"
#include "csolver.h"

Edge SATEncoder::encodeEnumEntriesTablePredicateSATEncoder(BooleanPredicate *constraint) {
	ASSERT(constraint->predicate->type == TABLEPRED);
	UndefinedBehavior undefStatus = ((PredicateTable *)constraint->predicate)->undefinedbehavior;
	ASSERT(undefStatus == SATC_IGNOREBEHAVIOR || undefStatus == SATC_FLAGFORCEUNDEFINED);
	Table *table = ((PredicateTable *)constraint->predicate)->table;
	FunctionEncodingType encType = constraint->encoding.type;
	Array<Element *> *inputs = &constraint->inputs;
	uint inputNum = inputs->getSize();
	uint size = table->getSize();
	Polarity polarity = constraint->polarity;
	bool generateNegation = encType == ENUMERATEIMPLICATIONSNEGATE;
	if (generateNegation)
		polarity = negatePolarity(polarity);
	if (undefStatus ==SATC_FLAGFORCEUNDEFINED)
		polarity = P_BOTHTRUEFALSE;

	Edge constraints[size];

	SetIteratorTableEntry *iterator = table->getEntries();
	uint i = 0;
	while (iterator->hasNext()) {
		TableEntry *entry = iterator->next();
		if (generateNegation == (entry->output != 0) && undefStatus == SATC_IGNOREBEHAVIOR) {
			//Skip the irrelevant entries
			continue;
		}
		Edge carray[inputNum];
		for (uint j = 0; j < inputNum; j++) {
			Element *el = inputs->get(j);
			carray[j] = getElementValueConstraint(el, polarity, entry->inputs[j]);
		}
		Edge row;
		switch (undefStatus) {
		case SATC_IGNOREBEHAVIOR:
			row = constraintAND(cnf, inputNum, carray);
			break;
		case SATC_FLAGFORCEUNDEFINED: {
			row = constraintAND(cnf, inputNum, carray);
			uint pSize = constraint->parents.getSize();
			if(!edgeIsVarConst(row) && pSize > (uint)solver->getTuner()->getTunable(PROXYVARIABLE, &proxyparameter)){
				Edge proxy = constraintNewVar(cnf);
				generateProxy(cnf, row, proxy, P_BOTHTRUEFALSE);
				Edge undefConst = encodeConstraintSATEncoder(constraint->undefStatus);
				addConstraintCNF(cnf, constraintIMPLIES(cnf, proxy,  constraintNegate(undefConst)));
				if (generateNegation == (entry->output != 0)) {
					continue;
				}
				row = proxy;
			}
			break;
		}
		default:
			ASSERT(0);
		}
		constraints[i++] = row;
	}
	delete iterator;
	ASSERT(i != 0);
	Edge result = generateNegation ? constraintNegate(constraintOR(cnf, i, constraints))
								: constraintOR(cnf, i, constraints);
	return result;
}

Edge SATEncoder::encodeEnumTablePredicateSATEncoder(BooleanPredicate *constraint) {
#ifdef TRACE_DEBUG
	model_print("Enumeration Table Predicate ...\n");
#endif
	ASSERT(constraint->predicate->type == TABLEPRED);
	Polarity polarity = constraint->polarity;

	//First encode children
	Array<Element *> *inputs = &constraint->inputs;
	uint inputNum = inputs->getSize();
	//Encode all the inputs first ...
	for (uint i = 0; i < inputNum; i++) {
		encodeElementSATEncoder(inputs->get(i));
	}
	PredicateTable *predicate = (PredicateTable *)constraint->predicate;
	switch (predicate->undefinedbehavior) {
	case SATC_IGNOREBEHAVIOR:
	case SATC_FLAGFORCEUNDEFINED:
		return encodeEnumEntriesTablePredicateSATEncoder(constraint);
	default:
		break;
	}
	bool generateNegation = constraint->encoding.type == ENUMERATEIMPLICATIONSNEGATE;
	uint numDomains = constraint->inputs.getSize();

	if (generateNegation)
		polarity = negatePolarity(polarity);
	
        ASSERT(numDomains != 0);
	VectorEdge *clauses = allocDefVectorEdge();

	uint indices[numDomains];	//setup indices
	bzero(indices, sizeof(uint) * numDomains);

	uint64_t vals[numDomains];//setup value array
	for (uint i = 0; i < numDomains; i++) {
		Set *set = constraint->inputs.get(i)->getRange();
		vals[i] = set->getElement(indices[i]);
	}
	bool hasOverflow = false;

	bool notfinished = true;
	while (notfinished) {
		Edge carray[numDomains];
		TableEntry *tableEntry = predicate->table->getTableEntry(vals, numDomains);
		bool isInRange = tableEntry != NULL;
		if (!isInRange && !hasOverflow) {
			hasOverflow = true;
		}
		Edge clause;
		for (uint i = 0; i < numDomains; i++) {
			Element *elem = constraint->inputs.get(i);
			carray[i] = getElementValueConstraint(elem, polarity, vals[i]);
		}

		switch (predicate->undefinedbehavior) {
		case SATC_UNDEFINEDSETSFLAG:
			if (isInRange) {
				clause = constraintAND(cnf, numDomains, carray);
			} else {
				Edge undefConstraint = encodeConstraintSATEncoder(constraint->undefStatus);
				addConstraintCNF(cnf, constraintIMPLIES(cnf,constraintAND(cnf, numDomains, carray), undefConstraint) );
			}
			break;
		case SATC_FLAGIFFUNDEFINED: {
			Edge undefConstraint = encodeConstraintSATEncoder(constraint->undefStatus);
			if (isInRange) {
				clause = constraintAND(cnf, numDomains, carray);
				addConstraintCNF(cnf, constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), constraintNegate(undefConstraint)));
			} else {
				addConstraintCNF(cnf, constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), undefConstraint) );
			}
			break;
		}
		default:
			ASSERT(0);
		}

		if (isInRange) {
#ifdef TRACE_DEBUG
			model_print("added clause in predicate table enumeration ...\n");
			printCNF(clause);
			model_print("\n");
#endif
			pushVectorEdge(clauses, clause);
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
	Edge result = E_NULL;
	ASSERT(getSizeVectorEdge(clauses) != 0);
	result = constraintOR(cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	if (hasOverflow) {
		Edge undefConstraint = encodeConstraintSATEncoder(constraint->undefStatus);
		result = constraintOR2(cnf, result, undefConstraint);
	}
	if (generateNegation) {
		ASSERT(!hasOverflow);
		result = constraintNegate(result);
	}
	deleteVectorEdge(clauses);
	return result;
}

void SATEncoder::encodeEnumEntriesTableElemFuncSATEncoder(ElementFunction *func) {
	UndefinedBehavior undefStatus = ((FunctionTable *) func->getFunction())->undefBehavior;
	ASSERT(undefStatus == SATC_IGNOREBEHAVIOR || undefStatus == SATC_FLAGFORCEUNDEFINED);
	Array<Element *> *elements = &func->inputs;

	Table *table = ((FunctionTable *) (func->getFunction()))->table;
	uint size = table->getSize();
	Edge constraints[size];
	SetIteratorTableEntry *iterator = table->getEntries();
	uint i = 0;
	while (iterator->hasNext()) {
		TableEntry *entry = iterator->next();
		ASSERT(entry != NULL);
		uint inputNum = elements->getSize();
		Edge carray[inputNum];
		for (uint j = 0; j < inputNum; j++) {
			Element *el = elements->get(j);
			carray[j] = getElementValueConstraint(el, P_FALSE, entry->inputs[j]);
		}
		Edge output = getElementValueConstraint(func, P_TRUE, entry->output);
		switch (undefStatus ) {
		case SATC_IGNOREBEHAVIOR: {
                        if(inputNum == 0){
                                addConstraintCNF(cnf, output);
                        }else{
                                addConstraintCNF(cnf, constraintIMPLIES(cnf,constraintAND(cnf, inputNum, carray), output));
                        }
			break;
		}
		case SATC_FLAGFORCEUNDEFINED: {
			Edge undefConst = encodeConstraintSATEncoder(func->overflowstatus);
                        if(inputNum ==0){
                                addConstraintCNF(cnf, constraintAND2(cnf, output, constraintNegate(undefConst)));
                        }else{
                                addConstraintCNF(cnf, constraintIMPLIES(cnf, constraintAND(cnf, inputNum, carray), constraintAND2(cnf, output, constraintNegate(undefConst))));
                        }
			break;
		}
		default:
			ASSERT(0);

		}
	}
	delete iterator;
}

void SATEncoder::encodeEnumTableElemFunctionSATEncoder(ElementFunction *elemFunc) {
#ifdef TRACE_DEBUG
	model_print("Enumeration Table functions ...\n");
#endif
	ASSERT(elemFunc->getFunction()->type == TABLEFUNC);
	
	//First encode children
	Array<Element *> *elements = &elemFunc->inputs;
	for (uint i = 0; i < elements->getSize(); i++) {
		Element *elem = elements->get(i);
		encodeElementSATEncoder(elem);
	}

	FunctionTable *function = (FunctionTable *)elemFunc->getFunction();
	switch (function->undefBehavior) {
	case SATC_IGNOREBEHAVIOR:
	case SATC_FLAGFORCEUNDEFINED:
		return encodeEnumEntriesTableElemFuncSATEncoder(elemFunc);
	default:
		break;
	}

	uint numDomains = elemFunc->inputs.getSize();

	uint indices[numDomains];	//setup indices
	bzero(indices, sizeof(uint) * numDomains);

	uint64_t vals[numDomains];//setup value array
	for (uint i = 0; i < numDomains; i++) {
		Set *set = elemFunc->inputs.get(i)->getRange();
		vals[i] = set->getElement(indices[i]);
	}

	bool notfinished = true;
	while (notfinished) {
		Edge carray[numDomains + 1];
		TableEntry *tableEntry = function->table->getTableEntry(vals, numDomains);
		bool isInRange = tableEntry != NULL;
		ASSERT(function->undefBehavior == SATC_UNDEFINEDSETSFLAG || function->undefBehavior == SATC_FLAGIFFUNDEFINED);
		for (uint i = 0; i < numDomains; i++) {
			Element *elem = elemFunc->inputs.get(i);
			carray[i] = getElementValueConstraint(elem, P_FALSE, vals[i]);
		}
		if (isInRange) {
			carray[numDomains] = getElementValueConstraint(elemFunc, P_TRUE, tableEntry->output);
		}

		switch (function->undefBehavior) {
		case SATC_UNDEFINEDSETSFLAG: {
			if (isInRange) {
                                if(numDomains == 0){
                                        addConstraintCNF(cnf,carray[numDomains]);
                                }else{
                                        addConstraintCNF(cnf, constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), carray[numDomains]));
                                }
			} else {
				Edge undefConstraint = encodeConstraintSATEncoder(elemFunc->overflowstatus);
				addConstraintCNF(cnf, constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), undefConstraint));
			}
			break;
		}
		case SATC_FLAGIFFUNDEFINED: {
			Edge undefConstraint = encodeConstraintSATEncoder(elemFunc->overflowstatus);
			if (isInRange) {
                                if(numDomains == 0){
                                        addConstraintCNF(cnf, constraintAND2(cnf,carray[numDomains], constraintNegate(undefConstraint)) );
                                }else{
                                        Edge freshvar = constraintNewVar(cnf);
                                        addConstraintCNF(cnf, constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), freshvar ));
                                        addConstraintCNF(cnf, constraintIMPLIES(cnf, freshvar, constraintNegate(undefConstraint) ));
                                        addConstraintCNF(cnf, constraintIMPLIES(cnf, freshvar, carray[numDomains]));
                                }
				
			} else {
				addConstraintCNF(cnf, constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), undefConstraint));
			}
			break;
		}
		default:
			ASSERT(0);
		}
		if (isInRange) {
#ifdef TRACE_DEBUG
			model_print("added clause in function table enumeration ...\n");
			printCNF(clause);
			model_print("\n");
#endif
		}

		notfinished = false;
		for (uint i = 0; i < numDomains; i++) {
			uint index = ++indices[i];
			Set *set = elemFunc->inputs.get(i)->getRange();

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
}
