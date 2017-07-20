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
