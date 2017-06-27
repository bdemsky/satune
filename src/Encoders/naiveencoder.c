#include "naiveencoder.h"
#include "elementencoding.h"
#include "element.h"
#include "functionencoding.h"
#include "function.h"
#include "set.h"
#include "common.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "table.h"
#include "tableentry.h"
//THIS FILE SHOULD HAVE NOTHING TO DO WITH CONSTRAINTS...
//#include "constraint.h"
#include <strings.h>

NaiveEncoder* allocNaiveEncoder(){
	NaiveEncoder* encoder = (NaiveEncoder*) ourmalloc(sizeof(NaiveEncoder));
	allocInlineDefVectorConstraint(GETNAIVEENCODERALLCONSTRAINTS(encoder));
	allocInlineDefVectorConstraint(GETNAIVEENCODERVARS(encoder));
	encoder->varindex=0;
	return encoder;
}

void naiveEncodingDecision(CSolver* csolver, NaiveEncoder* encoder){
	uint size = getSizeVectorElement(csolver->allElements);
	for(uint i=0; i<size; i++){
		Element* element = getVectorElement(csolver->allElements, i);
		switch(GETELEMENTTYPE(element)){
			case ELEMSET:
				setElementEncodingType(GETELEMENTENCODING(element), BINARYINDEX);
				baseBinaryIndexElementAssign(GETELEMENTENCODING(element));
				generateElementEncodingVariables(encoder,GETELEMENTENCODING(element));
				break;
			case ELEMFUNCRETURN: 
				setFunctionEncodingType(GETFUNCTIONENCODING(element), ENUMERATEIMPLICATIONS);
				break;
			default:
				ASSERT(0);
		}
	}
	
	size = getSizeVectorBoolean(csolver->allBooleans);
	for(uint i=0; i<size; i++){
		Boolean* predicate = getVectorBoolean(csolver->allBooleans, i);
		switch(GETBOOLEANTYPE(predicate)){
			case PREDICATEOP:
				setFunctionEncodingType(GETFUNCTIONENCODING(predicate), ENUMERATEIMPLICATIONS);
				break;
			default:
				continue;
		} 
	}
}


// THIS SHOULD NOT BE HERE
/*
void getArrayNewVars(NaiveEncoder* encoder, uint num, Constraint **carray) {
	for(uint i=0;i<num;i++)
		carray[i]=getNewVar(encoder);
}
*/

// THIS SHOULD NOT BE HERE
/*
Constraint * getNewVar(NaiveEncoder* encoder) {
	Constraint* var = allocVarConstraint(VAR, encoder->varindex);
	Constraint* notVar = allocVarConstraint(NOTVAR, encoder->varindex);
	setNegConstraint(var, notVar);
	setNegConstraint(notVar, var);
	pushVectorConstraint(GETNAIVEENCODERVARS(encoder), var);	
	encoder->varindex++;
	return var;
}
*/

void baseBinaryIndexElementAssign(ElementEncoding *This) {
	Element * element=This->element;
	ASSERT(element->type == ELEMSET);
	Set * set= ((ElementSet*)element)->set;
	ASSERT(set->isRange==false);
	uint size=getSizeVectorInt(set->members);
	uint encSize=NEXTPOW2(size);
	allocEncodingArrayElement(This, encSize);
	allocInUseArrayElement(This, encSize);

	for(uint i=0;i<size;i++) {
		This->encodingArray[i]=getVectorInt(set->members, i);
		setInUseElement(This, i);
	}
	This->numVars = NUMBITS(size-1);
	This->variables = ourmalloc(sizeof(Constraint*)* This->numVars);
	
	
}


void encode(CSolver* csolver){
	NaiveEncoder* encoder = allocNaiveEncoder();
	naiveEncodingDecision( csolver, encoder);
	uint size = getSizeVectorElement(csolver->allElements);
	for(uint i=0; i<size; i++){
		Element* element = getVectorElement(csolver->allElements, i);
		switch(GETELEMENTTYPE(element)){
			case ELEMFUNCRETURN: 
				naiveEncodeFunctionPredicate(encoder, GETFUNCTIONENCODING(element));
				break;
			default:
				continue;
		}
	}
	
	size = getSizeVectorBoolean(csolver->allBooleans);
	for(uint i=0; i<size; i++){
		Boolean* predicate = getVectorBoolean(csolver->allBooleans, i);
		switch(GETBOOLEANTYPE(predicate)){
			case PREDICATEOP:
				naiveEncodeFunctionPredicate(encoder, GETFUNCTIONENCODING(predicate));
				break;
			default:
				continue;
		} 
	}
}

void naiveEncodeFunctionPredicate(NaiveEncoder* encoder, FunctionEncoding *This){
	if(This->isFunction) {
		ASSERT(GETELEMENTTYPE(This->op.function)==ELEMFUNCRETURN);
		switch(This->type){
			case ENUMERATEIMPLICATIONS:
				naiveEncodeEnumeratedFunction(encoder, This);
				break;
			case CIRCUIT:
				naiveEncodeCircuitFunction(encoder, This);
				break;
			default:
				ASSERT(0);
		}
	}else {
		ASSERT(GETBOOLEANTYPE(This->op.predicate) == PREDICATEOP);
		BooleanPredicate* predicate = (BooleanPredicate*)This->op.predicate;
		//FIXME
		
	}
}

void naiveEncodeEnumeratedFunction(NaiveEncoder* encoder, FunctionEncoding* This){
	ElementFunction* ef =(ElementFunction*)This->op.function;
	switch(GETFUNCTIONTYPE(ef->function)){
		case TABLEFUNC:
			naiveEncodeEnumTableFunc(encoder, ef);
			break;
		case OPERATORFUNC:
			naiveEncodeEnumOperatingFunc(encoder, ef);
			break;
		default:
			ASSERT(0);
	} 
}

void naiveEncodeEnumTableFunc(NaiveEncoder* encoder, ElementFunction* This){
	ASSERT(GETFUNCTIONTYPE(This->function)==TABLEFUNC);
	ArrayElement* elements= &This->inputs;
	Table* table = ((FunctionTable*) (This->function))->table;
	uint size = getSizeVectorTableEntry(&table->entries);
	for(uint i=0; i<size; i++){
		TableEntry* entry = getVectorTableEntry(&table->entries, i);
		uint inputNum =getSizeArrayElement(elements);
		Element* el= getArrayElement(elements, i);
		Constraint* carray[inputNum];
		for(uint j=0; j<inputNum; j++){
			 carray[inputNum] = getElementValueConstraint(el, entry->inputs[j]);
		}
		Constraint* row= allocConstraint(IMPLIES, allocArrayConstraint(AND, inputNum, carray),
			getElementValueConstraint(table->range, entry->output));
		pushVectorConstraint( GETNAIVEENCODERALLCONSTRAINTS(encoder), row);
	}
	
}

void naiveEncodeEnumOperatingFunc(NaiveEncoder* encoder, ElementFunction* This){
	
}


void naiveEncodeCircuitFunction(NaiveEncoder* encoder, FunctionEncoding* This){
	
}

void deleteNaiveEncoder(NaiveEncoder* encoder){
	deleteVectorArrayConstraint(&encoder->vars);
}
