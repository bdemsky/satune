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

Edge SATEncoder::encodeEnumEntriesTablePredicateSATEncoder(BooleanPredicate *constraint) {
	ASSERT(constraint->predicate->type == TABLEPRED);
	UndefinedBehavior undefStatus = ((PredicateTable *)constraint->predicate)->undefinedbehavior;
	ASSERT(undefStatus == SATC_IGNOREBEHAVIOR || undefStatus == SATC_FLAGFORCEUNDEFINED);
	Table *table = ((PredicateTable *)constraint->predicate)->table;
	FunctionEncodingType encType = constraint->encoding.type;
	Array<Element *> *inputs = &constraint->inputs;
	uint inputNum = inputs->getSize();
	uint size = table->entries->getSize();
	bool generateNegation = encType == ENUMERATEIMPLICATIONSNEGATE;
	Edge constraints[size];
	Edge undefConst = encodeConstraintSATEncoder(constraint->undefStatus);
	printCNF(undefConst);
	model_print("**\n");
	SetIteratorTableEntry *iterator = table->entries->iterator();
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
			carray[j] = getElementValueConstraint(el, entry->inputs[j]);
			printCNF(carray[j]);
			model_print("\n");
		}
		Edge row;
		switch (undefStatus) {
		case SATC_IGNOREBEHAVIOR:
			row = constraintAND(cnf, inputNum, carray);
			break;
		case SATC_FLAGFORCEUNDEFINED: {
			addConstraintCNF(cnf, constraintIMPLIES(cnf, constraintAND(cnf, inputNum, carray),  constraintNegate(undefConst)));
			if (generateNegation == (entry->output != 0)) {
				continue;
			}
			row = constraintAND(cnf, inputNum, carray);
			break;
		}
		default:
			ASSERT(0);
		}
		constraints[i++] = row;
		printCNF(row);

		model_print("\n\n");
	}
	delete iterator;
	ASSERT(i != 0);
	Edge result = generateNegation ? constraintNegate(constraintOR(cnf, i, constraints))
								: constraintOR(cnf, i, constraints);
	printCNF(result);
	return result;
}
Edge SATEncoder::encodeEnumTablePredicateSATEncoder(BooleanPredicate *constraint) {
#ifdef TRACE_DEBUG
	model_print("Enumeration Table Predicate ...\n");
#endif
	ASSERT(constraint->predicate->type == TABLEPRED);
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
	uint numDomains = predicate->table->domains.getSize();

	VectorEdge *clauses = allocDefVectorEdge();

	uint indices[numDomains];	//setup indices
	bzero(indices, sizeof(uint) * numDomains);

	uint64_t vals[numDomains];//setup value array
	for (uint i = 0; i < numDomains; i++) {
		Set *set = predicate->table->domains.get(i);
		vals[i] = set->getElement(indices[i]);
	}
	bool hasOverflow = false;
	Edge undefConstraint = encodeConstraintSATEncoder(constraint->undefStatus);
	printCNF(undefConstraint);
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
			carray[i] = getElementValueConstraint(elem, vals[i]);
		}

		switch (predicate->undefinedbehavior) {
		case SATC_UNDEFINEDSETSFLAG:
			if (isInRange) {
				clause = constraintAND(cnf, numDomains, carray);
			} else {
				addConstraintCNF(cnf, constraintIMPLIES(cnf,constraintAND(cnf, numDomains, carray), undefConstraint) );
			}
			break;
		case SATC_FLAGIFFUNDEFINED:
			if (isInRange) {
				clause = constraintAND(cnf, numDomains, carray);
				addConstraintCNF(cnf, constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), constraintNegate(undefConstraint)));
			} else {
				addConstraintCNF(cnf, constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), undefConstraint) );
			}
			break;

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
			Set *set = predicate->table->domains.get(i);

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
	UndefinedBehavior undefStatus = ((FunctionTable *) func->function)->undefBehavior;
	ASSERT(undefStatus == SATC_IGNOREBEHAVIOR || undefStatus == SATC_FLAGFORCEUNDEFINED);
	Array<Element *> *elements = &func->inputs;
	Table *table = ((FunctionTable *) (func->function))->table;
	uint size = table->entries->getSize();
	Edge constraints[size];
	SetIteratorTableEntry *iterator = table->entries->iterator();
	uint i = 0;
	while (iterator->hasNext()) {
		TableEntry *entry = iterator->next();
		ASSERT(entry != NULL);
		uint inputNum = elements->getSize();
		Edge carray[inputNum];
		for (uint j = 0; j < inputNum; j++) {
			Element *el = elements->get(j);
			carray[j] = getElementValueConstraint(el, entry->inputs[j]);
		}
		Edge output = getElementValueConstraint(func, entry->output);
		Edge row;
		switch (undefStatus ) {
		case SATC_IGNOREBEHAVIOR: {
			row = constraintIMPLIES(cnf,constraintAND(cnf, inputNum, carray), output);
			break;
		}
		case SATC_FLAGFORCEUNDEFINED: {
			Edge undefConst = encodeConstraintSATEncoder(func->overflowstatus);
			row = constraintIMPLIES(cnf, constraintAND(cnf, inputNum, carray), constraintAND2(cnf, output, constraintNegate(undefConst)));
			break;
		}
		default:
			ASSERT(0);

		}
		constraints[i++] = row;
	}
	delete iterator;
	addConstraintCNF(cnf, constraintAND(cnf, size, constraints));
}

