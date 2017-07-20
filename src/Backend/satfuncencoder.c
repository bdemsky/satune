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

Edge encodeEnumEntriesTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	Table* table = ((PredicateTable*)constraint->predicate)->table;
	FunctionEncodingType encType = constraint->encoding.type;
	ArrayElement* inputs = &constraint->inputs;
	uint inputNum =getSizeArrayElement(inputs);
	//Encode all the inputs first ...
	for(uint i=0; i<inputNum; i++){
		encodeElementSATEncoder(This, getArrayElement(inputs, i));
	}
	uint size = getSizeHashSetTableEntry(table->entrie);
	bool generateNegation = encType == ENUMERATEIMPLICATIONSNEGATE;
	Edge constraints[size];
	HSIteratorTableEntry* iterator = iteratorTableEntry(table->entrie);
	uint i=0;
	while(hasNextTableEntry(iterator)){
		TableEntry* entry = nextTableEntry(iterator);
		if(generateNegation == entry->output) {
			//Skip the irrelevant entries
			continue;
		}
		Edge carray[inputNum];
		for(uint j=0; j<inputNum; j++){
			Element* el = getArrayElement(inputs, j);
			carray[j] = getElementValueConstraint(This, el, entry->inputs[j]);
		}
		constraints[i++]=constraintAND(This->cnf, inputNum, carray);
	}
	Edge result=constraintOR(This->cnf, size, constraints);

	return generateNegation ? constraintNegate(result) : result;
}
Edge encodeEnumTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	//FIXME
	return E_NULL;
}

Edge encodeOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint) {
	switch(constraint->encoding.type){
		case ENUMERATEIMPLICATIONS:
			return encodeEnumOperatorPredicateSATEncoder(This, constraint);
		case CIRCUIT:
			return encodeCircuitOperatorPredicateEncoder(This, constraint);
		default:
			ASSERT(0);
	}
	exit(-1);
}

Edge encodeCircuitOperatorPredicateEncoder(SATEncoder *This, BooleanPredicate * constraint) {
	PredicateOperator * predicate = (PredicateOperator*) constraint->predicate;
	
	switch(predicate->op) {
	case EQUALS: {
		return encodeCircuitEquals(This, constraint);
	}
	default:
		ASSERT(0);
	}
	exit(-1);
}

Edge encodeCircuitEquals(SATEncoder * This, BooleanPredicate * constraint) {
	PredicateOperator * predicate = (PredicateOperator*) constraint->predicate;
	ASSERT(getSizeArraySet(&predicate->domains) == 2);
	Element *elem0 = getArrayElement( &constraint->inputs, 0);
	Element *elem1 = getArrayElement( &constraint->inputs, 1);
	ElementEncoding *ee0=getElementEncoding(elem0);
	ElementEncoding *ee1=getElementEncoding(elem1);
	ASSERT(ee0->numVars==ee1->numVars);
	uint numVars=ee0->numVars;
	Edge carray[numVars];
	for (uint i=0; i<numVars; i++) {
		carray[i]=constraintIFF(This->cnf, ee0->variables[i], ee1->variables[i]);
	}
	return constraintAND(This->cnf, numVars, carray);
}

