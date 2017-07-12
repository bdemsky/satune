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

Edge encodeEnumTablePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint){
	VectorTableEntry* entries = &(((PredicateTable*)constraint->predicate)->table->entries);
	FunctionEncodingType encType = constraint->encoding.type;
	ArrayElement* inputs = &constraint->inputs;
	uint inputNum =getSizeArrayElement(inputs);
	//Encode all the inputs first ...
	for(uint i=0; i<inputNum; i++){
		encodeElementSATEncoder(This, getArrayElement(inputs, i));
	}
	
	//WARNING: THIS ASSUMES PREDICATE TABLE IS COMPLETE...SEEMS UNLIKELY TO BE SAFE IN MANY CASES...
	//WONDER WHAT BEST WAY TO HANDLE THIS IS...
	
	uint size = getSizeVectorTableEntry(entries);
	bool generateNegation = encType == ENUMERATEIMPLICATIONSNEGATE;
	Edge constraints[size];
	for(uint i=0; i<size; i++){
		TableEntry* entry = getVectorTableEntry(entries, i);
		if(generateNegation == entry->output) {
			//Skip the irrelevant entries
			continue;
		}
		Edge carray[inputNum];
		for(uint j=0; j<inputNum; j++){
			Element* el = getArrayElement(inputs, j);
			carray[j] = getElementValueConstraint(This, el, entry->inputs[j]);
		}
		constraints[i]=constraintAND(This->cnf, inputNum, carray);
	}
	Edge result=constraintOR(This->cnf, size, constraints);

	return generateNegation ? result: constraintNegate(result);
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
			pushVectorEdge(clauses, constraintAND(This->cnf, numDomains, carray));
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

	Edge cor=constraintOR(This->cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	deleteVectorEdge(clauses);
	return generateNegation ? cor : constraintNegate(cor);
}


void encodeOperatorElementFunctionSATEncoder(SATEncoder* This, ElementFunction* func) {
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
		Edge carray[numDomains+2];

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
				clause=constraintAND(This->cnf, numDomains+1, carray);
				break;
			}
			case FLAGFORCESOVERFLOW: {
				carray[numDomains+1]=constraintNegate(overFlowConstraint);
				clause=constraintAND(This->cnf, numDomains+2, carray);
				break;
			}
			case OVERFLOWSETSFLAG: {
				if (isInRange) {
					clause=constraintAND(This->cnf, numDomains+1, carray);
				} else {
					carray[numDomains+1]=overFlowConstraint;
					clause=constraintAND(This->cnf, numDomains+1, carray);
				}
				break;
			}
			case FLAGIFFOVERFLOW: {
				if (isInRange) {
				carray[numDomains+1]=constraintNegate(overFlowConstraint);
					clause=constraintAND(This->cnf, numDomains+2, carray);
				} else {
					carray[numDomains+1]=overFlowConstraint;
					clause=constraintAND(This->cnf, numDomains+1, carray);
				}
				break;
			}
			default:
				ASSERT(0);
			}
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

	Edge cor=constraintOR(This->cnf, getSizeVectorEdge(clauses), exposeArrayEdge(clauses));
	addConstraintCNF(This->cnf, cor);
	deleteVectorEdge(clauses);
}

void encodeEnumTableElemFunctionSATEncoder(SATEncoder* This, ElementFunction* func){
	//FIXME: HANDLE UNDEFINED BEHAVIORS
	ASSERT(GETFUNCTIONTYPE(func->function)==TABLEFUNC);
	ArrayElement* elements= &func->inputs;
	Table* table = ((FunctionTable*) (func->function))->table;
	uint size = getSizeVectorTableEntry(&table->entries);
	Edge constraints[size]; //FIXME: should add a space for the case that didn't match any entries
	for(uint i=0; i<size; i++) {
		TableEntry* entry = getVectorTableEntry(&table->entries, i);
		uint inputNum = getSizeArrayElement(elements);
		Edge carray[inputNum];
		for(uint j=0; j<inputNum; j++){
			Element* el= getArrayElement(elements, j);
			carray[j] = getElementValueConstraint(This, el, entry->inputs[j]);
		}
		Edge output = getElementValueConstraint(This, (Element*)func, entry->output);
		Edge row= constraintIMPLIES(This->cnf, constraintAND(This->cnf, inputNum, carray), output);
		constraints[i]=row;
	}
	addConstraintCNF(This->cnf, constraintOR(This->cnf, size, constraints));
}
