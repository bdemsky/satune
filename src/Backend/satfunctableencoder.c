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
	ASSERT(GETPREDICATETYPE(constraint->predicate) == TABLEPRED);
	UndefinedBehavior undefStatus = ((PredicateTable*)constraint->predicate)->undefinedbehavior;
	ASSERT(undefStatus == IGNOREBEHAVIOR || undefStatus == FLAGFORCEUNDEFINED);
	Table* table = ((PredicateTable*)constraint->predicate)->table;
	FunctionEncodingType encType = constraint->encoding.type;
	ArrayElement* inputs = &constraint->inputs;
	uint inputNum =getSizeArrayElement(inputs);
	uint size = getSizeHashSetTableEntry(table->entries);
	bool generateNegation = encType == ENUMERATEIMPLICATIONSNEGATE;
	Edge constraints[size];
	HSIteratorTableEntry* iterator = iteratorTableEntry(table->entries);
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
		Edge row;
		switch(undefStatus){
			case IGNOREBEHAVIOR:
				row=constraintAND(This->cnf, inputNum, carray);
				break;
			case FLAGFORCEUNDEFINED:{
				Edge undefConst = encodeConstraintSATEncoder(This, constraint->undefStatus);
				row=constraintIMPLIES(This->cnf,constraintAND(This->cnf, inputNum, carray),  constraintNegate(undefConst));
				break;
			}
			default:
				ASSERT(0);
		}
		constraints[i++] = row;
		
	}
	Edge result=constraintOR(This->cnf, size, constraints);

	return generateNegation ? constraintNegate(result) : result;
}
Edge encodeEnumTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
#ifdef TRACE_DEBUG
	model_print("Enumeration Table Predicate ...\n");
#endif
	ASSERT(GETPREDICATETYPE(constraint->predicate) == TABLEPRED);
	//First encode children
	ArrayElement* inputs = &constraint->inputs;
	uint inputNum =getSizeArrayElement(inputs);
	//Encode all the inputs first ...
	for(uint i=0; i<inputNum; i++){
		encodeElementSATEncoder(This, getArrayElement(inputs, i));
	}
	PredicateTable* predicate = (PredicateTable*)constraint->predicate;
	switch(predicate->undefinedbehavior){
		case IGNOREBEHAVIOR:
		case FLAGFORCEUNDEFINED:
			return encodeEnumEntriesTablePredicateSATEncoder(This, constraint);
		default:
			break;
	}
	bool generateNegation = constraint->encoding.type == ENUMERATEIMPLICATIONSNEGATE;
	uint numDomains=getSizeArraySet(&predicate->table->domains);

	VectorEdge * clauses=allocDefVectorEdge(); // Setup array of clauses
	
	uint indices[numDomains]; //setup indices
	bzero(indices, sizeof(uint)*numDomains);
	
	uint64_t vals[numDomains]; //setup value array
	for(uint i=0;i<numDomains; i++) {
		Set * set=getArraySet(&predicate->table->domains, i);
		vals[i]=getSetElement(set, indices[i]);
	}

	Edge undefConstraint = encodeConstraintSATEncoder (This, constraint->undefStatus);
	
	bool notfinished=true;
	while(notfinished) {
		Edge carray[numDomains];
		TableEntry* tableEntry = getTableEntryFromTable(predicate->table, vals, numDomains);
		bool isInRange = tableEntry!=NULL;
		bool ignoreEntry = generateNegation == tableEntry->output;
		ASSERT(predicate->undefinedbehavior == UNDEFINEDSETSFLAG || predicate->undefinedbehavior == FLAGIFFUNDEFINED);
		//Include this in the set of terms
		for(uint i=0;i<numDomains;i++) {
			Element * elem = getArrayElement(&constraint->inputs, i);
			carray[i] = getElementValueConstraint(This, elem, vals[i]);
		}

		Edge clause;
		switch(predicate->undefinedbehavior) {
			case UNDEFINEDSETSFLAG: {
				if (isInRange && !ignoreEntry) {
					clause=constraintAND(This->cnf, numDomains, carray);
				} else if(!isInRange) {
					clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), undefConstraint);
				}
				break;
			}
			case FLAGIFFUNDEFINED: {
				if (isInRange && !ignoreEntry) {
					clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), constraintAND2(This->cnf, carray[numDomains], constraintNegate(undefConstraint)));
				} else if(!isInRange) {
					clause=constraintIMPLIES(This->cnf,constraintAND(This->cnf, numDomains, carray), undefConstraint);
				}
				break;
			}
			default:
				ASSERT(0);
		}
#ifdef TRACE_DEBUG
		model_print("added clause in predicate table enumeration ...\n");
		printCNF(clause);
		model_print("\n");
#endif
		pushVectorEdge(clauses, clause);
		
		
		notfinished=false;
		for(uint i=0;i<numDomains; i++) {
			uint index=++indices[i];
			Set * set=getArraySet(&predicate->table->domains, i);

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
	if(getSizeVectorEdge(clauses) == 0){
		deleteVectorEdge(clauses);
		return E_False;
	}
	Edge cor=constraintOR(This->cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	deleteVectorEdge(clauses);
	return generateNegation ? constraintNegate(cor) : cor;
}

void encodeEnumEntriesTableElemFuncSATEncoder(SATEncoder* This, ElementFunction* func){
	UndefinedBehavior undefStatus = ((FunctionTable*) func->function)->undefBehavior;
	ASSERT(undefStatus == IGNOREBEHAVIOR || undefStatus == FLAGFORCEUNDEFINED);
	ArrayElement* elements= &func->inputs;
	Table* table = ((FunctionTable*) (func->function))->table;
	uint size = getSizeHashSetTableEntry(table->entries);
	Edge constraints[size];
	HSIteratorTableEntry* iterator = iteratorTableEntry(table->entries);
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
				Edge undefConst = encodeConstraintSATEncoder(This, func->overflowstatus);
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

	FunctionTable* function =(FunctionTable*)elemFunc->function;
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

	Edge undefConstraint = encodeConstraintSATEncoder(This, elemFunc->overflowstatus);
	bool notfinished=true;
	while(notfinished) {
		Edge carray[numDomains+1];
		TableEntry* tableEntry = getTableEntryFromTable(function->table, vals, numDomains);
		bool isInRange = tableEntry!=NULL;
		ASSERT(function->undefBehavior == UNDEFINEDSETSFLAG || function->undefBehavior == FLAGIFFUNDEFINED);
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
		model_print("added clause in function table enumeration ...\n");
		printCNF(clause);
		model_print("\n");
#endif
		pushVectorEdge(clauses, clause);

		
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
	if(getSizeVectorEdge(clauses) == 0){
		deleteVectorEdge(clauses);
		return;
	}
	Edge cor=constraintAND(This->cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	addConstraintCNF(This->cnf, cor);
	deleteVectorEdge(clauses);
	
}