Edge encodeEnumOperatorPredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint) {
	PredicateOperator* predicate = (PredicateOperator*)constraint->predicate;
	uint numDomains=getSizeArraySet(&predicate->domains);

	FunctionEncodingType encType = constraint->encoding.type;
	bool generateNegation = encType == ENUMERATEIMPLICATIONSNEGATE;

	/* Call base encoders for children */
	for(uint i=0;i<numDomains;i++) {
		Element *elem = getArrayElement( &constraint->inputs, i);
		encodeElementSATEncoder(This, elem);
	}
	VectorEdge * clauses=allocDefVectorEdge(); // Setup array of clauses
	
	uint indices[numDomains]; //setup indices
	bzero(indices, sizeof(uint)*numDomains);
	
	uint64_t vals[numDomains]; //setup value array
	for(uint i=0;i<numDomains; i++) {
		Set * set=getArraySet(&predicate->domains, i);
		vals[i]=getSetElement(set, indices[i]);
	}
	
	bool notfinished=true;
	while(notfinished) {
		Edge carray[numDomains];

		if (evalPredicateOperator(predicate, vals) ^ generateNegation) {
			//Include this in the set of terms
			for(uint i=0;i<numDomains;i++) {
				Element * elem = getArrayElement(&constraint->inputs, i);
				carray[i] = getElementValueConstraint(This, elem, vals[i]);
			}
			Edge term=constraintAND(This->cnf, numDomains, carray);
			pushVectorEdge(clauses, term);
		}
		
		notfinished=false;
		for(uint i=0;i<numDomains; i++) {
			uint index=++indices[i];
			Set * set=getArraySet(&predicate->domains, i);

			if (index < getSetSize(set)) {
				vals[i]=getSetElement(set, index);
				notfinished=true;
				break;
			} else {
				indices[i]=0;
				vals[i]=getSetElement(set, 0);
			}
		}
	}
	if(getSizeVectorEdge(clauses) == 0)
		return E_False;
	Edge cor=constraintOR(This->cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	deleteVectorEdge(clauses);
	return generateNegation ? constraintNegate(cor) : cor;
}


void encodeOperatorElementFunctionSATEncoder(SATEncoder* This, ElementFunction* func) {
#ifdef TRACE_DEBUG
	model_print("Operator Function ...\n");
#endif
	FunctionOperator * function = (FunctionOperator *) func->function;
	uint numDomains=getSizeArrayElement(&func->inputs);

	/* Call base encoders for children */
	for(uint i=0;i<numDomains;i++) {
		Element *elem = getArrayElement( &func->inputs, i);
		encodeElementSATEncoder(This, elem);
	}

	VectorEdge * clauses=allocDefVectorEdge(); // Setup array of clauses
	
	uint indices[numDomains]; //setup indices
	bzero(indices, sizeof(uint)*numDomains);
	
	uint64_t vals[numDomains]; //setup value array
	for(uint i=0;i<numDomains; i++) {
		Set * set=getElementSet(getArrayElement(&func->inputs, i));
		vals[i]=getSetElement(set, indices[i]);
	}

	Edge overFlowConstraint = ((BooleanVar*) func->overflowstatus)->var;
	
	bool notfinished=true;
	while(notfinished) {
		Edge carray[numDomains+1];

		uint64_t result=applyFunctionOperator(function, numDomains, vals);
		bool isInRange = isInRangeFunction((FunctionOperator*)func->function, result);
		bool needClause = isInRange;
		if (function->overflowbehavior == OVERFLOWSETSFLAG || function->overflowbehavior == FLAGIFFOVERFLOW) {
			needClause=true;
		}
		
		if (needClause) {
			//Include this in the set of terms
			for(uint i=0;i<numDomains;i++) {
				Element * elem = getArrayElement(&func->inputs, i);
				carray[i] = getElementValueConstraint(This, elem, vals[i]);
			}
			if (isInRange) {
				carray[numDomains] = getElementValueConstraint(This, &func->base, result);
			}

			Edge clause;
			switch(function->overflowbehavior) {
			case IGNORE:
			case NOOVERFLOW:
			case WRAPAROUND: {
				clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), carray[numDomains]);
				break;
			}
			case FLAGFORCESOVERFLOW: {
				clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), constraintAND2(This->cnf, carray[numDomains], constraintNegate(overFlowConstraint)));
				break;
			}
			case OVERFLOWSETSFLAG: {
				if (isInRange) {
					clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), carray[numDomains]);
				} else {
					clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), overFlowConstraint);
				}
				break;
			}
			case FLAGIFFOVERFLOW: {
				if (isInRange) {
					clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), constraintAND2(This->cnf, carray[numDomains], constraintNegate(overFlowConstraint)));
				} else {
					clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), overFlowConstraint);
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
		
		notfinished=false;
		for(uint i=0;i<numDomains; i++) {
			uint index=++indices[i];
			Set * set=getElementSet(getArrayElement(&func->inputs, i));

			if (index < getSetSize(set)) {
				vals[i]=getSetElement(set, index);
				notfinished=true;
				break;
			} else {
				indices[i]=0;
				vals[i]=getSetElement(set, 0);
			}
		}
	}

	Edge cor=constraintAND(This->cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	addConstraintCNF(This->cnf, cor);
	deleteVectorEdge(clauses);
}

