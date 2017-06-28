#include "satencoder.h"
#include "structs.h"
#include "csolver.h"
#include "boolean.h"
#include "constraint.h"
#include "common.h"
#include "element.h"
#include "function.h"
#include "tableentry.h"
#include "table.h"
#include "order.h"


SATEncoder * allocSATEncoder() {
	SATEncoder *This=ourmalloc(sizeof (SATEncoder));
	allocInlineDefVectorConstraint(getSATEncoderAllConstraints(This));
	allocInlineDefVectorConstraint(getSATEncoderVars(This));
	This->varcount=1;
	return This;
}

void deleteSATEncoder(SATEncoder *This) {
	ourfree(This);
}

void encodeAllSATEncoder(SATEncoder * This, CSolver *csolver) {
	VectorBoolean *constraints=csolver->constraints;
	uint size=getSizeVectorBoolean(constraints);
	for(uint i=0;i<size;i++) {
		Boolean *constraint=getVectorBoolean(constraints, i);
		encodeConstraintSATEncoder(This, constraint);
	}
	
	size = getSizeVectorElement(csolver->allElements);
	for(uint i=0; i<size; i++){
		Element* element = getVectorElement(csolver->allElements, i);
		switch(GETELEMENTTYPE(element)){
			case ELEMFUNCRETURN: 
				encodeFunctionElementSATEncoder(This, (ElementFunction*) element);
				break;
			default:	
				continue;
				//ElementSets that aren't used in any constraints/functions
				//will be eliminated.
		}
	}
}

Constraint * encodeConstraintSATEncoder(SATEncoder *This, Boolean *constraint) {
	switch(GETBOOLEANTYPE(constraint)) {
	case ORDERCONST:
		return encodeOrderSATEncoder(This, (BooleanOrder *) constraint);
	case BOOLEANVAR:
		return encodeVarSATEncoder(This, (BooleanVar *) constraint);
	case LOGICOP:
		return encodeLogicSATEncoder(This, (BooleanLogic *) constraint);
	case PREDICATEOP:
		return encodePredicateSATEncoder(This, (BooleanPredicate *) constraint);
	default:
		model_print("Unhandled case in encodeConstraintSATEncoder %u", GETBOOLEANTYPE(constraint));
		exit(-1);
	}
}

void getArrayNewVarsSATEncoder(SATEncoder* encoder, uint num, Constraint **carray) {
	for(uint i=0;i<num;i++)
		carray[i]=getNewVarSATEncoder(encoder);
}

Constraint * getNewVarSATEncoder(SATEncoder *This) {
	Constraint * var=allocVarConstraint(VAR, This->varcount);
	Constraint * varneg=allocVarConstraint(NOTVAR, This->varcount++);
	setNegConstraint(var, varneg);
	setNegConstraint(varneg, var);
	pushVectorConstraint(getSATEncoderVars(This), var);
	return var;
}

Constraint * encodeVarSATEncoder(SATEncoder *This, BooleanVar * constraint) {
	if (constraint->var == NULL) {
		constraint->var=getNewVarSATEncoder(This);
	}
	return constraint->var;
}

Constraint * encodeLogicSATEncoder(SATEncoder *This, BooleanLogic * constraint) {
	Constraint * array[getSizeArrayBoolean(&constraint->inputs)];
	for(uint i=0;i<getSizeArrayBoolean(&constraint->inputs);i++)
		array[i]=encodeConstraintSATEncoder(This, getArrayBoolean(&constraint->inputs, i));

	switch(constraint->op) {
	case L_AND:
		return allocArrayConstraint(AND, getSizeArrayBoolean(&constraint->inputs), array);
	case L_OR:
		return allocArrayConstraint(OR, getSizeArrayBoolean(&constraint->inputs), array);
	case L_NOT:
		ASSERT(constraint->numArray==1);
		return negateConstraint(array[0]);
	case L_XOR: {
		ASSERT(constraint->numArray==2);
		Constraint * nleft=negateConstraint(cloneConstraint(array[0]));
		Constraint * nright=negateConstraint(cloneConstraint(array[1]));
		return allocConstraint(OR,
													 allocConstraint(AND, array[0], nright),
													 allocConstraint(AND, nleft, array[1]));
	}
	case L_IMPLIES:
		ASSERT(constraint->numArray==2);
		return allocConstraint(IMPLIES, array[0], array[1]);
	default:
		model_print("Unhandled case in encodeLogicSATEncoder %u", constraint->op);
		exit(-1);
	}
}

Constraint * encodeOrderSATEncoder(SATEncoder *This, BooleanOrder * constraint) {
	if(constraint->var== NULL){
		constraint->var = getNewVarSATEncoder(This);
	}
	return constraint->var;
}

Constraint * encodePredicateSATEncoder(SATEncoder * This, BooleanPredicate * constraint) {
	//TO IMPLEMENT
	
	return NULL;
}

Constraint* encodeFunctionElementSATEncoder(SATEncoder* encoder, ElementFunction *This){
	switch(GETFUNCTIONTYPE(This->function)){
		case TABLEFUNC:
			return encodeTableElementFunctionSATEncoder(encoder, This);
		case OPERATORFUNC:
			return encodeOperatorElementFunctionSATEncoder(encoder, This);
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint* encodeTableElementFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This){
	switch(getElementFunctionEncoding(This)->type){
		case ENUMERATEIMPLICATIONS:
			return encodeEnumTableElemFunctionSATEncoder(encoder, This);
			break;
		default:
			ASSERT(0);
	}
	return NULL;
}

Constraint* encodeOperatorElementFunctionSATEncoder(SATEncoder* encoder,ElementFunction* This){
	return NULL;
}

Constraint* encodeEnumTableElemFunctionSATEncoder(SATEncoder* encoder, ElementFunction* This){
	ASSERT(GETFUNCTIONTYPE(This->function)==TABLEFUNC);
	ArrayElement* elements= &This->inputs;
	Table* table = ((FunctionTable*) (This->function))->table;
	uint size = getSizeVectorTableEntry(&table->entries);
	Constraint* constraints[size]; //FIXME: should add a space for the case that didn't match any entries
	for(uint i=0; i<size; i++){
		TableEntry* entry = getVectorTableEntry(&table->entries, i);
		uint inputNum =getSizeArrayElement(elements);
		Element* el= getArrayElement(elements, i);
		Constraint* carray[inputNum];
		for(uint j=0; j<inputNum; j++){
			 carray[inputNum] = getElementValueConstraint(el, entry->inputs[j]);
		}
		Constraint* row= allocConstraint(IMPLIES, allocArrayConstraint(AND, inputNum, carray),
			getElementValueConstraint((Element*)This, entry->output));
		constraints[i]=row;
	}
	Constraint* result = allocArrayConstraint(OR, size, constraints);
	pushVectorConstraint( getSATEncoderAllConstraints(encoder), result);
	return result;
}