void SATEncoder::encodeEnumTableElemFunctionSATEncoder(ElementFunction *elemFunc) {
#ifdef TRACE_DEBUG
	model_print("Enumeration Table functions ...\n");
#endif
	ASSERT(elemFunc->function->type == TABLEFUNC);
	//First encode children
	Array<Element *> *elements = &elemFunc->inputs;
	for (uint i = 0; i < elements->getSize(); i++) {
		Element *elem = elements->get(i);
		encodeElementSATEncoder(elem);
	}

	FunctionTable *function = (FunctionTable *)elemFunc->function;
	switch (function->undefBehavior) {
	case SATC_IGNOREBEHAVIOR:
	case SATC_FLAGFORCEUNDEFINED:
		return encodeEnumEntriesTableElemFuncSATEncoder(elemFunc);
	default:
		break;
	}

	uint numDomains = function->table->domains.getSize();

	VectorEdge *clauses = allocDefVectorEdge();	// Setup array of clauses

	uint indices[numDomains];	//setup indices
	bzero(indices, sizeof(uint) * numDomains);

	uint64_t vals[numDomains];//setup value array
	for (uint i = 0; i < numDomains; i++) {
		Set *set = function->table->domains.get(i);
		vals[i] = set->getElement(indices[i]);
	}

	Edge undefConstraint = encodeConstraintSATEncoder(elemFunc->overflowstatus);
	bool notfinished = true;
	while (notfinished) {
		Edge carray[numDomains + 1];
		TableEntry *tableEntry = function->table->getTableEntry(vals, numDomains);
		bool isInRange = tableEntry != NULL;
		ASSERT(function->undefBehavior == SATC_UNDEFINEDSETSFLAG || function->undefBehavior == SATC_FLAGIFFUNDEFINED);
		for (uint i = 0; i < numDomains; i++) {
			Element *elem = elemFunc->inputs.get(i);
			carray[i] = getElementValueConstraint(elem, vals[i]);
		}
		if (isInRange) {
			carray[numDomains] = getElementValueConstraint(elemFunc, tableEntry->output);
		}

		Edge clause;
		switch (function->undefBehavior) {
		case SATC_UNDEFINEDSETSFLAG: {
			if (isInRange) {
				//FIXME: Talk to Brian, It should be IFF not only IMPLY. --HG
				clause = constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), carray[numDomains]);
			} else {
				addConstraintCNF(cnf, constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), undefConstraint));
			}
			break;
		}
		case SATC_FLAGIFFUNDEFINED: {
			if (isInRange) {
				clause = constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), carray[numDomains]);
				addConstraintCNF(cnf, constraintIMPLIES(cnf, constraintAND(cnf, numDomains, carray), constraintNegate(undefConstraint) ));
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
			pushVectorEdge(clauses, clause);
		}

		notfinished = false;
		for (uint i = 0; i < numDomains; i++) {
			uint index = ++indices[i];
			Set *set = function->table->domains.get(i);

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
	Edge cor = constraintAND(cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	addConstraintCNF(cnf, cor);
	deleteVectorEdge(clauses);
}