void encodeEnumEntriesTableElemFuncSATEncoder(SATEncoder* This, ElementFunction* func){
	UndefinedBehavior undefStatus = ((FunctionTable*) func->function)->undefBehavior;
	ASSERT(undefStatus == IGNOREBEHAVIOR || undefStatus == FLAGFORCEUNDEFINED);
	ArrayElement* elements= &func->inputs;
	Table* table = ((FunctionTable*) (func->function))->table;
	uint size = getSizeHashSetTableEntry(table->entrie);
	Edge constraints[size];
	HSIteratorTableEntry* iterator = iteratorTableEntry(table->entrie);
	uint i=0;
	while(hasNextTableEntry(iterator)) {
		TableEntry* entry = nextTableEntry(iterator);
		ASSERT(entry!=NULL);
		uint inputNum = getSizeArrayElement(elements);
		Edge carray[inputNum];
		for(uint j=0; j<inputNum; j++){
			Element* el= getArrayElement(elements, j);
			carray[j] = getElementValueConstraint(This, el, entry->inputs[j]);
		}
		Edge output = getElementValueConstraint(This, (Element*)func, entry->output);
		Edge row;
		switch(undefStatus ){
			case IGNOREBEHAVIOR: {
				row=constraintIMPLIES(This->cnf,constraintAND(This->cnf, inputNum, carray), output);
				break;
			}
			case FLAGFORCEUNDEFINED: {
				Edge undefConst = ((BooleanVar*)func->overflowstatus)->var;
				row=constraintIMPLIES(This->cnf,constraintAND(This->cnf, inputNum, carray), constraintAND2(This->cnf, output, constraintNegate(undefConst)));
				break;
			}
			default:
				ASSERT(0);
		
		}
		constraints[i++]=row;
	}
	deleteIterTableEntry(iterator);
	addConstraintCNF(This->cnf, constraintAND(This->cnf, size, constraints));
}

void encodeEnumTableElemFunctionSATEncoder(SATEncoder* This, ElementFunction* elemFunc){
#ifdef TRACE_DEBUG
	model_print("Enumeration Table functions ...\n");
#endif
	ASSERT(GETFUNCTIONTYPE(elemFunc->function)==TABLEFUNC);
	//First encode children
	ArrayElement* elements= &elemFunc->inputs;
	for(uint i=0; i<getSizeArrayElement(elements); i++){
		Element *elem = getArrayElement( elements, i);
		encodeElementSATEncoder(This, elem);
	}

	FunctionTable* function =(FunctionTable*)elemFunc;
	switch(function->undefBehavior){
		case IGNOREBEHAVIOR:
		case FLAGFORCEUNDEFINED:
			return encodeEnumEntriesTableElemFuncSATEncoder(This, elemFunc);
		default:
			break;
	}
	
	uint numDomains=getSizeArraySet(&function->table->domains);

	VectorEdge * clauses=allocDefVectorEdge(); // Setup array of clauses
	
	uint indices[numDomains]; //setup indices
	bzero(indices, sizeof(uint)*numDomains);
	
	uint64_t vals[numDomains]; //setup value array
	for(uint i=0;i<numDomains; i++) {
		Set * set=getArraySet(&function->table->domains, i);
		vals[i]=getSetElement(set, indices[i]);
	}

	Edge undefConstraint = ((BooleanVar*) elemFunc->overflowstatus)->var;
	
	bool notfinished=true;
	while(notfinished) {
		Edge carray[numDomains+1];
		TableEntry* tableEntry = getTableEntryFromTable(function->table, vals, numDomains);
		bool isInRange = tableEntry!=NULL;
		bool needClause = isInRange;
		if (function->undefBehavior == UNDEFINEDSETSFLAG || function->undefBehavior == FLAGIFFUNDEFINED) {
			needClause=true;
		}
		
		if (needClause) {
			//Include this in the set of terms
			for(uint i=0;i<numDomains;i++) {
				Element * elem = getArrayElement(&elemFunc->inputs, i);
				carray[i] = getElementValueConstraint(This, elem, vals[i]);
			}
			if (isInRange) {
				carray[numDomains] = getElementValueConstraint(This, (Element*)elemFunc, tableEntry->output);
			}

			Edge clause;
			switch(function->undefBehavior) {
				case UNDEFINEDSETSFLAG: {
					if (isInRange) {
						clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), carray[numDomains]);
					} else {
						clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), undefConstraint);
					}
					break;
				}
				case FLAGIFFUNDEFINED: {
					if (isInRange) {
						clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), constraintAND2(This->cnf, carray[numDomains], constraintNegate(undefConstraint)));
					} else {
						clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), undefConstraint);
					}
					break;
				}
				default:
					ASSERT(0);
			}
#ifdef TRACE_DEBUG
			model_print("added clause in table enumeration ...\n");
			printCNF(clause);
			model_print("\n");
#endif
			pushVectorEdge(clauses, clause);
		}
		
		notfinished=false;
		for(uint i=0;i<numDomains; i++) {
			uint index=++indices[i];
			Set * set=getArraySet(&function->table->domains, i);

			if (index < getSetSize(set)) {
				vals[i]=getSetElement(set, index);
				notfinished=true;
				break;
			} else {
				indices[i]=0;
				vals[i]=getSetElement(set, 0);
			}
		}
	}

	Edge cor=constraintAND(This->cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	addConstraintCNF(This->cnf, cor);
	deleteVectorEdge(clauses);
	
}
